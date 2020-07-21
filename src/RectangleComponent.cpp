#include "RectangleComponent.h"
#include "Math.h"

RectangleComponent::RectangleComponent(class Actor* owner)
  :Component(owner)
  , m_Width(0)
  , m_Height(0)
{}

void RectangleComponent::SetWidth(float width) { m_Width = width; }

void RectangleComponent::SetHeight(float height) { m_Height = height; }

float RectangleComponent::GetWidth() const { return m_Width; }

float RectangleComponent::GetHeight() const { return m_Height; }

const Vector2& RectangleComponent::GetCenter() const { return m_Owner->GetPosition(); }

const Vector2& RectangleComponent::GetTopCornerPosition() const
{
  return m_Owner->GetTopCornerPosition();
}
