#pragma once

#include "Component.h"

class CircleComponent: public Component
{
private:
  float m_Radius;
public:
  CircleComponent(class Actor* owner);
  void SetRadius(float radius);
  float GetRadius() const;
  const Vector2& GetCenter() const;
};
