#pragma once

#include "Actor.h"

class FPSActor: public Actor
{
public:
  FPSActor(class Game* game);

	void UpdateActor(float deltaTime) override;
  void ActorInput(const InputState& state) override;
private:
	class MoveComponent* m_MoveComp;
};
