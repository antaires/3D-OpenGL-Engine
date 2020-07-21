#include "Game.h"
#include "SDL2/SDL_image.h"
#include "Constants.h"
#include "SpriteComponent.h"
#include "BackgroundSpriteComponent.h"
#include "CollisionDetection.h"
#include "TileMapComponent.h"
#include "Map.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include <GL/glew.h>

#include <algorithm>

#include <iostream> // remove

Game::Game()
  : m_Window(nullptr)
  , m_DeltaTime(0)
  , m_IsRunning(true)
  , m_UpdatingActors(false)
  , m_TicksCount(0)
  , m_Map(nullptr)
  , m_Player(nullptr)
  , m_EnemySpawnTimer(0.0f)
  , m_SpriteVerts(nullptr)
  , m_SpriteShader(nullptr)
  // TODO consider setting enemy vector initial size ...
{}

bool Game::Initialize()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    SDL_Log("Unable to Initialize SDL: %s", SDL_GetError());
    return false;
  }

  // set up openGL attributes, returns 0 if succes
  // use the core openGL profile
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  // specifiy version 3.3
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  // request color buffer with 8-bits per RGBA channel
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  // enable double buffering
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  // force OpenGL to sue hardware acceleration
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  m_Window = SDL_CreateWindow(
    "Asteroids"
    , 100   // top left  x-coord
    , 100   // top left  y-coord
    , SCREEN_WIDTH  // width
    , SCREEN_HEIGHT   // height
    , SDL_WINDOW_OPENGL     // use openGL
  );
  if (!m_Window)
  {
    SDL_Log("Failed to create window: %s", SDL_GetError());
    return false;
  }

  // create open GL context and saves it to member variable
  m_Context = SDL_GL_CreateContext(m_Window);

  // required for mac! without this, a rectangle is drawn as a rhombus
  int screenWidth, screenHeight;
  SDL_GL_GetDrawableSize(m_Window, &screenWidth, &screenHeight );
  glViewport(0, 0, screenWidth, screenHeight); // adjust to high density screen

  // init GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    SDL_Log("Failed to initialize GLEW");
    return false;
  }
  glGetError(); // clears benign error code

  if(!LoadShaders())
  {
    SDL_Log("Failed to load shaders");
    return false;
  }

  // create quad for drawing sprites
  CreateSpriteVerts();

  LoadData();

  m_TicksCount = SDL_GetTicks();

  return true;
}

void Game::RunLoop()
{
  while(m_IsRunning)
  {
    ProcessInput();
    UpdateGame();
    HandleCollisions();
    GenerateOutput();
  }
}

void Game::ProcessInput()
{
  SDL_Event event;

  // loop over events in event queue
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      // handle all event types
      case SDL_QUIT:
        m_IsRunning = false;
        break;
    }
  }

  // get state of keyboard
  const Uint8* keyState = SDL_GetKeyboardState(NULL);

  // if escape pressed, exit Game RunLoop
  if (keyState[SDL_SCANCODE_ESCAPE])
  {
    m_IsRunning = false;
  }

  // get mouse state
  int mouseX, mouseY;
  const uint32_t mouseState = SDL_GetMouseState(&mouseX, &mouseY);

  // handle input
  m_UpdatingActors = true;
  for(auto actor: m_Actors)
  {
    actor->ProcessInput(keyState);
    actor->ProcessMouse(mouseState, mouseX, mouseY);
  }
  m_UpdatingActors = false;
}

