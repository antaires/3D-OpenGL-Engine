#include "Game.h"
#include "SDL2/SDL_image.h"
#include "Actor.h"
#include "Constants.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "PlaneActor.h"
#include "InputSystem.h"
#include "FPSActor.h"

#include <GL/glew.h>
#include <algorithm>

#include <iostream> // remove

Game::Game()
  : m_Renderer(nullptr)
  , m_TicksCount(0)
  , m_IsRunning(true)
  , m_UpdatingActors(false)
  , m_InputSystem(nullptr)
{}

bool Game::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
  {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return false;
  }

  // Create the renderer
  m_Renderer = new Renderer(this);
  if (!m_Renderer->Initialize((float) SCREEN_WIDTH, (float) SCREEN_HEIGHT))
  {
    SDL_Log("Failed to initialize renderer");
    delete m_Renderer;
    m_Renderer = nullptr;
    return false;
  }

  LoadData();
  m_TicksCount = SDL_GetTicks();

  // set up input system
  m_InputSystem = new InputSystem();
  m_InputSystem->Initialize();
  // GAME SPECIFIC (relative mouse movement for rotating FPS actor)
  m_InputSystem->SetRelativeMouseMode(true);

  return true;
}

void Game::RunLoop()
{
  while(m_IsRunning)
  {
    ProcessInput();
    UpdateGame();
    GenerateOutput();
  }
}

void Game::ProcessInput()
{
  m_InputSystem->PrepareForUpdate();

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
      case SDL_MOUSEWHEEL:
        m_InputSystem->ProcessEvent(event);
        break;
    }
  }

  m_InputSystem->Update();
  const InputState& keyboardState = m_InputSystem->GetState();

  // todo process any keys here ...
  // if escape pressed, exit Game RunLoop
  if (keyboardState.keyboard.GetKeyState(SDL_SCANCODE_ESCAPE) == E_Released)
  {
    m_IsRunning = false;
  }

  // handle input
  // m_UpdatingActors = true;
  for(auto actor: m_Actors)
  {
    actor->ProcessInput(keyboardState);
  }
  // m_UpdatingActors = false;
}

void Game::UpdateGame()
{
  // frame limiting : wait until 16 ms elapsed
  while(!SDL_TICKS_PASSED(SDL_GetTicks(), m_TicksCount + 16));

  // deltaTime is difference in ticks from last frame
  float deltaTime = (SDL_GetTicks() - m_TicksCount) / 1000.0f;

  // clamp max delta time value (to avoid jumping ahead during debug)
  if (deltaTime > 0.05f)
  {
    deltaTime = 0.05f;
  }
  m_TicksCount = SDL_GetTicks();

  // update objects in game world as function of delta time
  m_UpdatingActors = true;
  for(auto actor: m_Actors)
  {
    actor->Update(deltaTime);
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
    auto it = std::find(m_Actors.begin(), m_Actors.end(), actor);
    if (it != m_Actors.end())
    {
      m_Actors.erase(it);
      // delete actor // TODO
    }
  }
}

void Game::GenerateOutput()
{
  m_Renderer->Draw();
}

