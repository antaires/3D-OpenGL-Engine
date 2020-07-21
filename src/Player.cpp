#include "Player.h"
#include "SDL2/SDL.h"
#include "AnimSpriteComponent.h"
#include "InputComponent.h"
#include "CircleComponent.h"
#include "RectangleComponent.h"
#include "Bullet.h"
#include "Game.h"
#include "Texture.h"

#include <iostream> // remove

Player::Player(class Game* game)
  : Actor(game)
  , m_Circle(nullptr)
  , m_Rectangle(nullptr)
{
  SetScale(0.1f);

  // set up move component
  m_InputComponent = new InputComponent(this);
  m_InputComponent->SetClampToScreen(true);

  // set up animation component
  m_AnimSpriteComponent = new AnimSpriteComponent(this);
  std::vector<Texture*> anims = {
      game->GetTexture("assets/ship_up_still.png")    // up still 0
    , game->GetTexture("assets/ship_up_moving_1.png") // up 1 - 3
    , game->GetTexture("assets/ship_up_moving_2.png")
    , game->GetTexture("assets/ship_up_moving_3.png")
    , game->GetTexture("assets/ship_down_still.png")  // down still 4
    , game->GetTexture("assets/ship_down_moving_1.png") // down 5 - 7
    , game->GetTexture("assets/ship_down_moving_2.png")
    , game->GetTexture("assets/ship_down_moving_3.png")
    , game->GetTexture("assets/ship_left_still.png")  // left still 8
    , game->GetTexture("assets/ship_left_moving_1.png") // left 9 - 11
    , game->GetTexture("assets/ship_left_moving_2.png")
    , game->GetTexture("assets/ship_left_moving_3.png")
    , game->GetTexture("assets/ship_right_still.png")  // right still 12
    , game->GetTexture("assets/ship_right_moving_1.png") // right 13 - 15
    , game->GetTexture("assets/ship_right_moving_2.png")
    , game->GetTexture("assets/ship_right_moving_3.png")
  };
  m_AnimSpriteComponent->SetAnimTextures(anims);

  // set names and ranges of animations
  m_AnimSpriteComponent->SetAnimationClip("right", 13, 15, true);
  m_AnimSpriteComponent->SetAnimationClip("left", 9, 11, true);
  m_AnimSpriteComponent->SetAnimationClip("down", 5, 7, true);
  m_AnimSpriteComponent->SetAnimationClip("up", 1, 3, true);
  m_AnimSpriteComponent->SetAnimationClip("stillRight", 12, 12, false);
  m_AnimSpriteComponent->SetAnimationClip("stillLeft", 8, 8, false);
  m_AnimSpriteComponent->SetAnimationClip("stillUp", 0, 0, false);
  m_AnimSpriteComponent->SetAnimationClip("stillDown", 4, 4, false);

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

}

void Player::UpdateActor(float deltaTime)
{
  Actor::UpdateActor(deltaTime);

  m_InputComponent->DecrementShotTimer(deltaTime);

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
      m_AnimSpriteComponent->SetCurrentAnimationClip("up");
    } else {
      m_AnimSpriteComponent->SetCurrentAnimationClip("stillUp");
    }
  }
  if ( isAbove && (angle < 2 && angle > 1) )
  {
    if(isMoving)
    {
      m_AnimSpriteComponent->SetCurrentAnimationClip("down");
    } else {
      m_AnimSpriteComponent->SetCurrentAnimationClip("stillDown");
    }
  }

}

void Player::ProcessKeyboard(const uint8_t* state)
{}

class CircleComponent* Player::GetCircle() const { return m_Circle; }

class RectangleComponent* Player::GetRectangle() const { return m_Rectangle; }

void Player::HandleDeath()
{
  // TODO - play death animation & freeze motion
}
