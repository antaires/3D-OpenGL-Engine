#include "FPSActor.h"
#include "MoveComponent.h"
#include "SDL2/SDL_scancode.h"
#include "Renderer.h"
#include "Game.h"
#include "FPSCamera.h"
#include "MeshComponent.h"
#include "BoxComponent.h"

FPSActor::FPSActor(Game* game)
	:Actor(game)
{
	m_MoveComp = new MoveComponent(this);
  m_Camera = new FPSCamera(this);

  // set up mesh
  m_Model = new Actor(game);
  m_Model->SetScale(0.75f);
  m_MeshComp = new MeshComponent(m_Model);
  m_MeshComp->SetMesh(game->GetRenderer()->GetMesh("assets/Rifle.gpmesh"));
}

void FPSActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

  // update position of FPS model relative to actor position
  const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
  Vector3 modelPos = GetPosition();
  modelPos += GetForward() * modelOffset.x;
  modelPos += GetRight() * modelOffset.y;
  modelPos.z += modelOffset.z;
  m_Model->SetPosition(modelPos);

  // init rotation of actor rotation
  Quaternion quat = GetRotation();
  m_Model->SetRotation(quat);
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
    strafeSpeed -= 300.0f;
	}
	buttonState = state.keyboard.GetKeyState(SDL_SCANCODE_D);
	if (buttonState == E_Pressed || buttonState == E_Held)
	{
    strafeSpeed += 300.0f;
	}
  m_MoveComp->SetForwardSpeed(forwardSpeed);
  m_MoveComp->SetStrafeSpeed(strafeSpeed);

  // use mouse movement to get rotation using relative mouse movement
  // assume movement usually between -500 & 500
  // update angular speed by relative x motion of mouse
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

  // update pitch speed by relative y motion of mouse
  const float maxPitchSpeed = Math::Pi * 8;
  float pitchSpeed = 0.0f;
  if (state.mouseState.m_MousePosition.y != 0)
  {
    // convert to approximately [-1.0, 1.0]
    pitchSpeed = state.mouseState.m_MousePosition.y / maxMouseSpeed;
    // multiply by rotation / sec
    pitchSpeed *= maxPitchSpeed;
  }
  m_Camera->SetPitchSpeed(pitchSpeed);
}
