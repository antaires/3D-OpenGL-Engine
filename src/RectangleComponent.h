#pragma once

#include "Component.h"

class RectangleComponent: public Component
{
private:
  float m_Width;
  float m_Height;
public:
  RectangleComponent(class Actor* owner);

  void SetWidth(float width);
  void SetHeight(float height);
  float GetWidth() const;
  float GetHeight() const;
  const Vector2& GetCenter() const;
  const Vector2& GetTopCornerPosition() const;
};
