#pragma once

#include "Math.h"

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <unordered_map>

class Game {
private:
  std::vector<class Actor*> m_Actors;
  std::vector<class Actor*> m_PendingActors;
  std::vector<class PlaneActor*> m_PlaneActors;

  class Renderer* m_Renderer;
  Uint32 m_TicksCount;

  bool m_IsRunning;
  bool m_UpdatingActors;

  class InputSystem* m_InputSystem;
  class PhysWorld* m_PhysWorld;

public:
  Game();
  bool Initialize();
  void RunLoop();
  void ShutDown();

  void AddActor(class Actor* actor);
  void RemoveActor(class Actor* actor);
  void AddPlane(class PlaneActor* planeActor);
  void RemovePlane(class PlaneActor* planeActor);

  class Renderer* GetRenderer();
  class PhysWorld* GetPhysWorld();
  std::vector<class PlaneActor*>& GetPlaneActors();
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
