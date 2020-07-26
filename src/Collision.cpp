#include "Collision.h"
#include <array>

//*****************************//
//       LINE SEGMENT          //
//*****************************//

LineSegment::LineSegment(const Vector3& start, const Vector3& end)
  :m_Start(start)
  , m_End(end)
{}

Vector3 LineSegment::PointOnSegment(float t) const
{
  return m_Start + (m_End - m_Start) * t;
}

float LineSegment::MinDistSq(const Vector3& point) const
{
  // construct vectors
  Vector3 ab = m_End - m_Start;
  Vector3 ba = -1.0f * ab;
  Vector3 ac = point - m_Start;
  Vector3 bc = point - m_End;

  // case 1 : C projects prior to A
  if (Vector3::Dot(ab, ac) < 0.0f)
  {
    return ac.LengthSq();
  }

  // case 2 : C projects after B
  else if(Vector3::Dot(ba, bc) < 0.0f)
  {
    return bc.LengthSq();
  }

  else
  {
    // compute p vector (length of ab until perpendicular to c)
    float scalar = Vector3::Dot(ac, ab) / Vector3::Dot(ab, ab);
    Vector3 p = scalar * ab;

    // compute length squared of ac - p
    return (ac - p).LengthSq();
  }
}

float LineSegment::MinDistSq(const LineSegment& s1, const LineSegment& s2)
{
	Vector3   u = s1.m_End - s1.m_Start;
	Vector3   v = s2.m_End - s2.m_Start;
	Vector3   w = s1.m_Start - s2.m_Start;
	float    a = Vector3::Dot(u, u);         // always >= 0
	float    b = Vector3::Dot(u, v);
	float    c = Vector3::Dot(v, v);         // always >= 0
	float    d = Vector3::Dot(u, w);
	float    e = Vector3::Dot(v, w);
	float    D = a*c - b*b;        // always >= 0
	float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

								   // compute the line parameters of the two closest points
	if (Math::NearZero(D)) { // the lines are almost parallel
		sN = 0.0;         // force using point P0 on segment S1
		sD = 1.0;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (Math::NearZero(sN) ? 0.0f : sN / sD);
	tc = (Math::NearZero(tN) ? 0.0f : tN / tD);

	// get the difference of the two closest points
	Vector3   dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)

	return dP.LengthSq();   // return the closest distance squared
}

//*****************************//
//          PLANE              //
//*****************************//

Plane::Plane(Vector3& a, Vector3& b, Vector3& c)
{
  // compute vectors from a to b and a to c
  Vector3 ab = b - a;
  Vector3 ac = c - a;

  // cross produce and normalize to get normal
  m_Normal = Vector3::Cross(ab, ac);
  m_Normal.Normalize();
  // dist is -P dot n
  m_Dist = -Vector3::Dot(a, m_Normal);
}

float Plane::SignedDist(const Vector3& point) const
{
  // a negative value means point is BELOW the plane (facing away from normal)
  // and a positive value means point is ABOVE the plane
  return Vector3::Dot(point, m_Normal) - m_Dist;
}

//*****************************//
//          SPHERE             //
//*****************************//

bool Sphere::Contains(const Vector3& point) const
{
  // get distance squared between center and point
  float distSq = (m_Center - point).LengthSq();
  return distSq <= (m_Radius * m_Radius);
}


//*****************************//
//          AABB               //
//*****************************//

AABB::AABB(const Vector3& min, const Vector3& max)
  :m_Min(min)
  , m_Max(max)
{
  // NOTE: to build AABB for a given model,
  // init min and max to first points in container, then
  // call UpdateMinMax over all points
  // -----------------------------------
  // AABB box(points[0], poitns[0]);
  // for (size_t i = 1; i < points.size(); ++i)
  // {
  //    box.UpdateMinMax(points[i]);
  // }
}

void AABB::UpdateMinMax(const Vector3& point)
{
  // udpate each component separately
  m_Min.x = Math::Min(m_Min.x, point.x);
  m_Min.y = Math::Min(m_Min.y, point.y);
  m_Min.z = Math::Min(m_Min.z, point.z);

  m_Max.x = Math::Min(m_Max.x, point.x);
  m_Max.y = Math::Min(m_Max.y, point.y);
  m_Max.z = Math::Min(m_Max.z, point.z);
}

