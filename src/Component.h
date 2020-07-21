#pragma once

#include "Actor.h"

class Component {
protected:
  class Actor* m_Owner;
  int m_UpdateOrder;
public:
  // lower update order -> update earlier
  Component(class Actor* owner, int updateOrder = 100);
  virtual ~Component();
  virtual void Update(float deltaTime);
  virtual void ProcessInput(const uint8_t* keyState);
  virtual void ProcessMouse(const uint32_t mouseState, const int x, const int y);
  virtual void OnUpdateWorldTransform();
  int GetUpdateOrder() const;

  class Actor* GetOwner(); // not const because AI states may make changes
};
