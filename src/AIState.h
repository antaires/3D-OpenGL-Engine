#pragma once

class AIState {
private:
protected:
  class AIComponent* m_Owner;
public:
  AIState(class AIComponent* owner);

  // state specific behaviour
  virtual void Update(float deltaTime) = 0;
  virtual void OnEnter() = 0;
  virtual void OnExit() = 0;

  // getter for string name of state
  virtual const char* GetName() const = 0;
};
