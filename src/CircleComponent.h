#pragma once

#include "Component.h"
#include "Math.h"

class CircleComponent: public Component
{
private:
  float m_Radius;
public:
  CircleComponent(class Actor* owner);
  void SetRadius(float radius);
  float GetRadius() const;
  const Vector3& GetCenter() const;
};

bool Intersect(const CircleComponent& a, const CircleComponent& b);
