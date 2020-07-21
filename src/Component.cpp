#include "Component.h"

Component::Component(class Actor* owner, int updateOrder)
  : m_Owner(owner)
  , m_UpdateOrder(updateOrder)
{
  // add to actor's vector of components
  m_Owner->AddComponent(this);
}

Component::~Component()
{
  m_Owner->RemoveComponent(this);
}

void Component::Update(float deltaTime)
{}

void Component::ProcessInput(const uint8_t* keyState)
{}

void Component::ProcessMouse(const uint32_t mouseState, const int x, const int y)
{}

void Component::OnUpdateWorldTransform(){}

int Component::GetUpdateOrder() const {return m_UpdateOrder;}

class Actor* Component::GetOwner()
{
  return m_Owner;
}
