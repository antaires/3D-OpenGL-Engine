#include "CameraComponent.h"

class FPSCamera: public CameraComponent
{
public:
  FPSCamera(class Actor* owner);

  void Update(float deltaTime) override;

  float GetPitch() const;
  float GetPitchSpeed() const;
  float GetMaxPitch() const;

  void SetPitchSpeed(float speed);
  void SetMaxPitch(float pitch);

private:
  float m_PitchSpeed;
  float m_MaxPitch;
  float m_Pitch;
};
