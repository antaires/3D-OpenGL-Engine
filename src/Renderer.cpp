#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"

#include <algorithm>
#include <GL/glew.h>

Renderer::Renderer(Game* game)
  :m_Game(game)
  , m_SpriteShader(nullptr)
{}

Renderer::~Renderer()
{}

bool Renderer::Initialize(float width, float height)
{
  m_ScreenWidth = width;
  m_ScreenHeight = height;

  /*
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    SDL_Log("Unable to Initialize SDL: %s", SDL_GetError());
    return false;
  }*/

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
  // set up depth buffer (ie Z-buffer)
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  // enable double buffering
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  // force OpenGL to sue hardware acceleration
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  m_Window = SDL_CreateWindow(
    ""      // window title
    , 100   // top left  x-coord
    , 100   // top left  y-coord
    , static_cast<int>(m_ScreenWidth)
    , static_cast<int>(m_ScreenHeight)
    , SDL_WINDOW_OPENGL     // use openGL
  );
  if (!m_Window)
  {
    SDL_Log("Failed to create window: %s", SDL_GetError());
    return false;
  }

  // create open GL context and saves it to member variable
  m_Context = SDL_GL_CreateContext(m_Window);

  // TODO
  // required for mac! without this, a rectangle is drawn as a rhombus
  int screenWidth, screenHeight;
  SDL_GL_GetDrawableSize(m_Window, &screenWidth, &screenHeight );
  glViewport(0, 0, screenWidth, screenHeight); // adjust to high density screen
  m_ScreenWidth = (float) screenWidth;
  m_ScreenHeight = (float) screenHeight;

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

  return true;
}

void Renderer::ShutDown()
{
  delete m_SpriteVerts;
  m_SpriteShader->Unload();
  delete m_SpriteShader;
  for(auto ms: m_MeshShaders)
  {
    ms->Unload();
    // delete ms; TODO delete safely
  }
  SDL_GL_DeleteContext(m_Context);
  SDL_DestroyWindow(m_Window);
}

void Renderer::UnloadData()
{
  // destroy textures
  for(auto t : m_Textures)
  {
    t.second->Unload();
    delete t.second;
  }
  m_Textures.clear();

  // destroy meshes
  for(auto i : m_Meshes)
  {
    i.second->Unload();
    delete i.second;
  }
  m_Meshes.clear();
}

void Renderer::Draw()
{
  // Set the clear color to light grey
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw mesh components
  // Enable depth buffering/disable alpha blend
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  // draw all shaders, grouped by which shader they use
  for(Shader* shader : m_MeshShaders)
  {
    // Set the mesh shader active
    shader->SetActive();

    // Update view-projection matrix
    shader->SetMatrixUniform("uViewProj", m_View * m_Projection);
    SetLightUniforms(shader);
    for (auto mc : m_MeshComps)
    {
      // only draw if shader matches mesh components shader
      if (shader->GetShaderName() == mc->GetShaderName())
      {
        mc->Draw(shader);
      }
    }
  }

  // Draw all sprite components
  // Disable depth buffering
  glDisable(GL_DEPTH_TEST);
  // Enable alpha blending on the color buffer
  glEnable(GL_BLEND);
  glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

  // Set shader/vao as active
  m_SpriteShader->SetActive();
  m_SpriteVerts->SetActive();
  for (auto sprite : m_Sprites)
  {
    sprite->Draw(m_SpriteShader);
  }

  // Swap the buffers
  SDL_GL_SwapWindow(m_Window);
}

void Renderer::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = m_Sprites.begin();
	for (; iter != m_Sprites.end(); ++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	m_Sprites.insert(iter, sprite);
}

void Renderer::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(m_Sprites.begin(), m_Sprites.end(), sprite);
	m_Sprites.erase(iter);
}

void Renderer::AddMeshComp(MeshComponent* mesh)
{
	m_MeshComps.emplace_back(mesh);
}

void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
	auto iter = std::find(m_MeshComps.begin(), m_MeshComps.end(), mesh);
	m_MeshComps.erase(iter);
}

Texture* Renderer::GetTexture(const std::string& fileName)
{
	Texture* tex = nullptr;
	auto iter = m_Textures.find(fileName);
	if (iter != m_Textures.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = new Texture();
		if (tex->Load(fileName))
		{
			m_Textures.emplace(fileName, tex);
		}
		else
		{
			delete tex;
			tex = nullptr;
		}
	}
	return tex;
}

