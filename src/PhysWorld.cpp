#include "PhysWorld.h"
#include "BoxComponent.h"
#include <algorithm>
#include <SDL2/SDL.h>

PhysWorld::PhysWorld(Game* game)
	:m_Game(game)
{
}

bool PhysWorld::SegmentCast(const LineSegment& l, CollisionInfo& outColl)
{
	bool collided = false;
	// Initialize closestT to infinity, so first
	// intersection will always update closestT
	float closestT = Math::Infinity;
	Vector3 norm;
	// Test against all boxes
	for (auto box : m_Boxes)
	{
		float t;
		// Does the segment intersect with the box?
		if (Intersect(l, box->GetWorldBox(), t, norm))
		{
			// Is this closer than previous intersection?
			if (t < closestT)
			{
				outColl.m_Point = l.PointOnSegment(t);
				outColl.m_Normal = norm;
				outColl.m_Box = box;
				outColl.m_Actor = box->GetOwner();
				collided = true;
			}
		}
	}
	return collided;
}

void PhysWorld::TestPairwise(std::function<void(Actor*, Actor*)> func)
{
	// Naive implementation O(n^2)
	for (size_t i = 0; i < m_Boxes.size(); i++)
	{
		// Don't need to test vs itself and any previous i values
		for (size_t j = i + 1; j < m_Boxes.size(); j++)
		{
			BoxComponent* a = m_Boxes[i];
			BoxComponent* b = m_Boxes[j];
			if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				// Call supplied function to handle intersection
				func(a->GetOwner(), b->GetOwner());
			}
		}
	}
}

void PhysWorld::TestSweepAndPrune(std::function<void(Actor*, Actor*)> func)
{
	// Sort by min.x
	std::sort(m_Boxes.begin(), m_Boxes.end(),
		[](BoxComponent* a, BoxComponent* b) {
			return a->GetWorldBox().m_Min.x <
				b->GetWorldBox().m_Min.x;
	});

	for (size_t i = 0; i < m_Boxes.size(); i++)
	{
		// Get max.x for current box
		BoxComponent* a = m_Boxes[i];
		float max = a->GetWorldBox().m_Max.x;
		for (size_t j = i + 1; j < m_Boxes.size(); j++)
		{
			BoxComponent* b = m_Boxes[j];
			// If AABB[j] min is past the max bounds of AABB[i],
			// then there aren't any other possible intersections
			// against AABB[i]
			if (b->GetWorldBox().m_Min.x > max)
			{
				break;
			}
			else if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				func(a->GetOwner(), b->GetOwner());
			}
		}
	}
}

void PhysWorld::AddBox(BoxComponent* box)
{
	m_Boxes.emplace_back(box);
}

void PhysWorld::RemoveBox(BoxComponent* box)
{
	auto iter = std::find(m_Boxes.begin(), m_Boxes.end(), box);
	if (iter != m_Boxes.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, m_Boxes.end() - 1);
		m_Boxes.pop_back();
	}
}
