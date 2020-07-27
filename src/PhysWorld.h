#pragma once

#include "Math.h"
#include "Collision.h"
#include <vector>
#include <functional>

class PhysWorld
{
public:
  PhysWorld(class Game* game);

  // Used to give helpful information about collision results
	struct CollisionInfo
	{
		// Point of collision
		Vector3 m_Point;
		// Normal at collision
		Vector3 m_Normal;
		// Component collided with
		class BoxComponent* m_Box;
		// Owning actor of componnet
		class Actor* m_Actor;
	};

	// Test a line segment against boxes
	// Returns true if it collides against a box
	bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

	// Tests collisions using naive pairwise
	void TestPairwise(std::function<void(class Actor*, class Actor*)> f);
	// Test collisions using sweep and prune
	void TestSweepAndPrune(std::function<void(class Actor*, class Actor*)> f);

  // add / remove box components from the world
  void AddBox(class BoxComponent* box);
  void RemoveBox(class BoxComponent* box);
private:
  class Game* m_Game;
  std::vector<class BoxComponent*> m_Boxes;
};
