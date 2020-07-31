#pragma once
#include "Actor.h"

class FollowActor : public Actor
{
public:
	FollowActor(class Game* game);

  void UpdateActor(float deltaTime) override;
  void ActorInput(const InputState& state) override;

	void SetVisible(bool visible);
private:
	class MoveComponent* m_MoveComp;
	class FollowCamera* m_CameraComp;
	class SkeletalMeshComponent* m_MeshComp;
	bool m_Moving;
  class BoxComponent* m_BoxComp;
  float m_ShootTimer;

  void FixCollisions();
};
