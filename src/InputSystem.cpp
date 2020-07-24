#include "InputSystem.h"

bool InputSystem::Initialize()
{}

void InputSystem::ShutDown()
{}

// called right before SDL_PollEvents loop
void InputSystem::PrepareForUpdate()
{}

// called right after SDL_PollEvents loop
void InputSystem::Update()
{}

const InputState& InputSystem::GetState() const
{
  return m_State;
}
