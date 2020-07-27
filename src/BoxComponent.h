#pragma once

#include "Component.h"
#include "Collision.h"

class BoxComponent : public Component
{
public:
  BoxComponent(class Actor* owner, int updateOrder = 100);
  ~BoxComponent();
  void OnUpdateWorldTransform() override;
  void SetObjectBox(const AABB& model);
  const AABB& GetWorldBox() const;
  void SetShouldRotate(bool value);
private:
  AABB m_ObjectBox;
  AABB m_WorldBox;
  bool m_ShouldRotate;
};
