#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <SDL2/SDL.h>
#include "Math.h"

struct DirectionalLight
{
Vector3 m_Direction; // direction of light
Vector3 m_DiffuseColor;
Vector3 m_SpecColor;
};

struct PointLight
{
  Vector3 m_Pos;
  Vector3 m_DiffuseColor;
  Vector3 m_SpecColor;
  float m_SpecPower;
  float m_RadiusInfluence;
};

class Renderer
{
public:
  Renderer(class Game* game);
  ~Renderer();

  bool Initialize(float screenWidth, float screenHeight);
  void ShutDown();
  void UnloadData();

  void Draw();

  void AddSprite(class SpriteComponent* sprite);
  void RemoveSprite(class SpriteComponent* sprite);

  void AddMeshComp(class MeshComponent* mesh);
  void RemoveMeshComp(class MeshComponent* mesh);

  class Texture* GetTexture(const std::string& fileName);
  class Mesh* GetMesh(const std::string& fileName);
  void SetViewMatrix(const Matrix4& view);
  void SetAmbientLight(const Vector3& ambient);
  DirectionalLight& GetDirectionalLight();
  std::vector<PointLight*> GetPointLights();

  float GetScreenWidth() const;
  float GetScreenHeight() const;
private:
  bool LoadShaders();
  void CreateSpriteVerts();
  void SetLightUniforms(class Shader* shader);

  // Map of textures loaded
  std::unordered_map<std::string, class Texture*> m_Textures;
  // Map of meshes loaded
  std::unordered_map<std::string, class Mesh*> m_Meshes;

  // All the sprite components drawn
  std::vector<class SpriteComponent*> m_Sprites;

  // All mesh components drawn
  std::vector<class MeshComponent*> m_MeshComps;

  // Game
  class Game* m_Game;

  // Sprite shader
  class Shader* m_SpriteShader;
  // Sprite vertex array
  class VertexArray* m_SpriteVerts;

  // Mesh shaders
  std::vector<class Shader*> m_MeshShaders;

  // View/projection for 3D shaders
  Matrix4 m_View;
  Matrix4 m_Projection;
  // Width/height of screen
  float m_ScreenWidth;
  float m_ScreenHeight;

  // Lighting data
  Vector3 m_AmbientLight;
  DirectionalLight m_DirLight;
  std::vector<PointLight*> m_PointLights;

  // Window
  SDL_Window* m_Window;
  // OpenGL context
  SDL_GLContext m_Context;
};
