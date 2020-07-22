#pragma once

#include "Math.h"

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <unordered_map>

class Game {
private:
  SDL_Window* m_Window;
  SDL_GLContext m_Context;

  float m_DeltaTime;

  bool m_IsRunning;
  bool m_UpdatingActors;
  Uint32 m_TicksCount;
  class Map* m_Map;

  std::vector<class Actor*> m_Actors;
  std::vector<class Actor*> m_PendingActors;
  std::vector<class SpriteComponent*> m_Sprites;
  std::unordered_map<std::string, class Texture*> m_Textures;

  // OpenGL
  class VertexArray* m_SpriteVerts;
  class Shader* m_SpriteShader;

public:
  Game();
  bool Initialize();
  void RunLoop();
  void ShutDown();

  void AddActor(class Actor* actor);
  void RemoveActor(class Actor* actor);
  void AddSprite(class SpriteComponent* sprite);
  void RemoveSprite(class SpriteComponent* sprite);
  class Texture* GetTexture(const std::string& fileName);

private:
  void ProcessInput();
  void UpdateGame();
  void GenerateOutput();

  void LoadData();
  void UnloadData();

  // OpenGL
  void CreateSpriteVerts();
  bool LoadShaders();
};
