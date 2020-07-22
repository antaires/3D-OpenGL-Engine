#include "CircleComponent.h"
#include "Actor.h"

CircleComponent::CircleComponent(class Actor* owner)
  :Component(owner)
  , m_Radius(0.0f)
{}

void CircleComponent::SetRadius(float radius)
{
  m_Radius = radius;
}

float CircleComponent::GetRadius() const
{
  return m_Owner->GetScale() * m_Radius;
}

const Vector3& CircleComponent::GetCenter() const
{
  return m_Owner->GetPosition();
}

bool Intersect(const CircleComponent& a, const CircleComponent& b)
{
  // calc distance squared
  Vector3 diff = a.GetCenter() - b.GetCenter();
  float distSq = diff.LengthSq();

  // calc sum of radii squared
  float radiiSq = a.GetRadius() + b.GetRadius();
  radiiSq *= radiiSq;

  return distSq <= radiiSq;
}
