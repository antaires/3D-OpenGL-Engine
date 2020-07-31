#include "FollowActor.h"
#include "SkeletalMeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "FollowCamera.h"
#include "MoveComponent.h"
#include "Constants.h"
#include "Mesh.h"
#include "BoxComponent.h"
#include "collision.h"
#include "PlaneActor.h"
#include "BallActor.h"
#include "SDL2/SDL_scancode.h"

FollowActor::FollowActor(Game* game)
	:Actor(game)
	,m_Moving(false)
  , m_ShootTimer(PLAYER_SHOOT_TIMER)

{
	SetPosition(Vector3(0.0f, 0.0f, 500.0f));

	m_MeshComp = new SkeletalMeshComponent(this);
  Mesh* mesh = game->GetRenderer()->GetMesh("assets/CatWarrior.gpmesh");
	m_MeshComp->SetMesh(mesh);
  std::string skel = "assets/CatWarrior.gpskel";
	m_MeshComp->SetSkeleton(game->GetSkeleton(skel));
  std::string anim = "assets/CatActionIdle.gpanim";
	m_MeshComp->PlayAnimation(game->GetAnimation(anim), 1.25f);

	m_MoveComp = new MoveComponent(this);
	m_CameraComp = new FollowCamera(this);
	m_CameraComp->SnapToIdeal();

  // set up box component
	m_BoxComp = new BoxComponent(this);
	m_BoxComp->SetObjectBox(mesh->GetBox());
	m_BoxComp->SetShouldRotate(false);
}

void FollowActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();

	// decrease shoot timer
	m_ShootTimer -= deltaTime;
}


void FollowActor::ActorInput(const InputState& state)
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

  // TODO : rotate player to point at mouse
  // mousePos - playerPos and use angle between this vector and player rotation


	// check for jumping
	buttonState = state.keyboard.GetKeyState(SDL_SCANCODE_SPACE);
	if (buttonState == E_Pressed && m_MoveComp->GetIsGrounded())
	{
		m_MoveComp->SetJumpSpeed(1200.0f);
	}

	// check for shooting
	if (state.mouseState.GetButtonValue(SDL_BUTTON_LEFT) && m_ShootTimer < 0.0f)
	{
		// Shoot(); TODO
    // create shoot func to fire from player position towards mouse position
		m_ShootTimer = PLAYER_SHOOT_TIMER;
	}

  // Did we just start moving?
  if (!m_Moving && !Math::NearZero(forwardSpeed))
  {
    m_Moving = true;
    std::string sprint = "assets/CatRunSprint.gpanim";
    m_MeshComp->PlayAnimation(GetGame()->GetAnimation(sprint), 1.25f);
  }
  // Or did we just stop moving?
  else if (m_Moving && Math::NearZero(forwardSpeed))
  {
    m_Moving = false;
    std::string idle = "assets/CatActionIdle.gpanim";
    m_MeshComp->PlayAnimation(GetGame()->GetAnimation(idle), 1.25f);
  }
  m_MoveComp->SetForwardSpeed(forwardSpeed);

  // m_MoveComp->SetAngularSpeed(angularSpeed);
}

void FollowActor::FixCollisions()
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

void FollowActor::SetVisible(bool visible)
{
	m_MeshComp->SetVisible(visible);
}
