#pragma once

#include "Math.h"
#include <vector>

struct LineSegment
{
  Vector3 m_Start;
  Vector3 m_End;
  LineSegment(const Vector3& start, const Vector3& end);
  // poing along segment where 0 <= t <= 1
  Vector3 PointOnSegment(float t) const;
  // get min distance squared between point and line segment
  float MinDistSq(const Vector3& point) const;
  // get min dist sq between 2 line segments
  static float MinDistSq(const LineSegment& s1, const LineSegment& s2);
};

struct Plane
{
  Vector3 m_Normal;
  float m_Dist; // signed minimal distance between plane and origin
  Plane(Vector3& a, Vector3& b, Vector3& c);
  // get signed distance of point from plane
  float SignedDist(const Vector3& point) const;
};

//***********************//
//    BOUNDING VOLUMES   //
//***********************//

struct Sphere
{
  Vector3 m_Center;
  float m_Radius;
  bool Contains(const Vector3& point) const;
};

struct AABB
{
  // axis aligned bounding box
  // edges are parallel to one of the coord axis planes
  // 2 points: min = bottom left point, max = top right point
  Vector3 m_Min;
  Vector3 m_Max;
  AABB(const Vector3& min, const Vector3& max);
  void UpdateMinMax(const Vector3& point);
  void Rotate(const Quaternion& quat);
  bool Contains(const Vector3& point) const;
  float MinDistSq(const Vector3& point) const;
};

struct OBB
{
  // oriented bounding box - maintains tightness of bounds regardless of rotation
  // BUT collision computations are much more expensive than with AABBs
  Vector3 m_Center;
  Quaternion m_Rotation;
  Vector3 m_Extents;
};

struct Capsule
{
  LineSegment m_Segment;
  float m_Radius;
  bool Contains(const Vector3& point) const;
};

//*************************//
//   INTERSECTION FUNCS    //
//*************************//

bool Intersect(const Sphere& a, const Sphere& b);
bool Intersect(const AABB& a, const AABB& b);
bool Intersect(const Capsule& a, const Capsule& b);
bool Intersect(const Sphere& s, const AABB& box);

bool Intersect(const LineSegment& l, const Sphere& s, float& outT);
bool Intersect(const LineSegment& l, const Plane& p, float& outT);
bool Intersect(const LineSegment& l, const AABB& b, float& outT, Vector3& outNorm);

bool SweptSphere(const Sphere& P0, const Sphere& P1, const Sphere& Q0, const Sphere& Q1, float& outT);

// helper
bool TestSidePlane(float start, float end, float negd, const Vector3& norm,
	std::vector<std::pair<float, Vector3>>& out);
