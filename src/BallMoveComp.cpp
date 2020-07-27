#include "BallMoveComp.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"
#include "TargetActor.h"
#include "BallActor.h"

BallMoveComp::BallMoveComp(Actor* owner)
	:MoveComponent(owner)
{
}

void BallMoveComp::Update(float deltaTime)
{
	// Construct segment in direction of travel
	const float segmentLength = 30.0f;
	Vector3 start = m_Owner->GetPosition();
	Vector3 dir = m_Owner->GetForward();
	Vector3 end = start + dir * segmentLength;

	// Create line segment
	LineSegment l(start, end);

	// Test segment vs world
	PhysWorld* phys = m_Owner->GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	// (Don't collide vs player)
	if (phys->SegmentCast(l, info) && info.m_Actor != m_Player)
	{
		// If we collided, reflect the ball about the normal
		dir = Vector3::Reflect(dir, info.m_Normal);
		m_Owner->RotateToNewForward(dir);
		// Did we hit a target?
		TargetActor* target = dynamic_cast<TargetActor*>(info.m_Actor);
		if (target)
		{
			static_cast<BallActor*>(m_Owner)->HitTarget();
		}
	}

	// Base class update moves based on forward speed
	MoveComponent::Update(deltaTime);
}

void BallMoveComp::SetPlayer(Actor* player)
{
  m_Player = player;
}
