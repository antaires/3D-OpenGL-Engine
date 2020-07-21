#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <unordered_map>

#include "Actor.h"
#include "Player.h"
#include "Enemy.h"
#include "SpriteComponent.h"


class Game {
private:
  SDL_Window* m_Window;
  SDL_GLContext m_Context;

  float m_DeltaTime;

  bool m_IsRunning;
  bool m_UpdatingActors;
  Uint32 m_TicksCount;
  class Map* m_Map;

  // Game specific
  Player* m_Player;
  std::vector<Enemy*> m_Enemies;
  float m_EnemySpawnTimer;

  std::vector<class Actor*> m_Actors;
  std::vector<class Actor*> m_PendingActors;
  std::vector<class SpriteComponent*> m_Sprites;
  std::unordered_map<std::string, class Texture*> m_Textures;

  // OpenGL
  class VertexArray* m_SpriteVerts;
  class Shader* m_SpriteShader;

  void ProcessInput();
  void UpdateGame();
  void HandleCollisions();
  void GenerateOutput();

  void LoadData();
  void UnloadData();

  // OpenGL
  void CreateSpriteVerts();
  bool LoadShaders();

public:
  Game();
  bool Initialize();
  void RunLoop();
  void ShutDown();

  std::vector<Enemy*> GetEnemies(); // TODO is this const?

  void AddActor(class Actor* actor);
  void RemoveActor(class Actor* actor);
  void AddSprite(class SpriteComponent* sprite);
  void RemoveSprite(class SpriteComponent* sprite);
  void SetMap(class Map* map);
  Vector2 GetPlayerPosition() const;
  class Player* GetPlayer();

  class Texture* GetTexture(const std::string& fileName);

  bool CollidesWithBarrier(Vector2 pos, float width, float height);
};
