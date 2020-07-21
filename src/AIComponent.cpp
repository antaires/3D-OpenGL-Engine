#include "AIComponent.h"
#include "AIState.h"
#include "SDL2/SDL.h" // just for SDL_Log

AIComponent::AIComponent(class Actor* owner, class Map* map)
  :Component(owner)
  , m_CurrentState(nullptr)
  , m_Map(map)
{}

void AIComponent::Update(float deltaTime)
{
  if(m_CurrentState)
  {
    m_CurrentState->Update(deltaTime);
  }
}

void AIComponent::ChangeState(const std::string& name)
{
  // calls OnExit to current state, then tries to find
  // next state in the map. if it finds the next state, it
  // changes current state to new state and calls OnEnter
  // if no next state found, error message
  if(m_CurrentState)
  {
    m_CurrentState->OnExit();
  }

  // find next state
  auto it = m_StateMap.find(name);
  if (it != m_StateMap.end())
  {
    m_CurrentState = it->second;
    m_CurrentState->OnEnter();
  } else
  {
    SDL_Log("Could not find AI State %s in state map", name.c_str());
    m_CurrentState = nullptr;
  }

}

// add new state to the map
void AIComponent::RegisterState(AIState* state)
{
  m_StateMap.emplace(state->GetName(), state);
}

Map* AIComponent::GetMap() const
{
  return m_Map;
}
