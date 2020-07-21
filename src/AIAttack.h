#pragma once

#include "AIState.h"
#include "Math.h"

#include <vector>

class AIAttack: public AIState
{
private:
  int m_ShotTimer;
public:
  AIAttack(class AIComponent* owner);

  // override with behaviours for this state
  void Update(float deltaTime) override;
  void OnEnter() override;
  void OnExit() override;

  const char* GetName() const override;
};
