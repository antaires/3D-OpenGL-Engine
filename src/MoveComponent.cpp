#include "MoveComponent.h"
#include "Actor.h"
#include "Math.h"

MoveComponent::MoveComponent(class Actor* owner, int updateOrder)
  :Component(owner, updateOrder)
  , m_AngularSpeed(0.0f)
  , m_ForwardSpeed(0.0f)
  , m_StrafeSpeed(0.0f)
  , m_JumpSpeed(0.0f)
  , m_IsGrounded(false)
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

  Vector3 pos = m_Owner->GetPosition();
  if (!Math::NearZero(m_ForwardSpeed) ||
      !Math::NearZero(m_StrafeSpeed)  ||
      !Math::NearZero(m_JumpSpeed))
  {
    // update pos based on forward speed
    pos += m_Owner->GetForward() * m_ForwardSpeed * deltaTime;

    // update position based on strafe
    pos += m_Owner->GetRight() * m_StrafeSpeed * deltaTime;

    // update based on up (jump) speed
    if (m_JumpSpeed > 0.0f)
    {
      pos += m_Owner->GetUp() * m_JumpSpeed * deltaTime;
      m_JumpSpeed -= 50;
    }
  }

  // add gravity
  if (!m_IsGrounded)
  {
    pos += Vector3(0.0f, 0.0f, -330.0f) * deltaTime;
  }

  // update position
  m_Owner->SetPosition(pos);
}

float MoveComponent::GetAngularSpeed() const { return m_AngularSpeed; }
float MoveComponent::GetForwardSpeed() const { return m_ForwardSpeed; }
float MoveComponent::GetStrafeSpeed() const { return m_StrafeSpeed; }
float MoveComponent::GetJumpSpeed() const { return m_JumpSpeed; }

void MoveComponent::SetAngularSpeed(float angularSpeed) { m_AngularSpeed = angularSpeed; }
void MoveComponent::SetForwardSpeed(float speed) { m_ForwardSpeed = speed; }
void MoveComponent::SetStrafeSpeed(float speed) { m_StrafeSpeed = speed; }
void MoveComponent::SetJumpSpeed(float speed) { m_JumpSpeed = speed; }

void MoveComponent::SetIsGrounded(bool value) { m_IsGrounded = value; }
bool MoveComponent::GetIsGrounded() const { return m_IsGrounded; }
