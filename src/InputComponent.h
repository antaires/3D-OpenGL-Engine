#pragma once

#include "MoveComponent.h"

// sets up specific keys to control forward / angular speed
// of MoveComponent
class InputComponent: public MoveComponent
{
private:
  float m_MaxForwardSpeed;
  float m_MaxAngularSpeed;
  int m_UpKey;
  int m_DownKey;
  int m_LeftKey;
  int m_RightKey;
  int m_ClockwiseKey; // currently not used, using mouse position
  int m_CounterClockwiseKey;

  float m_ShotTimer;
public:
  InputComponent(class Actor* owner);
  void ProcessInput(const uint8_t* keyState) override;
  void ProcessMouse(const uint32_t mouseState, const int x, const int y) override;

  void DecrementShotTimer(float deltaTime);
};
