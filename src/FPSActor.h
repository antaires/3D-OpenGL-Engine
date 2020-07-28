#pragma once

#include "Actor.h"

class FPSActor: public Actor
{
public:
  FPSActor(class Game* game);

	void UpdateActor(float deltaTime) override;
  void ActorInput(const InputState& state) override;

  void FixCollisions();
private:
	class MoveComponent* m_MoveComp;
  class FPSCamera* m_Camera;
  Actor* m_Model;
  class MeshComponent* m_MeshComp;
  class BoxComponent* m_BoxComp;
};
