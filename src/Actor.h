#pragma once

#include "Constants.h"
#include "Math.h"
#include "InputSystem.h"
#include <vector>
#include <cstdint>

class Game;

class Actor {
public:
  enum State {E_Active, E_Paused, E_Dead};
private:
  State m_State;

  // transform
  Matrix4 m_WorldTransform;
  Vector3 m_Position; // center of actor
  float m_Scale; // 1.0f is 100%
  Quaternion m_Rotation;
  bool m_RecomputeWorldTransform;

  std::vector<class Component*> m_Components; // sorted by update order
  class Game* m_Game;

public:
  Actor(class Game* game);
  virtual ~Actor();
  void Update(float deltaTime);
  void UpdateComponents(float deltaTime); // updates all components
  virtual void UpdateActor(float deltaTime);

  void ProcessInput(const InputState& state); // called in Game not overridable
  void ProcessMouse(const uint32_t mouseState, const int x, const int y); // called in Game not overridable
  virtual void ActorInput(const InputState& state);

  // getters / setters
  const Vector3& GetPosition() const;
  void SetPosition(const Vector3& pos);
  float GetScale() const;
  void SetScale(float scale);
  const Quaternion& GetRotation() const;
  void SetRotation(const Quaternion& rotation);
  State GetState() const;
  void SetState(State state);
  class Game* GetGame();
  Vector3 GetForward() const;

  void ComputeWorldTransform();
  Matrix4 GetWorldTransform() const;

  void AddComponent(class Component* component);
  void RemoveComponent(class Component* component);

  void ClampToScreen(float& pos, int objSize, float lowerLimit, float upperLimit);
};