void Game::LoadData()
{
  // create FPS actor
  FPSActor* fpsActor = new FPSActor(this);

	// Create scene actors
	Actor* a = new Actor(this);
	a->SetPosition(Vector3(200.0f, 75.0f, 0.0f));
	a->SetScale(100.0f);
	Quaternion q(Vector3::UnitY, -Math::PiOver2);
	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::Pi + Math::Pi / 4.0f));
	a->SetRotation(q);
	MeshComponent* mc = new MeshComponent(a);
	mc->SetMesh(m_Renderer->GetMesh("assets/Cube.gpmesh"));

	a = new Actor(this);
	a->SetPosition(Vector3(200.0f, -75.0f, 0.0f));
	a->SetScale(3.0f);
	mc = new MeshComponent(a);
	mc->SetMesh(m_Renderer->GetMesh("assets/Sphere.gpmesh"));

	// Setup floor
	const float start = -1250.0f;
	const float size = 250.0f;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			a = new PlaneActor(this);
			a->SetPosition(Vector3(start + i * size, start + j * size, -100.0f));
		}
	}

	// Left/right walls
	q = Quaternion(Vector3::UnitX, Math::PiOver2);
	for (int i = 0; i < 10; i++)
	{
		a = new PlaneActor(this);
		a->SetPosition(Vector3(start + i * size, start - size, 0.0f));
		a->SetRotation(q);

		a = new PlaneActor(this);
		a->SetPosition(Vector3(start + i * size, -start + size, 0.0f));
		a->SetRotation(q);
	}

	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::PiOver2));
	// Forward/back walls
	for (int i = 0; i < 10; i++)
	{
		a = new PlaneActor(this);
		a->SetPosition(Vector3(start - size, start + i * size, 0.0f));
		a->SetRotation(q);

		a = new PlaneActor(this);
		a->SetPosition(Vector3(-start + size, start + i * size, 0.0f));
		a->SetRotation(q);
	}

	// Setup lights
  // direction light
	m_Renderer->SetAmbientLight(Vector3(0.2f, 0.2f, 0.2f));
	DirectionalLight& dir = m_Renderer->GetDirectionalLight();
	dir.m_Direction = Vector3(0.0f, -0.707f, -0.707f);
	dir.m_DiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
	dir.m_SpecColor = Vector3(0.8f, 0.8f, 0.8f);

  // point lights
  std::vector<PointLight*> pointLights = m_Renderer->GetPointLights();
  // set up point light 0
  pointLights.at(0)->m_Pos = Vector3(350.0f, -100.0f, 300.0f);
  pointLights.at(0)->m_DiffuseColor = Vector3(0.0f, 0.5f, 1.0f);
  pointLights.at(0)->m_SpecColor = Vector3(1.0f, 0.0f, 0.0f);
  pointLights.at(0)->m_SpecPower = 0.5f;
  pointLights.at(0)->m_RadiusInfluence = 1000;
  // set up point light 1
  pointLights.at(1)->m_Pos = Vector3(-200.0f, -175.0f, 500.0f);
  pointLights.at(1)->m_DiffuseColor = Vector3(1.0f, 0.0f, 0.0f);
  pointLights.at(1)->m_SpecColor = Vector3(0.0f, 1.0f, 0.0f);
  pointLights.at(1)->m_SpecPower = 0.2f;
  pointLights.at(1)->m_RadiusInfluence = 1000;

	// UI elements
	a = new Actor(this);
	a->SetPosition(Vector3(-350.0f, -350.0f, 0.0f));
	SpriteComponent* sc = new SpriteComponent(a);
	sc->SetTexture(m_Renderer->GetTexture("assets/HealthBar.png"));

	a = new Actor(this);
	a->SetPosition(Vector3(375.0f, -275.0f, 0.0f));
	a->SetScale(0.75f);
	sc = new SpriteComponent(a);
	sc->SetTexture(m_Renderer->GetTexture("assets/Radar.png"));

  // crosshair
  a = new Actor(this);
  a->SetPosition(Vector3(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 0.0f));
  sc = new SpriteComponent(a);
  sc->SetTexture(m_Renderer->GetTexture("assets/Crosshair.png"));
}

void Game::UnloadData()
{
  while(!m_Actors.empty())
  {
    delete m_Actors.back();
  }

  if(m_Renderer)
  {
    m_Renderer->UnloadData();
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

Renderer* Game::GetRenderer()
{
  return m_Renderer;
}

void Game::ShutDown()
{
  UnloadData();
  if(m_Renderer)
  {
    m_Renderer->ShutDown();
  }

  // shut down input system
  m_InputSystem->ShutDown();
  delete m_InputSystem;

  SDL_Quit();
}
