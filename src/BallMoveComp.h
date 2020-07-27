#pragma once
#include "MoveComponent.h"

class BallMoveComp : public MoveComponent
{
public:
	BallMoveComp(class Actor* owner);

	void SetPlayer(Actor* player);
	void Update(float deltaTime) override;
protected:
	class Actor* m_Player;
};
