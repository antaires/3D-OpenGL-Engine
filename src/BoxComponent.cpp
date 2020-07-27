#include "BoxComponent.h"
#include "Game.h"
#include "Actor.h"
#include "PhysWorld.h"

BoxComponent::BoxComponent(class Actor* owner, int updateOrder)
  :Component(owner, updateOrder)
  , m_ObjectBox(Vector3::Zero, Vector3::Zero)
  , m_WorldBox(Vector3::Zero, Vector3::Zero)
  , m_ShouldRotate(true)
{
  m_Owner->GetGame()->GetPhysicalWorld()->AddBox(this);
}

BoxComponent::~BoxComponent()
{
  m_Owner->GetGame()->GetPhysicalWorld()->RemoveBox(this);
}

void BoxComponent::OnUpdateWorldTransform()
{
  // reset to object space box
  m_WorldBox = m_ObjectBox;

  // scale
  m_WorldBox.m_Min *= m_Owner->GetScale();
  m_WorldBox.m_Max *= m_Owner->GetScale();

  // rotate
  if(m_ShouldRotate)
  {
    m_WorldBox.Rotate(m_Owner->GetRotation());
  }

  // translate
  m_WorldBox.m_Min += m_Owner->GetPosition();
  m_WorldBox.m_Max += m_Owner->GetPosition();
}

void BoxComponent::SetObjectBox(const AABB& model)
{
  m_ObjectBox = model;
}

const AABB& BoxComponent::GetWorldBox() const
{
  return m_WorldBox;
}

void BoxComponent::SetShouldRotate(bool value)
{
  m_ShouldRotate = value;
}