void AABB::Rotate(const Quaternion& q)
{
	// Construct the 8 points for the corners of the box
	std::array<Vector3, 8> points;
	// Min point is always a corner
	points[0] = m_Min;
	// Permutations with 2 min and 1 max
	points[1] = Vector3(m_Max.x, m_Min.y, m_Min.z);
	points[2] = Vector3(m_Min.x, m_Max.y, m_Min.z);
	points[3] = Vector3(m_Min.x, m_Min.y, m_Max.z);
	// Permutations with 2 max and 1 min
	points[4] = Vector3(m_Min.x, m_Max.y, m_Max.z);
	points[5] = Vector3(m_Max.x, m_Min.y, m_Max.z);
	points[6] = Vector3(m_Max.x, m_Max.y, m_Min.z);
	// Max point corner
	points[7] = Vector3(m_Max);

	// Rotate first point
	Vector3 p = Vector3::Transform(points[0], q);
	// Reset min/max to first point rotated
	m_Min = p;
	m_Max = p;
	// Update min/max based on remaining points, rotated
	for (size_t i = 1; i < points.size(); i++)
	{
		p = Vector3::Transform(points[i], q);
		UpdateMinMax(p);
	}
}

bool AABB::Contains(const Vector3& point) const
{
  // if the point is any of these, its not contained:
  // to the left, right, above, below, in front, behind the box
  bool outside = point.x < m_Min.x ||
    point.y < m_Min.y ||
    point.z < m_Min.z ||
    point.x > m_Max.x ||
    point.y > m_Max.y ||
    point.z > m_Max.z;
  // if none of these are true, point is outside the box
  return !outside;
}

float AABB::MinDistSq(const Vector3& point) const
{
  // compute differences for each axis
  float dx = Math::Max(m_Min.x - point.x, 0.0f);
  dx       = Math::Max(dx, point.x - m_Max.x);

  float dy = Math::Max(m_Min.y - point.y, 0.0f);
  dy       = Math::Max(dy, point.y - m_Max.y);

  float dz = Math::Max(m_Min.z - point.z, 0.0f);
  dz       = Math::Max(dz, point.z - m_Max.z);

  // distance squared formula
  return dx * dx + dy * dy + dz * dz;
}

//*****************************//
//          OBB                //
//*****************************//


//*****************************//
//        Capsule              //
//*****************************//

bool Capsule::Contains(const Vector3& point) const
{
  // get minimal distance squared between point and line segment
  float distSq = m_Segment.MinDistSq(point);
  return distSq <= (m_Radius * m_Radius);
}

//*************************//
//   INTERSECTION FUNCS    //
//*************************//

bool Intersect(const Sphere& a, const Sphere& b)
{
  float distSq = (a.m_Center - b.m_Center).LengthSq();
  float sumRadii = a.m_Radius + b.m_Radius;
  return distSq <= (sumRadii * sumRadii);
}

bool Intersect(const AABB& a, const AABB& b)
{
  // separating axis theorem
  bool noInterSection = a.m_Max.x < b.m_Min.x ||
    a.m_Max.y < b.m_Min.y ||
    a.m_Max.z < b.m_Min.z ||

    b.m_Max.x < a.m_Min.y ||
    b.m_Max.y < a.m_Min.y ||
    b.m_Max.z < a.m_Min.z;
    // if none of these are true, they must intersect
    return !noInterSection;
}

bool Intersect(const Capsule& a, const Capsule& b)
{
  float distSq   = LineSegment::MinDistSq(a.m_Segment, b.m_Segment);
  float sumRadii = a.m_Radius + b.m_Radius;
  return distSq <= (sumRadii * sumRadii);
}

bool Intersect(const Sphere& s, const AABB& box)
{
  float distSq = box.MinDistSq(s.m_Center);
  return distSq <= (s.m_Radius * s.m_Radius);
}

bool Intersect(const LineSegment& l, const Sphere& s, float& outT)
{
  // 3 possibilities:
  // discriminant < 0 : no intersect
  // discriminant = 0 : 1 intersect on edge
  // discriminant > 0 : 2 intersections passing through sphere
  // (false if sphere wholly contains line segment)

  // compute X Y a b c
  Vector3 X = l.m_Start - s.m_Center;
  Vector3 Y = l.m_End - l.m_Start;
  float a = Vector3::Dot(Y, Y);
  float b = 2.0f * Vector3::Dot(X, Y);
  float c = Vector3::Dot(X, X) - s.m_Radius * s.m_Radius;

  // compute discriminant
  float disc = b * b - 4.0f * a * c;
  if (disc < 0.0f)
  {
    return false;
  }
  else
  {
    disc = Math::Sqrt(disc);
    // compute min and max solutions of t
    float tMin = (-b - disc) / (2.0f * a);
    float tMax = (-b + disc) / (2.0f * a);
    // check if t is within bounds of segment
    if (tMin >= 0.0f && tMin <= 1.0f)
    {
      outT = tMin;
      return true;
    }
    else if (tMax >= 0.0f && tMax <= 1.0f)
    {
      outT = tMax;
      return true;
    }
    else
    {
      return false;
    }
  }
}

