#pragma once
#include "CameraComponent.h"

class FollowCamera : public CameraComponent
{
public:
	FollowCamera(class Actor* owner);

	void Update(float deltaTime) override;

	void SnapToIdeal();

	void SetHorzDist(float dist) { m_HorzDist = dist; }
	void SetVertDist(float dist) { m_VertDist = dist; }
	void SetTargetDist(float dist) { m_TargetDist = dist; }
	void SetSpringConstant(float spring) { m_SpringConstant = spring; }
private:
	Vector3 ComputeCameraPos() const;

	// Actual position of camera
	Vector3 m_ActualPos;
	// Velocity of actual camera
	Vector3 m_Velocity;
	// Horizontal follow distance
	float m_HorzDist;
	// Vertical follow distance
	float m_VertDist;
	// Target distance
	float m_TargetDist;
	// Spring constant (higher is more stiff)
	float m_SpringConstant;
};
