#pragma once

#include "Constants.h"
#include "Math.h"
#include <vector>

class Game;

class Actor {
public:
  enum State {E_Active, E_Paused, E_Dead, E_Dying};
private:
  State m_State;

  // transform
  Vector2 m_Position; // center of actor
  float m_Scale; // 1.0f is 100%
  float m_Rotation; // rotation angle in RADIANS

  Matrix4 m_WorldTransform;
  bool m_RecomputeWorldTransform;

  Vector2 m_PrevPosition;
  Vector2 m_TopCornerPosition; // used for rectangle collision check

  Vector2 m_Velocity;
  Vector2 m_ForwardVector;

  float m_Width;
  float m_Height;

  class Game* m_Game;

  std::vector<class Component*> m_Components; // sorted by update order

protected:
    bool m_DyingStarted;
    bool m_HasCollision;
    Vector2 m_GoalPosition;
public:
  Actor(class Game* game);
  virtual ~Actor();
  void Update(float deltaTime);
  void UpdateComponents(float deltaTime); // updates all components
  virtual void UpdateActor(float deltaTime);

  void ProcessInput(const uint8_t* keyState); // called in Game not overridable
  void ProcessMouse(const uint32_t mouseState, const int x, const int y); // called in Game not overridable
  virtual void ActorInput(const uint8_t* keyState);

  virtual void HandleDeath();

  Vector2 GetForward() const;

  void ComputeWorldTransform();

  State GetState() const;
  void SetState(State state);
  Vector2 GetPosition() const;
  void SetPosition(const Vector2& pos);
  Vector2 GetPrevPosition() const;
  float GetRotation() const;
  void SetRotation(float rotation);
  float GetScale() const;
  void SetScale(float scale);
  Vector2 GetVelocity() const;
  void SetVelocity(Vector2 velocity);
  Vector2 GetForwardVector() const;
  void SetForwardVector(Vector2 forwardVector);
  float GetHeight() const;
  float GetWidth() const;
  void SetHeight(float height);
  void SetWidth(float width);
  void SetHasCollision(bool hasCollision);
  Vector2 GetGoal() const;
  void SetGoal(Vector2 goal);
  const Vector2& GetTopCornerPosition() const;
  class Game* GetGame();
  Matrix4 GetWorldTransform() const;

  void AddComponent(class Component* component);
  void RemoveComponent(class Component* component);

  void ClampToScreen(float& pos, int objSize, float lowerLimit, float upperLimit);
  bool CollidesWithBarrier(Vector2 position, float width, float height) const;
};