void Game::UpdateGame()
{
  // frame limiting : wait until 16 ms elapsed
  while(!SDL_TICKS_PASSED(SDL_GetTicks(), m_TicksCount + 16));

  // deltaTime is difference in ticks from last frame
  m_DeltaTime = (SDL_GetTicks() - m_TicksCount) / 1000.0f;

  // clamp max delta time value (to avoid jumping ahead during debug)
  if (m_DeltaTime > 0.05f)
  {
    m_DeltaTime = 0.05f;
  }
  m_TicksCount = SDL_GetTicks();

  // update objects in game world as function of delta time
  m_UpdatingActors = true;
  for(auto actor: m_Actors)
  {
    actor->Update(m_DeltaTime);
  }
  m_UpdatingActors = false;

  // move pending actors to actors and clear
  for(auto pending : m_PendingActors)
  {
    pending->ComputeWorldTransform();
    m_Actors.emplace_back(pending);
  }
  m_PendingActors.clear();

  // add dead actors to temp vector
  std::vector<Actor*> deadActors;
  for(auto actor : m_Actors)
  {
    if (actor->GetState() == Actor::E_Dead)
    {
      deadActors.emplace_back(actor);
    }
  }

  // delete dead actors
  for(auto actor : deadActors)
  {
    // TODO delete properly from actors
    auto it = std::find(m_Actors.begin(), m_Actors.end(), actor);
    if (it != m_Actors.end())
    {
      m_Actors.erase(it);
    }
  }

  // add new enemies on timer
  m_EnemySpawnTimer += (1 * m_DeltaTime);
  if(m_EnemySpawnTimer > 1)
  {
    m_EnemySpawnTimer = 0;
    Enemy* enemy = new Enemy(this, m_Map); // scale set inside class
    enemy->SetPosition(Vector2(
      m_Map->GetRand(-SCREEN_WIDTH/2.0f, SCREEN_WIDTH/2.0f)
      , m_Map->GetRand(-SCREEN_HEIGHT/2.0f, SCREEN_HEIGHT/2.0f)
    ));
    m_Enemies.push_back(enemy);
  }
}

void Game::HandleCollisions()
{
  // player against enemy (ies)
  // check if collide with Player - if yes then kill player
  if(m_Player != nullptr && m_Player->GetState() == Actor::E_Active)
  {
    for(auto enemy : m_Enemies)
    {
      if (enemy != nullptr && enemy->GetState() == Actor::E_Active)
      {
        if( CollisionDetection::HasCollision(enemy->GetCircle(), m_Player->GetCircle()) )
        {
          m_Player->SetState(Actor::E_Dead);
        }
      }
    }
  }

}

void Game::AddActor(Actor* actor)
{
  if (m_UpdatingActors)
  {
    m_PendingActors.emplace_back(actor);
  } else {
    m_Actors.emplace_back(actor);
  }
}

void Game::RemoveActor(Actor* actor)
{
  // check both pending and active actors
  auto it = std::find(m_PendingActors.begin(), m_PendingActors.end(), actor);
  if (it != m_PendingActors.end())
  {
    // swap to end of vector and pop off (avoid erase copies)
    std::iter_swap(it, m_PendingActors.end() -1);
    m_PendingActors.pop_back();
  }

  it = std::find(m_Actors.begin(), m_Actors.end(), actor);
  if (it != m_Actors.end())
  {
    std::iter_swap(it, m_Actors.end() -1);
    m_Actors.pop_back();
  }
}

// hard coded for now, TODO: load from files and binary
void Game::LoadData()
{
  // load all textures

  // create path map (barriers and walls) and AI
  m_Map = new Map("assets/testMap01.csv");
  m_Map->SetMode(Map::E_BFS); // choose pathfinding algorithm

  // create player
  m_Player = new Player(this); // scale set inside class
  m_Player->SetPosition(Vector2(SCREEN_WIDTH/2, SCREEN_HEIGHT/3));

  // create initial enemies
  for(int i = 0; i < 5; ++i)
  {
    Enemy* enemy = new Enemy(this, m_Map); // scale set inside class
    enemy->SetPosition(Vector2(
      m_Map->GetRand(-SCREEN_WIDTH/2.0f, SCREEN_WIDTH/2.0f)
      , m_Map->GetRand(-SCREEN_HEIGHT/2.0f, SCREEN_HEIGHT/2.0f)
    ));
    m_Enemies.push_back(enemy);
  }

  // create background tile map
  Actor* tileMapActor = new Actor(this);
  tileMapActor->SetPosition(Vector2(SCREEN_WIDTH/2, SCREEN_HEIGHT/2));
  TileMapComponent* tileMapComponent = new TileMapComponent(tileMapActor, 10);
  // load CSV
  tileMapComponent->LoadCsv("assets/testMap01.csv");
  // set texture
  tileMapComponent->SetTextureRowsCols( 24, 8);
  tileMapComponent->SetTexture(GetTexture("assets/tiles.png"));
}

void Game::UnloadData()
{
  while(!m_Actors.empty())
  {
    delete m_Actors.back();
  }

  for(auto t : m_Textures)
  {
    t.second->Unload();
    // SDL_DestroyTexture(t.second);
  }
  m_Textures.clear();
}