bool Intersect(const LineSegment& l, const Plane& p, float& outT)
{
  // test if there's a solution for t
  float denom = Vector3::Dot(l.m_End - l.m_Start, p.m_Normal);
  if (Math::NearZero(denom))
  {
    // the only way they intersect if start / end are points on the plane (P dot N) == d
    if(Math::NearZero(Vector3::Dot(l.m_Start, p.m_Normal) - p.m_Dist))
    {
      outT = 0.0f;
      return true;
    } else
    {
      return false;
    }
  } else
  {
    float numer = -Vector3::Dot(l.m_Start, p.m_Normal) - p.m_Dist;
    outT = numer / denom;
    // validate t is within bounds of the line segment
    if(outT >= 0.0f && outT <= 1.0f)
    {
      return true;
    } else
    {
      return false;
    }
  }
}

bool Intersect(const LineSegment& l, const AABB& b, float& outT, Vector3& outNorm)
{
	// Vector to save all possible t values, and normals for those sides
	std::vector<std::pair<float, Vector3>> tValues;
	// Test the x planes
	TestSidePlane(l.m_Start.x, l.m_End.x, b.m_Min.x, Vector3::NegUnitX,
		tValues);
	TestSidePlane(l.m_Start.x, l.m_End.x, b.m_Max.x, Vector3::UnitX,
		tValues);
	// Test the y planes
	TestSidePlane(l.m_Start.y, l.m_End.y, b.m_Min.y, Vector3::NegUnitY,
		tValues);
	TestSidePlane(l.m_Start.y, l.m_End.y, b.m_Max.y, Vector3::UnitY,
		tValues);
	// Test the z planes
	TestSidePlane(l.m_Start.z, l.m_End.z, b.m_Min.z, Vector3::NegUnitZ,
		tValues);
	TestSidePlane(l.m_Start.z, l.m_End.z, b.m_Max.z, Vector3::UnitZ,
		tValues);

	// Sort the t values in ascending order
	std::sort(tValues.begin(), tValues.end(), [](
		const std::pair<float, Vector3>& a,
		const std::pair<float, Vector3>& b) {
		return a.first < b.first;
	});
	// Test if the box contains any of these points of intersection
	Vector3 point;
	for (auto& t : tValues)
	{
		point = l.PointOnSegment(t.first);
		if (b.Contains(point))
		{
			outT = t.first;
			outNorm = t.second;
			return true;
		}
	}

	//None of the intersections are within bounds of box
	return false;
}

// FOR DYNAMIC COLLISIONS
bool SweptSphere(const Sphere& P0, const Sphere& P1, const Sphere& Q0, const Sphere& Q1, float& outT)
{
	// Compute X, Y, a, b, and c
	Vector3 X = P0.m_Center - Q0.m_Center;
	Vector3 Y = P1.m_Center - P0.m_Center -
		(Q1.m_Center - Q0.m_Center);
	float a = Vector3::Dot(Y, Y);
	float b = 2.0f * Vector3::Dot(X, Y);
	float sumRadii = P0.m_Radius + Q0.m_Radius;
	float c = Vector3::Dot(X, X) - sumRadii * sumRadii;
	// Solve discriminant
	float disc = b * b - 4.0f * a * c;
	if (disc < 0.0f)
	{
		return false;
	}
	else
	{
		disc = Math::Sqrt(disc);
		// We only care about the smaller solution
		outT = (-b - disc) / (2.0f * a);
		if (outT >= 0.0f && outT <= 0.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool TestSidePlane(float start, float end, float negd, const Vector3& norm,
	std::vector<std::pair<float, Vector3>>& out)
{
	float denom = end - start;
	if (Math::NearZero(denom))
	{
		return false;
	}
	else
	{
		float numer = -start + negd;
		float t = numer / denom;
		// Test that t is within bounds
		if (t >= 0.0f && t <= 1.0f)
		{
			out.emplace_back(t, norm);
			return true;
		}
		else
		{
			return false;
		}
	}
}
