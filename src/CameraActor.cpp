#include "CameraActor.h"
#include "MoveComponent.h"
#include "SDL2/SDL_scancode.h"
#include "Renderer.h"
#include "Game.h"

CameraActor::CameraActor(Game* game)
	:Actor(game)
{
	m_MoveComp = new MoveComponent(this);
}

void CameraActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	// Compute new camera from this actor
	Vector3 cameraPos = GetPosition();
	Vector3 target = GetPosition() + GetForward() * 100.0f;
	Vector3 up = Vector3::UnitZ;

	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	GetGame()->GetRenderer()->SetViewMatrix(view);
}

void CameraActor::ActorInput(const InputState& state)
{
	float forwardSpeed = 0.0f;
	float angularSpeed = 0.0f;
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
		angularSpeed -= Math::TwoPi;
	}
	buttonState = state.keyboard.GetKeyState(SDL_SCANCODE_D);
	if (buttonState == E_Pressed || buttonState == E_Held)
	{
		angularSpeed += Math::TwoPi;
	}

	m_MoveComp->SetForwardSpeed(forwardSpeed);
	m_MoveComp->SetAngularSpeed(angularSpeed);
}
