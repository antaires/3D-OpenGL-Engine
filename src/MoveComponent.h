#pragma once
#include "Component.h"

class MoveComponent: public Component
{
private:
  float m_AngularSpeed; // control rotation (radians/second)
  float m_ForwardSpeed;

public:

  MoveComponent(class Actor* owner, int updateOrder = 10); // update early
  void Update(float deltaTime) override;

  float GetAngularSpeed() const;
  float GetForwardSpeed() const;
  void SetAngularSpeed(float angularSpeed);
  void SetForwardSpeed(float speed);
};
