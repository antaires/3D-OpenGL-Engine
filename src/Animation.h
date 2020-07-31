#pragma once

#include "Math.h"
#include "BoneTransform.h"
#include <string>
#include <vector>

class Animation
{
public:
  Animation();
  bool Load(const std::string& fileName);

  // getters
  size_t GetNumBones() const;
  float GetDuration() const;
  float GetFrameDuration() const;

  // fills the provided vector with the global (current) pose matrices
  // for each bone at the specified time in the animation
  void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const class Skeleton* inSkeleton, float inTime) const;

private:
  size_t m_NumBones;
  size_t m_NumFrames;
  float m_Duration;
  float m_FrameDuration;

  // transform info for each frame on the track
  // each index in the outer vector is a bone, inner vector is a frame
  std::vector<std::vector<BoneTransform>> m_Tracks;
};
