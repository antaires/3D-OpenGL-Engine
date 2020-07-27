#pragma once
#include "Actor.h"

class PlaneActor : public Actor
{
public:
	PlaneActor(class Game* game);
	~PlaneActor();
	class BoxComponent* GetBox() { return m_Box; }
private:
	class BoxComponent* m_Box;
};
