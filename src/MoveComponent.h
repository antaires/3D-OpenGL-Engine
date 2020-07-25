#pragma once
#include "Component.h"

class MoveComponent: public Component
{
private:
  float m_AngularSpeed; // control rotation (radians/second)
  float m_ForwardSpeed;
  float m_StrafeSpeed; // speed character moves left / right
public:

  MoveComponent(class Actor* owner, int updateOrder = 10); // update early
  void Update(float deltaTime) override;

  float GetAngularSpeed() const;
  float GetForwardSpeed() const;
  float GetStrafeSpeed() const;
  void SetAngularSpeed(float angularSpeed);
  void SetForwardSpeed(float speed);
  void SetStrafeSpeed(float speed);
};
