#pragma once

#include "AIState.h"
#include "Math.h"

#include <vector>

class AIPatrol: public AIState
{
private:
  std::vector<Vector2> m_Path;
public:
  AIPatrol(class AIComponent* owner);

  // override with behaviours for this state
  void Update(float deltaTime) override;
  void OnEnter() override;
  void OnExit() override;

  const char* GetName() const override;
};
