#include "FPSCamera.h"
#include "Actor.h"

FPSCamera::FPSCamera(class Actor* owner)
  :CameraComponent(owner)
  , m_PitchSpeed(0.0f)
  , m_MaxPitch(Math::Pi / 3.0f)
  , m_Pitch(0.0f)
{}

void FPSCamera::Update(float deltaTime)
{
  CameraComponent::Update(deltaTime);

  // camera position is owner position
  Vector3 cameraPos = m_Owner->GetPosition();

  // update pitch by pitch speed
  m_Pitch = m_PitchSpeed * deltaTime;
  // clamp pitch to [-max, +max]
  m_Pitch = Math::Clamp(m_Pitch, -m_MaxPitch, m_MaxPitch);
  // make quaternion representing pitch rotation
  // which is about owner's right vector
  Quaternion pitchQuaternion(m_Owner->GetRight(), m_Pitch);

  // rotate owner forward by pitch quaternion
  Vector3 viewForward = Vector3::Transform(m_Owner->GetForward(), pitchQuaternion);
  // target position 100 units in front of view forward
  Vector3 target = cameraPos + viewForward * 100.0f;
  // rotate up by pitch quaternion
  Vector3 up = Vector3::Transform(Vector3::UnitZ, pitchQuaternion);

  // create look at matrix, set as view
  Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
  SetViewMatrix(view);
}

float FPSCamera::GetPitch() const { return m_Pitch; }
float FPSCamera::GetPitchSpeed() const { return m_PitchSpeed; }
float FPSCamera::GetMaxPitch() const { return m_MaxPitch; }

void FPSCamera::SetPitchSpeed(float speed) { m_PitchSpeed = speed; }
void FPSCamera::SetMaxPitch(float pitch) { m_MaxPitch = pitch; }
