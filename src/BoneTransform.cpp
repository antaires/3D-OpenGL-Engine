#include "BoneTransform.h"

Matrix4 BoneTransform::ToMatrix() const
{
  Matrix4 rot   = Matrix4::CreateFromQuaternion(m_Rotation);
  Matrix4 trans = Matrix4::CreateTranslation(m_Translation);

  return rot * trans;
}

BoneTransform BoneTransform::Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
  BoneTransform retVal;
  retVal.m_Rotation = Quaternion::Slerp(a.m_Rotation, b.m_Rotation, f);
  retVal.m_Translation = Vector3::Lerp(a.m_Translation, b.m_Translation, f);
  return retVal;
}
