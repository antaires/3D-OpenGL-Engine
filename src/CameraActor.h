#pragma once
#include "Actor.h"

class CameraActor : public Actor
{
public:
	CameraActor(class Game* game);

	void UpdateActor(float deltaTime) override;
	void ActorInput(const InputState& state) override;
private:
	class MoveComponent* m_MoveComp;
};
