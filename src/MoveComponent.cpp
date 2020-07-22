#include "MoveComponent.h"
#include "Actor.h"
#include "Math.h"

MoveComponent::MoveComponent(class Actor* owner, int updateOrder)
  :Component(owner, updateOrder)
  , m_AngularSpeed(0.0f)
  , m_ForwardSpeed(0.0f)
{}

void MoveComponent::Update(float deltaTime)
{
  if(!Math::NearZero(m_AngularSpeed))
  {
    Quaternion rot = m_Owner->GetRotation();
    float angle = m_AngularSpeed * deltaTime;

    // create Quaternion for incremental rotation
    // rotate about up axis
    Quaternion inc(Vector3::UnitZ, angle);

    // concatenate old and new quaternion (multiply)
    rot = Quaternion::Concatenate(rot, inc);
    m_Owner->SetRotation(rot);
  }

  if (!Math::NearZero(m_ForwardSpeed))
  {
    Vector3 pos = m_Owner->GetPosition();
    pos += m_Owner->GetForward() * m_ForwardSpeed * deltaTime;
    m_Owner->SetPosition(pos);
  }
}

float MoveComponent::GetAngularSpeed() const { return m_AngularSpeed; }
float MoveComponent::GetForwardSpeed() const { return m_ForwardSpeed; }
void MoveComponent::SetAngularSpeed(float angularSpeed) { m_AngularSpeed = angularSpeed; }
void MoveComponent::SetForwardSpeed(float speed) { m_ForwardSpeed = speed; }
