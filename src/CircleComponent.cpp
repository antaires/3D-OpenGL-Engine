#include "CircleComponent.h"

CircleComponent::CircleComponent(class Actor* owner)
  :Component(owner)
  , m_Radius(0)
{}

void CircleComponent::SetRadius(float radius)
{
  m_Radius = radius;
}

float CircleComponent::GetRadius() const
{
  return m_Radius;
}

const Vector2& CircleComponent::GetCenter() const
{
  return m_Owner->GetPosition();
}
