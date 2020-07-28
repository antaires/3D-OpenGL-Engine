#include "FPSActor.h"
#include "MoveComponent.h"
#include "SDL2/SDL_scancode.h"
#include "Renderer.h"
#include "Game.h"
#include "FPSCamera.h"
#include "MeshComponent.h"
#include "BoxComponent.h"
#include "PlaneActor.h"

#include <iostream> // todo remove

FPSActor::FPSActor(Game* game)
	:Actor(game)
{
	m_MoveComp = new MoveComponent(this);
  m_Camera = new FPSCamera(this);

  // set up rifle mesh
  m_Model = new Actor(game);
  m_Model->SetScale(0.75f);
  m_MeshComp = new MeshComponent(m_Model);
	Mesh* mesh = game->GetRenderer()->GetMesh("assets/Rifle.gpmesh");
  m_MeshComp->SetMesh(mesh);

	// set up box component
	m_BoxComp = new BoxComponent(this);
	AABB myBox(
		Vector3(-25.0f, -25.0f, -87.5f)
		, Vector3(25.0f, 25.0f, 87.5f)
	);
	m_BoxComp->SetObjectBox(myBox);
	m_BoxComp->SetShouldRotate(false);

	// todo remove
	SetPosition(Vector3(0.0f, 0.0f, 200.0f));
}

void FPSActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();

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

	// check for jumping
	buttonState = state.keyboard.GetKeyState(SDL_SCANCODE_SPACE);
	if (buttonState == E_Pressed && m_MoveComp->GetIsGrounded())
	{
		m_MoveComp->SetJumpSpeed(1200.0f);
	}

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

void FPSActor::FixCollisions()
{
	// recompute my world transform to update world box
	ComputeWorldTransform();

	const AABB& playerBox = m_BoxComp->GetWorldBox();
	Vector3 pos = GetPosition();

	m_MoveComp->SetIsGrounded(false); // gets set true if collision with top of AABB

	auto& planeActors = GetGame()->GetPlaneActors();
	for(auto plane : planeActors)
	{
		// do we collide with this plane actor ?
		const AABB& planeBox = plane->GetBox()->GetWorldBox();
		if(Intersect(playerBox, planeBox))
		{
			// Calculate all our differences
			float dx1 = planeBox.m_Max.x - playerBox.m_Min.x;
			float dx2 = planeBox.m_Min.x - playerBox.m_Max.x;
			float dy1 = planeBox.m_Max.y - playerBox.m_Min.y;
			float dy2 = planeBox.m_Min.y - playerBox.m_Max.y;
			float dz1 = planeBox.m_Max.z - playerBox.m_Min.z;
			float dz2 = planeBox.m_Min.z - playerBox.m_Max.z;

			// Set dx to whichever of dx1/dx2 have a lower abs (same for dy, dz)
			float dx = Math::Abs(dx1) < Math::Abs(dx2) ? dx1 : dx2;
			float dy = Math::Abs(dy1) < Math::Abs(dy2) ? dy1 : dy2;
			float dz = Math::Abs(dz1) < Math::Abs(dz2) ? dz1 : dz2;

			// Whichever is closest, adjust x/y position
			if (Math::Abs(dx) <= Math::Abs(dy) && Math::Abs(dx) <= Math::Abs(dz))
			{
				pos.x += dx;
			}
			else if (Math::Abs(dy) <= Math::Abs(dx) && Math::Abs(dy) <= Math::Abs(dz))
			{
				pos.y += dy;
			}
			else
			{
				pos.z += dz;
			}

			// check if grounded (dz2 is top of plane)
			if (dz2 < 0.0f)
			{
				m_MoveComp->SetIsGrounded(true);
			}

			// set position and update box component
			SetPosition(pos);
			m_BoxComp->OnUpdateWorldTransform();
		}
	}
}