void Game::CreateSpriteVerts()
{
  // V texture coord is flipped to account for how openGL expects image data upsidedown
  // pos = 3 #s, UV coords = 2 #s
  float vertexBuffer[] = {
    -0.5f,  0.5f, 0.f, 0.f, 0.f, // top left
		 0.5f,  0.5f, 0.f, 1.f, 0.f, // top right
		 0.5f, -0.5f, 0.f, 1.f, 1.f, // bottom right
		-0.5f, -0.5f, 0.f, 0.f, 1.f // bottom left
  };

	unsigned int indexBuffer[] = {
		0, 1, 2,
		2, 3, 0
	};

  m_SpriteVerts = new VertexArray(vertexBuffer, 4, indexBuffer, 6);
}

bool Game::LoadShaders()
{
  m_SpriteShader = new Shader();
  if(!m_SpriteShader->Load("shaders/Sprite.vert", "shaders/Sprite.frag"))
  {
    return false;
  }
  m_SpriteShader->SetActive();

  // Matrix4 viewProj = Matrix4::CreateSimpleViewProj(m_ScreenWidth, m_ScreenHeight);
  Matrix4 viewProj = Matrix4::CreateSimpleViewProj((float) SCREEN_WIDTH, (float) SCREEN_HEIGHT);
  m_SpriteShader->SetMatrixUniform("uViewProj", viewProj);

  return true;
}

Texture* Game::GetTexture(const std::string& fileName)
{
  Texture* texture = nullptr;

  // is texture already loaded?
  auto it = m_Textures.find(fileName);
  if (it != m_Textures.end())
  {
    texture = it->second;
  } else {
    // load texture
    texture = new Texture();
    if (texture->Load(fileName))
    {
      m_Textures.emplace(fileName, texture);
    } else {
      delete texture;
      texture = nullptr;
    }
  }
  return texture;
}


void Game::AddSprite(SpriteComponent* sprite)
{
  // insert by sorted order
  int myDrawOrder = sprite->GetDrawOrder();
  auto it = m_Sprites.begin();
  for(; it != m_Sprites.end(); ++it)
  {
    if (myDrawOrder < (*it)->GetDrawOrder())
    {
      break;
    }
  }
  m_Sprites.insert(it, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
  // cant use swap here because need to preserve ordering
  auto it = std::find(m_Sprites.begin(), m_Sprites.end(), sprite);
  if (it != m_Sprites.end())
  {
    m_Sprites.erase(it);
  }
}

void Game::GenerateOutput()
{
  // modulate color based on deltaTime
  static float r = 0;
  static float g = 0;
  static float b = 0;
  static float changeFactor = 0.1;
  b+=changeFactor * m_DeltaTime;
  if (b > 1.0f || b < 0){g += changeFactor * m_DeltaTime;}
  if (g > 1.0f || g < 0){r += changeFactor * m_DeltaTime;}
  if (r > 1.0f || r < 0){ changeFactor *= -1;}

  glClearColor(r, g, b, 1.0f);

  // clear color buffer
  glClear(GL_COLOR_BUFFER_BIT);

  // draw scene
  // set sprite shader and vertex array objects active
  m_SpriteShader->SetActive();
  m_SpriteVerts->SetActive();

  // enable alpha blending
  glEnable(GL_BLEND);
  glBlendFunc(
    GL_SRC_ALPHA              // srcFactor is srcAlpha
    , GL_ONE_MINUS_SRC_ALPHA  // dstFactor is 1 - srcAlpha
  );

  // draw all sprites
  for(auto sprite: m_Sprites)
  {
    sprite->Draw(m_SpriteShader);
  }

  // swap buffers, which also displays the scene
  SDL_GL_SwapWindow(m_Window);
}

Vector2 Game::GetPlayerPosition() const
{
  return m_Player->GetPosition();
}

class Player* Game::GetPlayer()
{
  return m_Player;
}


std::vector<Enemy*> Game::GetEnemies()
{
  return m_Enemies;
}

bool Game::CollidesWithBarrier(Vector2 pos, float width, float height)
{
  return m_Map->CollidesWithBarrier(pos, width, height);
}

void Game::ShutDown()
{
  UnloadData();
  IMG_Quit();
  SDL_GL_DeleteContext(m_Context);
  SDL_DestroyWindow(m_Window);
  SDL_Quit();
}
