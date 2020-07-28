#include "PhysWorld.h"
#include "BoxComponent.h"
#include <algorithm>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <set>
#include <utility> // std::pair

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
	for (auto box : m_BoxesX)
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
	for (size_t i = 0; i < m_BoxesX.size(); i++)
	{
		// Don't need to test vs itself and any previous i values
		for (size_t j = i + 1; j < m_BoxesX.size(); j++)
		{
			BoxComponent* a = m_BoxesX[i];
			BoxComponent* b = m_BoxesX[j];
			if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				// Call supplied function to handle intersection
				func(a->GetOwner(), b->GetOwner());
			}
		}
	}
}

struct Pairs
{
	std::set<std::pair<BoxComponent*, BoxComponent*>> pairs;

	void Add(BoxComponent* a, BoxComponent* b)
	{
		// only add if neither permutation is in set
		auto revP = std::make_pair(b, a);
		bool isIn = pairs.find(revP) != pairs.end();
		if (!isIn)
		{
			auto p = std::make_pair(a, b);
			pairs.insert(p); // if p already in, nothing happens
		}
	}

	void Remove(BoxComponent* a, BoxComponent* b)
	{
		// remove pair, in either permutation, if in set
		auto p = std::make_pair(a, b);
		auto revP = std::make_pair(b, a);

		bool isIn     = pairs.find(p) != pairs.end();
		bool isInRev = pairs.find(revP) != pairs.end();

		// delete either permutation
		if (isIn)
		{
			pairs.erase(p);
		}
		if (isInRev)
		{
			pairs.erase(revP);
		}
	}
};

void PhysWorld::TestSweepAndPrune(std::function<void(Actor*, Actor*)> func)
{
	// 3d sweep and prune

	// pairs to check for collision by axis
	Pairs pairs;

	// Sort by min.x
	std::sort(m_BoxesX.begin(), m_BoxesX.end(),
		[](BoxComponent* a, BoxComponent* b) {
			return a->GetWorldBox().m_Min.x <
				b->GetWorldBox().m_Min.x;
	});

	// prune by x axis
	for (size_t i = 0; i < m_BoxesX.size(); i++)
	{
		// Get max.x for current box
		BoxComponent* a = m_BoxesX[i];
		float max = a->GetWorldBox().m_Max.x;
		for (size_t j = i + 1; j < m_BoxesX.size(); j++)
		{
			BoxComponent* b = m_BoxesX[j];
			// If AABB[j] min is past the max bounds of AABB[i],
			// then there aren't any other possible intersections
			// against AABB[i]
			if (b->GetWorldBox().m_Min.x > max)
			{
				break;
			}
			// add to possible pairs
			else
			{
				pairs.Add(a, b);
			}
		}
	}

	// if no pairs in list, return !
	if (pairs.pairs.empty()) { return; }

	// sort by min.y
	std::sort(m_BoxesY.begin(), m_BoxesY.end(),
		[](BoxComponent* a, BoxComponent* b) {
			return a->GetWorldBox().m_Min.y <
				b->GetWorldBox().m_Min.y;
	});

	// prune by y axis
	for (size_t i = 0; i < m_BoxesY.size(); i++)
	{
		// Get max.x for current box
		BoxComponent* a = m_BoxesY[i];
		float max = a->GetWorldBox().m_Max.y;
		for (size_t j = i + 1; j < m_BoxesY.size(); j++)
		{
			BoxComponent* b = m_BoxesY[j];
			// If AABB[j] min is past the max bounds of AABB[i],
			// then there aren't any other possible intersections
			// against AABB[i]
			if (b->GetWorldBox().m_Min.y > max)
			{
				pairs.Remove(a, b);
				break;
			}
			// check for collision ( z axis) -> store pairs (x, y axis )
			else
			{
				pairs.Add(a, b);
			}
		}
	}

	// if no pairs, return !
	if (pairs.pairs.empty()) { return; }

	// sort by min.z
	std::sort(m_BoxesZ.begin(), m_BoxesZ.end(),
		[](BoxComponent* a, BoxComponent* b) {
			return a->GetWorldBox().m_Min.z <
				b->GetWorldBox().m_Min.z;
	});

	// prune by z axis and test intersections for remaining pairs
	for (size_t i = 0; i < m_BoxesZ.size(); i++)
	{
		// Get max.x for current box
		BoxComponent* a = m_BoxesZ[i];
		float max = a->GetWorldBox().m_Max.z;
		for (size_t j = i + 1; j < m_BoxesZ.size(); j++)
		{
			BoxComponent* b = m_BoxesZ[j];
			// If AABB[j] min is past the max bounds of AABB[i],
			// then there aren't any other possible intersections
			// against AABB[i]
			if (b->GetWorldBox().m_Min.z > max)
			{
				// remove from pair list (y & z axis)
				pairs.Remove(a, b);
				break;
			}
			// check for collision ( z axis) -> store pairs (x, y axis )
			else
			{
				pairs.Add(a, b);
			}
		}
	}

	// loop over pairs and check for collisions
	auto it = pairs.pairs.begin();
	while (it != pairs.pairs.end())
	{
		it++;
		auto pair = (*it);
		auto a = pair.first;
		auto b = pair.second;
		if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
		{
			func(a->GetOwner(), b->GetOwner());
		}
	}
}

void PhysWorld::AddBox(BoxComponent* box)
{
	m_BoxesX.emplace_back(box);
	m_BoxesY.emplace_back(box);
	m_BoxesZ.emplace_back(box);
}

void PhysWorld::RemoveBox(BoxComponent* box)
{
	auto iter = std::find(m_BoxesX.begin(), m_BoxesX.end(), box);
	if (iter != m_BoxesX.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, m_BoxesX.end() - 1);
		m_BoxesX.pop_back();
	}

	iter = std::find(m_BoxesY.begin(), m_BoxesY.end(), box);
	if (iter != m_BoxesY.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, m_BoxesY.end() - 1);
		m_BoxesY.pop_back();
	}

	iter = std::find(m_BoxesZ.begin(), m_BoxesZ.end(), box);
	if (iter != m_BoxesZ.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, m_BoxesZ.end() - 1);
		m_BoxesZ.pop_back();
	}

}
