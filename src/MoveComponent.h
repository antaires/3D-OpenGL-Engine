#pragma once

#include "Component.h"

class MoveComponent: public Component
{
private:
  float m_AngularSpeed; // control rotation (radians/second)
  float m_ForwardSpeed;
  Vector2 m_Velocity; // controls movement (units/ second)
  Vector2 m_ForwardVector; // controls which direction actor is facing
  bool m_ClampToScreen;

public:
  MoveComponent(class Actor* owner, int updateOrder = 10); // update early
  void Update(float deltaTime) override;

  float GetAngularSpeed() const;
  void SetVelocity(Vector2 velocity);
  Vector2 GetVelocity() const;
  Vector2 GetForwardVector() const;
  void SetForwardVector(Vector2 forwardVector);
  void SetAngularSpeed(float angularSpeed);
  void SetForwardSpeed(float speed);
  void SetClampToScreen(bool clampToScreen);
};
