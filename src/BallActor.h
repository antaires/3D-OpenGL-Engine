#pragma once

#include "Actor.h"

class BallActor: public Actor
{
public:
  BallActor(class Game* game);

  void UpdateActor(float deltaTime) override;
  void SetPlayer(Actor* player);

  void HitTarget();

private:
  class BallMoveComp* m_MyMove;
  float m_LifeSpan;
};
