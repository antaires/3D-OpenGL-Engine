#include "AIHunt.h"
#include "AIComponent.h"
#include "Actor.h"
#include "Game.h"

#include <iostream> // todo remove

AIHunt::AIHunt(class AIComponent* owner)
  :AIState(owner)
{}

// override with behaviours for this state
void AIHunt::Update(float deltaTime)
{
  // if STAY IN HUNT:
  Actor* owner = m_Owner->GetOwner();
  Vector2 playerPos = owner->GetGame()->GetPlayerPosition();
  Vector2 toPlayer = playerPos - owner->GetPosition();
  //owner->SetGoal(playerPos);

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
      // if path empty then get new path to player position
      m_Owner->GetMap()->GetPath(owner->GetPosition(), playerPos, m_Path);
      if (!m_Path.empty())
      {
        // path found, set next goal point as next node in path
        owner->SetGoal(m_Path.at(0));
        m_Path.erase(m_Path.begin() + 0);
      }
      else
      {
        // no path found, default to towards player
        m_Path.push_back(playerPos);
      }
    }
  }



  // if player over x distance, switch to patrol
  if ( toPlayer.Length() > ENEMY_HUNT_DISTANCE )
  {
    m_Owner->ChangeState("Patrol");
  }

  // if player close enough, attack
  if ( toPlayer.Length() < ENEMY_ATTACK_DISTANCE)
  {
    m_Owner->ChangeState("Attack");
  }

}

void AIHunt::OnEnter()
{
  Actor* owner = m_Owner->GetOwner();
  Vector2 playerPos = owner->GetGame()->GetPlayerPosition();

  // if path empty then get new path to player position
  m_Owner->GetMap()->GetPath(owner->GetPosition(), playerPos, m_Path);
  if (!m_Path.empty())
  {
    // path found, set next goal point as next node in path
    owner->SetGoal(m_Path.at(0));
    m_Path.erase(m_Path.begin() + 0);
  }
  else
  {
    // no path found, default to towards player
    m_Path.push_back(playerPos);
  }

}

void AIHunt::OnExit()
{}

const char* AIHunt::GetName() const
{
  return "Hunt";
}
