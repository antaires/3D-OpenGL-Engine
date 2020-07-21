#pragma once

#include "Actor.h"

// enemy bullet for killing player
class EnemyBullet: public Actor
{
private:
  class AnimSpriteComponent* m_AnimSpriteComponent;
  class CircleComponent* m_Circle;
public:
  EnemyBullet(class Game* game, Vector2 forwardVector);

  void UpdateActor(float deltaTime) override;
  void HandleDeath() override;
};