Mesh* Renderer::GetMesh(const std::string & fileName)
{
	Mesh* m = nullptr;
	auto iter = m_Meshes.find(fileName);
	if (iter != m_Meshes.end())
	{
		m = iter->second;
	}
	else
	{
		m = new Mesh();
		if (m->Load(fileName, this))
		{
			m_Meshes.emplace(fileName, m);
		}
		else
		{
			delete m;
			m = nullptr;
		}
	}
	return m;
}

bool Renderer::LoadShaders()
{
  // create 2d sprite shaders
  std::string spriteShader = "Sprite";
  m_SpriteShader = new Shader(spriteShader);
  if(!m_SpriteShader->Load("shaders/Sprite.vert", "shaders/Sprite.frag"))
  {
    return false;
  }
  m_SpriteShader->SetActive();

  Matrix4 viewProj = Matrix4::CreateSimpleViewProj(m_ScreenWidth, m_ScreenHeight);
  m_SpriteShader->SetMatrixUniform("uViewProj", viewProj);

  // Create all 3d shaders:
  // basic
  std::string basicShader = "BasicMesh";
  Shader* basicMesh = new Shader(basicShader);
  if (!basicMesh->Load("shaders/BasicMesh.vert", "shaders/BasicMesh.frag"))
  {
    return false;
  }
  m_MeshShaders.push_back(basicMesh);
  // phong
  std::string phongShaderName = "Phong";
	Shader* phongShader = new Shader(phongShaderName);
	if (!phongShader->Load("shaders/Phong.vert", "shaders/Phong.frag"))
	{
		return false;
	}
  m_MeshShaders.push_back(phongShader);
  phongShader->SetActive();

	// Set the view-projection matrix
	m_View = Matrix4::CreateLookAt(
    Vector3::Zero     // camera position
    , Vector3::UnitX  // target position
    , Vector3::UnitZ  // up
  );
	m_Projection = Matrix4::CreatePerspectiveFOV(
    Math::ToRadians(70.0f)  // horizontal FOV
    , m_ScreenWidth         // width of view
    , m_ScreenHeight        // height of view
    , 25.0f                 // near plane
    , 10000.0f              // far plane
  );
  for(auto ms : m_MeshShaders)
  {
    ms->SetMatrixUniform("uViewProj", m_View * m_Projection);
  }

  return true;
}

void Renderer::CreateSpriteVerts()
{
  // V texture coord is flipped to account for how openGL expects image data upsidedown
  // pos = 3 #s, normals = 3#s, tex UV coords = 2 #s
  float vertexBuffer[] = {
    -0.5f,  0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
		 0.5f,  0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
		 0.5f, -0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
		-0.5f, -0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f // bottom left
  };

	unsigned int indexBuffer[] = {
		0, 1, 2,
		2, 3, 0
	};

  m_SpriteVerts = new VertexArray(vertexBuffer, 4, indexBuffer, 6);
}

void Renderer::SetLightUniforms(Shader* shader)
{
	// Camera position is from inverted view
	Matrix4 invView = m_View;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
	// Ambient light
	shader->SetVectorUniform("uAmbientLight", m_AmbientLight);
	// Directional light
	shader->SetVectorUniform("uDirLight.mDirection", m_DirLight.m_Direction);
	shader->SetVectorUniform("uDirLight.mDiffuseColor", m_DirLight.m_DiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecColor", m_DirLight.m_SpecColor);

  // TODO make array
  shader->SetVectorUniform("uPointLight.mPos", m_PointLight.m_Pos);
  shader->SetVectorUniform("uPointLight.mDiffuseColor", m_PointLight.m_DiffuseColor);
  shader->SetVectorUniform("uPointLight.mSpecColor", m_PointLight.m_SpecColor);
  shader->SetFloatUniform("uPointLight.mSpecPower", m_PointLight.m_SpecPower);
  shader->SetFloatUniform("uPointLight.mRadiusInfluence", m_PointLight.m_RadiusInfluence);
}

void Renderer::SetViewMatrix(const Matrix4& view) { m_View = view; }

void Renderer::SetAmbientLight(const Vector3& ambient) { m_AmbientLight = ambient; }

DirectionalLight& Renderer::GetDirectionalLight() { return m_DirLight; }

// todo make array
PointLight& Renderer::GetPointLight() { return m_PointLight; }

float Renderer::GetScreenWidth() const { return m_ScreenWidth; }

float Renderer::GetScreenHeight() const { return m_ScreenHeight; }
