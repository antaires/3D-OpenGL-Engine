#include "NavigationComponent.h"

NavigationComponent::NavigationComponent(class Actor* owner, int updateOrder)
  :MoveComponent(owner, updateOrder)
{}

void NavigationComponent::Update(float deltaTime)
{
  Vector2 goal = m_Owner->GetGoal();
  Vector2 pos  = m_Owner->GetPosition();

  // if at goal or close enough, do nothing
  // else, move towards goal
  // get vector from pos to goal, then normalize it and multiple by deltaTime
  Vector2 diff = goal - pos;
  if (diff.Length() > 30)
  {
    diff.Normalize();
    SetVelocity(diff);
    SetForwardVector(diff); // face in same direction as movement
  }

  MoveComponent::Update(deltaTime);
}
