#include "InputComponent.h"
#include "Bullet.h"

#include <SDL2/SDL.h>

InputComponent::InputComponent(class Actor* owner)
  :MoveComponent(owner)
  , m_MaxForwardSpeed(FOX_SPEED)
  , m_MaxAngularSpeed(FOX_ANGULAR_SPEED)
  , m_UpKey(SDL_SCANCODE_W) //default to WASD keys
  , m_DownKey(SDL_SCANCODE_S)
  , m_LeftKey(SDL_SCANCODE_A)
  , m_RightKey(SDL_SCANCODE_D)
  , m_ClockwiseKey(SDL_SCANCODE_K) // not currently used
  , m_CounterClockwiseKey(SDL_SCANCODE_L)
  , m_ShotTimer(0)
{}

void InputComponent::ProcessInput(const uint8_t* keyState)
{
  // calc velocity & set
  Vector2 velocity(0.0f, 0.0f);
  if (keyState[m_UpKey])
  {
    velocity.y += 1;
  }
  if (keyState[m_DownKey])
  {
    velocity.y -= 1;
  }
  if (keyState[m_LeftKey])
  {
    velocity.x -= 1;
  }
  if (keyState[m_RightKey])
  {
    velocity.x += 1;
  }
  SetVelocity(velocity);

  /* NOT USED (adapt for enemy, to turn towards player)
  // calc angular speed & set
  float angularSpeed = 0.0f;
  if(keyState[m_ClockwiseKey])
  {
    angularSpeed += m_MaxAngularSpeed;
  }
  if(keyState[m_CounterClockwiseKey])
  {
    angularSpeed -= m_MaxAngularSpeed;
  }
  SetAngularSpeed(angularSpeed);
  */
}

void InputComponent::DecrementShotTimer(float deltaTime)
{
  m_ShotTimer -= deltaTime;
}

void InputComponent::ProcessMouse(const uint32_t mouseState, const int x, const int y)
{
  // TODO simplify
  Vector2 mousePos(x, y);
  Vector2 pos = m_Owner->GetPosition();

  // translate SDL coords to center screen origin OpenGL coords
  // can add zoom factor
  mousePos.x = mousePos.x - SCREEN_WIDTH / 2.0f;
  mousePos.y = -1 * (mousePos.y - SCREEN_HEIGHT / 2.0f);

  Vector2 forwardVector = mousePos - pos;
  SetForwardVector(forwardVector);

  // handle mouse clicks here using mouseState
  SDL_PumpEvents();
  if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
  {
    if (m_ShotTimer <= 0)
    {
      forwardVector.Normalize();
      forwardVector.x *= BULLET_SPEED;
      forwardVector.y *= BULLET_SPEED;
      Bullet* bullet = new Bullet(m_Owner->GetGame(), forwardVector);
      bullet->SetPosition(m_Owner->GetPosition());
      m_ShotTimer = PLAYER_SHOOT_TIMER;
    }
  }
}
