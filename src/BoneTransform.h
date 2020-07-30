#pragma once

#include "Math.h"

class BoneTransform
{
public:
  // for now, public
  Quaternion m_Rotation;
  Vector3 m_Translation;
  Matrix4 ToMatrix() const; // convert to matrix

  static BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f);
};
