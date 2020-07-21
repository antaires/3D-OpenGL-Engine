#pragma once

#include "MoveComponent.h"
#include "Math.h"

// simple class to move actor towards goal position 
class NavigationComponent: public MoveComponent
{
private:
public:
  NavigationComponent(class Actor* owner, int updateOrder = 10);
  void Update(float deltaTime) override;
};
