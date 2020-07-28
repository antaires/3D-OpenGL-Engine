#pragma once
#include "Component.h"

class MoveComponent: public Component
{
private:
  float m_AngularSpeed; // control rotation (radians/second)
  float m_ForwardSpeed;
  float m_StrafeSpeed; // speed character moves left / right
  float m_JumpSpeed; // vertical jump speed
  bool  m_IsGrounded;
public:

  MoveComponent(class Actor* owner, int updateOrder = 10); // update early
  void Update(float deltaTime) override;

  float GetAngularSpeed() const;
  float GetForwardSpeed() const;
  float GetStrafeSpeed() const;
  float GetJumpSpeed() const;
  void SetAngularSpeed(float angularSpeed);
  void SetForwardSpeed(float speed);
  void SetStrafeSpeed(float speed);
  void SetJumpSpeed(float speed);

  void SetIsGrounded(bool value);
  bool GetIsGrounded() const;
};
