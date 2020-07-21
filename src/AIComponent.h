#pragma once

#include "Component.h"
#include "Map.h"
#include <unordered_map>
#include <string>

class AIComponent: public Component
{
private:
  // maps name of state to AIState instance
  std::unordered_map<std::string, class AIState*> m_StateMap;
  class AIState* m_CurrentState;
protected:
  class Map* m_Map;
public:
  AIComponent(class Actor* owner, class Map* map);

  void Update(float deltaTime) override;
  void ChangeState(const std::string& name);

  // add new state to the map
  void RegisterState(class AIState* state);

  Map* GetMap() const;
};
