#include "AIAttack.h"
#include "AIComponent.h"
#include "Actor.h"
#include "Game.h"
#include "EnemyBullet.h"

#include <iostream> // todo remove

AIAttack::AIAttack(class AIComponent* owner)
  :AIState(owner)
  , m_ShotTimer(0)
{}

// override with behaviours for this state
void AIAttack::Update(float deltaTime)
{
  // if STAY IN ATTACK:
  Actor* owner = m_Owner->GetOwner();
  Vector2 playerPos = owner->GetGame()->GetPlayerPosition();
  Vector2 toPlayer = playerPos - owner->GetPosition();

  // spawn bullet
  if (m_ShotTimer <= 0)
  {
    toPlayer.Normalize();
    toPlayer.x *= ENEMY_BULLET_SPEED;
    toPlayer.y *= ENEMY_BULLET_SPEED;
    EnemyBullet* bullet = new EnemyBullet(owner->GetGame(), toPlayer);
    bullet->SetPosition(owner->GetPosition());
    m_ShotTimer = ENEMY_SHOOT_TIMER;
  }

  owner->SetGoal(playerPos);
  // if player over x distance, switch to hunt
  if ( toPlayer.Length() > ENEMY_ATTACK_DISTANCE )
  {
    m_Owner->ChangeState("Hunt");
  }
}

void AIAttack::OnEnter()
{}

void AIAttack::OnExit()
{}

const char* AIAttack::GetName() const
{
  return "Attack";
}
