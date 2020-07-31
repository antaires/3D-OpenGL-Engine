#include "FollowCamera.h"
#include "Actor.h"

FollowCamera::FollowCamera(Actor* owner)
	:CameraComponent(owner)
	,m_HorzDist(350.0f)
	,m_VertDist(150.0f)
	,m_TargetDist(100.0f)
	,m_SpringConstant(128.0f)
{
}

void FollowCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);
	// Compute dampening from spring constant
	float dampening = 2.0f * Math::Sqrt(m_SpringConstant);
	// Compute ideal position
	Vector3 idealPos = ComputeCameraPos();
	// Compute difference between actual and ideal
	Vector3 diff = m_ActualPos - idealPos;
	// Compute acceleration of spring
	Vector3 acel = -m_SpringConstant * diff -
		dampening * m_Velocity;
	// Update velocity
	m_Velocity += acel * deltaTime;
	// Update actual camera position
	m_ActualPos += m_Velocity * deltaTime;
	// Target is target dist in front of owning actor
	Vector3 target = m_Owner->GetPosition() +
		m_Owner->GetForward() * m_TargetDist;
	// Use actual position here, not ideal
	Matrix4 view = Matrix4::CreateLookAt(m_ActualPos, target,
		Vector3::UnitZ);
	SetViewMatrix(view);
}

void FollowCamera::SnapToIdeal()
{
	// Set actual position to ideal
	m_ActualPos = ComputeCameraPos();
	// Zero velocity
	m_Velocity = Vector3::Zero;
	// Compute target and view
	Vector3 target = m_Owner->GetPosition() +
		m_Owner->GetForward() * m_TargetDist;
	// Use actual position here, not ideal
	Matrix4 view = Matrix4::CreateLookAt(m_ActualPos, target,
		Vector3::UnitZ);
	SetViewMatrix(view);
}

Vector3 FollowCamera::ComputeCameraPos() const
{
	// Set camera position behind and above owner
	Vector3 cameraPos = m_Owner->GetPosition();
	cameraPos -= m_Owner->GetForward() * m_HorzDist;
	cameraPos += Vector3::UnitZ * m_VertDist;
	return cameraPos;
}
