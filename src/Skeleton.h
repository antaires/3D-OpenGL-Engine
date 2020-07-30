#pragma once

#include "BoneTransform.h"
#include "Constants.h"
#include <string>
#include <vector>

class Skeleton
{
public:
  // definition for each bone in the Skeleton
  struct Bone
  {
    BoneTransform m_LocalBindPose;
    std::string m_Name;
    int m_Parent;
  };
  
  // Load from file
  bool Load(const std::string& fileName);

  // getters
  size_t GetNumBones() const;
  const Bone& GetBone(size_t idx) const;
  const std::vector<Bone>& GetBones() const;
  const std::vector<Matrix4>& GetGlobalInvBindPoses() const;

  // computes the global inverse bind pose for each bone
  // called when loading the skeleton
  void ComputeGlobalInvBindPose();

private:
  std::vector<Bone> m_Bones;
  std::vector<Matrix4> m_GlobalInvBindPoses;
};
