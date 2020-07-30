#pragma once

#include "MeshComponent.h"
#include "MatrixPalette.h"

class SkeletalMeshComponent: public MeshComponent
{
public:
  SkeletalMeshComponent(class Actor* owner);

  void Draw(class Shader* shader) override;

  void ComputeMatrixPalette();
  float PlayAnimation(const class Animation* anim, float playRate);
private:
  class Skeleton* m_Skeleton;

  MatrixPalette m_Palette;
  // animation currently playing
  const class Animation* m_Animation;
  // play rate of animation (1.0 is normal speed)
  float m_AnimPlayRate;
  // current time in the animation
  float m_AnimTime;
};
