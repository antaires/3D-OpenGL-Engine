#pragma once

#include "Math.h"

class CollisionDetection {
public:
  static bool HasCollision(class CircleComponent* a, class CircleComponent* b);
  static bool HasCollision(class RectangleComponent* a, class RectangleComponent* b);
  static bool HasCollision(Vector2 posA, float widthA, float heightA, Vector2 posB, float widthB, float heightB);

  // TODO make HasCollision(a, b) for every combination of colliders
};
