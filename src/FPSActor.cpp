#include "FPSActor.h"
#include "MoveComponent.h"
#include "SDL2/SDL_scancode.h"
#include "Renderer.h"
#include "Game.h"

FPSActor::FPSActor(Game* game)
	:Actor(game)
{
	m_MoveComp = new MoveComponent(this);
}

void FPSActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	// Compute new camera from this actor
	Vector3 cameraPos = GetPosition();
	Vector3 target = GetPosition() + GetForward() * 100.0f;
	Vector3 up = Vector3::UnitZ;

	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	GetGame()->GetRenderer()->SetViewMatrix(view);
}

void FPSActor::ActorInput(const InputState& state)
{
	float forwardSpeed = 0.0f;
  float strafeSpeed = 0.0f;

	// wasd movement
	ButtonState buttonState = state.keyboard.GetKeyState(SDL_SCANCODE_W);
	if (buttonState == E_Pressed || buttonState == E_Held)
	{
		forwardSpeed += 300.0f;
	}
	buttonState = state.keyboard.GetKeyState(SDL_SCANCODE_S);
	if (buttonState == E_Pressed || buttonState == E_Held)
	{
		forwardSpeed -= 300.0f;
	}
	buttonState = state.keyboard.GetKeyState(SDL_SCANCODE_A);
	if (buttonState == E_Pressed || buttonState == E_Held)
	{
		// angularSpeed -= Math::TwoPi;
    strafeSpeed -= 300.0f;
	}
	buttonState = state.keyboard.GetKeyState(SDL_SCANCODE_D);
	if (buttonState == E_Pressed || buttonState == E_Held)
	{
		// angularSpeed += Math::TwoPi;
    strafeSpeed += 300.0f;
	}
  m_MoveComp->SetForwardSpeed(forwardSpeed);
  m_MoveComp->SetStrafeSpeed(strafeSpeed);

  // use mouse movement to get rotation using relative mouse movement
  // assume movement usually between -500 & 500
  const int maxMouseSpeed = 500;
  // rotation / sec at max speed
  const float maxAngularSpeed = Math::Pi * 8;
  float angularSpeed = 0.0f;
  if (state.mouseState.m_MousePosition.x != 0)
  {
    // convert to approximately [-1.0, 1.0]
    angularSpeed = state.mouseState.m_MousePosition.x / maxMouseSpeed;
    // multiply by rotation / sec
    angularSpeed *= maxAngularSpeed;
  }
  m_MoveComp->SetAngularSpeed(angularSpeed);
}
