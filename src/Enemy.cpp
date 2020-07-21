#include "Enemy.h"
#include "SDL2/SDL.h"
#include "AnimSpriteComponent.h"
#include "CircleComponent.h"
#include "RectangleComponent.h"
#include "AIComponent.h"
#include "AIPatrol.h"
#include "AIHunt.h"
#include "AIAttack.h"
#include "NavigationComponent.h"
#include "Player.h"
#include "Game.h"
#include "Texture.h"


#include <iostream> // todo remove
// TODO: add AI component for movement

Enemy::Enemy(class Game* game, class Map* map)
  : Actor(game)
  , m_Circle(nullptr)
  , m_Rectangle(nullptr)
{
  SetScale(0.1f);

  // set up animation component
  m_AnimSpriteComponent = new AnimSpriteComponent(this);
  std::vector<Texture*> anims = {
      game->GetTexture("assets/asteroid.png")     // 0
      , game->GetTexture("assets/asteroid_exp_1.png") // death 1 - 4
      , game->GetTexture("assets/asteroid_exp_2.png")
      , game->GetTexture("assets/asteroid_exp_3.png")
      , game->GetTexture("assets/empty.png")

  };
  m_AnimSpriteComponent->SetAnimTextures(anims);

  // set names and ranges of animations
  m_AnimSpriteComponent->SetAnimationClip("right", 0, 0, true);
  m_AnimSpriteComponent->SetAnimationClip("left", 0, 0, true);
  m_AnimSpriteComponent->SetAnimationClip("down", 0, 0, false);
  m_AnimSpriteComponent->SetAnimationClip("up", 0, 0, false);
  m_AnimSpriteComponent->SetAnimationClip("stillRight", 0, 0, false);
  m_AnimSpriteComponent->SetAnimationClip("stillLeft", 0, 0, false);
  m_AnimSpriteComponent->SetAnimationClip("stillUp", 0, 0, false);
  m_AnimSpriteComponent->SetAnimationClip("stillDown", 0, 0, false);
  m_AnimSpriteComponent->SetAnimationClip("dying", 1, 4, false);

  // set actor heigth / width from texture and scale
  SetHeight(m_AnimSpriteComponent->GetTextureHeight() * GetScale());
  SetWidth(m_AnimSpriteComponent->GetTextureWidth() * GetScale());

  // set up circle component for collisions
  m_Circle = new CircleComponent(this);
  m_Circle->SetRadius( m_AnimSpriteComponent->GetTextureWidth() / 2 * GetScale());

  // set up rectangle component for collisions
  m_Rectangle = new RectangleComponent(this);
  m_Rectangle->SetWidth(GetWidth());
  m_Rectangle->SetHeight(GetHeight());

  // add AI component for state machine AI behaviour
  AIComponent* aic = new AIComponent(this, map);
  // register states with ai component
  aic->RegisterState(new AIPatrol(aic));
  aic->RegisterState(new AIHunt(aic));
  aic->RegisterState(new AIAttack(aic));
  // set initial state
  aic->ChangeState("Patrol");

  // add navigation component
  NavigationComponent* nav = new NavigationComponent(this);
  nav->SetForwardSpeed(ENEMY_SPEED);
}

void Enemy::UpdateActor(float deltaTime)
{
  Actor::UpdateActor(deltaTime);

  // TODO place all of this on AnimSpriteComponent, based on isMoving, isFiring, velocity, forwardVector
  // since NPCs will need this same logic

  bool isMoving = true;
  Vector2 velocity = GetVelocity();
  if (velocity.x == 0 && velocity.y == 0)
  {
    isMoving = false;
  }

  Vector2 forwardVector = Vector2::Normalize(GetForwardVector());
  Vector2 rightVector = Vector2(1, 0);
  float angle = Math::Acos(Vector2::Dot(rightVector, forwardVector)); //Math::Atan2(forwardVector.y, forwardVector.x);
  bool isAbove = forwardVector.y < 0;

  // set animation based on movement and forward vector
  // TODO convert forwardVector to rotation angle, and add animations for vertical movement as well
  if (angle > 2)
  {
    if (isMoving)
    {
      m_AnimSpriteComponent->SetCurrentAnimationClip("left");
    } else {
      m_AnimSpriteComponent->SetCurrentAnimationClip("stillLeft");
    }
  }
  if (angle < 2)
  {
    if(isMoving)
    {
      m_AnimSpriteComponent->SetCurrentAnimationClip("right");
    } else {
      m_AnimSpriteComponent->SetCurrentAnimationClip("stillRight");
    }
  }
  if ( !isAbove && (angle < 2 && angle > 1) )
  {
    if(isMoving)
    {
      m_AnimSpriteComponent->SetCurrentAnimationClip("down");
    } else {
      m_AnimSpriteComponent->SetCurrentAnimationClip("stillDown");
    }
  }
  if ( isAbove && (angle < 2 && angle > 1) )
  {
    if(isMoving)
    {
      m_AnimSpriteComponent->SetCurrentAnimationClip("up");
    } else {
      m_AnimSpriteComponent->SetCurrentAnimationClip("stillUp");
    }
  }
}

class CircleComponent* Enemy::GetCircle() const
{
  return m_Circle;
}

class RectangleComponent* Enemy::GetRectangle() const { return m_Rectangle; }

// void Enemy::ProcessKeyboard(const uint8_t* state){}

void Enemy::HandleDeath()
{
  if (!m_DyingStarted)
  {
    // if first time here, start dying animation
    m_AnimSpriteComponent->SetCurrentAnimationClip("dying");
  }

  // else if anim done, set to m_Dead state
  else if (m_AnimSpriteComponent->GetPreviousAnimationClip() == m_AnimSpriteComponent->GetCurrentAnimationClip())
  {
    SetState(Actor::E_Dead);
  }
}
