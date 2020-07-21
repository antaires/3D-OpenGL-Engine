#include "AIPatrol.h"
#include "AIComponent.h"
#include "Actor.h"
#include "Game.h"

#include <iostream> // todo remove

AIPatrol::AIPatrol(class AIComponent* owner)
  :AIState(owner)
{}

// override with behaviours for this state
void AIPatrol::Update(float deltaTime)
{
  // if STAY IN PATROL:
  Actor* owner = m_Owner->GetOwner();
  Vector2 diff = owner->GetPosition() - owner->GetGoal();
  Vector2 prevPos = owner->GetPrevPosition();
  Vector2 pos = owner->GetPosition();
  if ( diff.Length() < 1.0f || (prevPos.x == pos.x && prevPos.y == pos.y) )
  {
    // set as next in vector
    if (!m_Path.empty())
    {
      owner->SetGoal(m_Path.at(0));
      m_Path.erase(m_Path.begin() + 0); // erase first element
    }
    else
    {
      // if path empty then pick a new random target location (only in center of a cell grid)
      // and set goal using the m_Map
      m_Owner->GetMap()->GetPath(owner->GetPosition(), m_Owner->GetMap()->GetRandomOpenPosition(), m_Path);
      if (!m_Path.empty())
      {
        // path found, set next goal point as next node in path
        owner->SetGoal(m_Path.at(0));
        m_Path.erase(m_Path.begin() + 0);
      }
      else
      {
        // no path found, default to zero position & try again
        m_Path.push_back(Vector2(0.0f, 0.0f));
      }
    }
  }

  // TODO if 'see' player using line-of-sight (collision ray, player)
  // if player within x distance, switch to hunt / attack
  Vector2 playerPos = owner->GetGame()->GetPlayerPosition();
  Vector2 distanceToPlayer = playerPos - owner->GetPosition();
  if ( distanceToPlayer.Length() < ENEMY_HUNT_DISTANCE)
  {
    m_Owner->ChangeState("Hunt");
  }
}

void AIPatrol::OnEnter()
{
  m_Owner->GetOwner()->SetGoal(Vector2(SCREEN_WIDTH/2, SCREEN_HEIGHT/2));
}

void AIPatrol::OnExit()
{}

const char* AIPatrol::GetName() const
{
  return "Patrol";
}
