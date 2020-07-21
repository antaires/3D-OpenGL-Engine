#include "EnemyBullet.h"
#include "CollisionDetection.h"
#include "AnimSpriteComponent.h"
#include "MoveComponent.h"
#include "CircleComponent.h"
#include "Game.h"
#include "Texture.h"

// bullet class for player to hit and kill enemies
EnemyBullet::EnemyBullet(class Game* game, Vector2 forwardVector)
  :Actor(game)
  {
    SetScale(0.05f);

    // set up anim sprite component
    m_AnimSpriteComponent = new AnimSpriteComponent(this);
    std::vector<Texture*> anims = {
      game->GetTexture("assets/playerBullet01.png") // TODO use different bullet
      , game->GetTexture("assets/playerBullet01.png")
      , game->GetTexture("assets/playerBullet02.png")
      , game->GetTexture("assets/playerBullet02.png")
    };
    m_AnimSpriteComponent->SetAnimTextures(anims);
    // set names and ranges of animations
    m_AnimSpriteComponent->SetAnimationClip("fire", 0, 1, true);
    m_AnimSpriteComponent->SetAnimationClip("die", 2, 3, false);

    //set to fire to start
    m_AnimSpriteComponent->SetCurrentAnimationClip("fire");

    // set actor heigth / width from texture and scale
    SetHeight(m_AnimSpriteComponent->GetTextureHeight() * GetScale());
    SetWidth(m_AnimSpriteComponent->GetTextureWidth() * GetScale());

    // move component
    MoveComponent* mc = new MoveComponent(this);
    mc->SetVelocity(forwardVector);

    // set up circle component for collisions
    m_Circle = new CircleComponent(this);
    m_Circle->SetRadius( m_AnimSpriteComponent->GetTextureWidth() / 2 * GetScale());
  }

void EnemyBullet::UpdateActor(float deltaTime)
{
  // do you hit the player?
  Player* player = GetGame()->GetPlayer();

  if( CollisionDetection::HasCollision(player->GetCircle(), m_Circle) )
  {
    player->SetState(Actor::E_Dying);
    SetState(Actor::E_Dying);
  }

  if (m_HasCollision) // if hits a wall
  {
    SetState(Actor::E_Dying);
  }

}

void EnemyBullet::HandleDeath()
{
  if (!m_DyingStarted)
  {
    // if first time here, start dying animation
    m_AnimSpriteComponent->SetCurrentAnimationClip("die");
  }
  // else if anim done, set to m_Dead state
  else if (m_AnimSpriteComponent->GetPreviousAnimationClip() == m_AnimSpriteComponent->GetCurrentAnimationClip())
  {
    SetState(Actor::E_Dead);
  }
}
