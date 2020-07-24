#include "InputSystem.h"

// KEYBOARD STATE
bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
  return m_CurrentState[keyCode];
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
{
  if (m_PrevState[keyCode] == 0)
  {
    if (m_CurrentState[keyCode] == 0)
    {
      return E_None;
    } else
    {
      return E_Pressed;
    }
  } else  // prev state must be 1
  {
    if (m_CurrentState[keyCode] == 0)
    {
      return E_Released;
    } else
    {
      return E_Held;
    }
  }
}

// INPUT SYSTEM
bool InputSystem::Initialize()
{
  // assign current state to pointer
  m_State.keyboard.m_CurrentState = SDL_GetKeyboardState(NULL);

  // clear prev state memory
  memset(m_State.keyboard.m_PrevState, 0, SDL_NUM_SCANCODES);

  // hide cursor
  SDL_ShowCursor(SDL_FALSE);
}

void InputSystem::ShutDown()
{}

// called right before SDL_PollEvents loop
void InputSystem::PrepareForUpdate()
{
  memcpy(m_State.keyboard.m_PrevState, m_State.keyboard.m_CurrentState, SDL_NUM_SCANCODES);
}

// called right after SDL_PollEvents loop
void InputSystem::Update()
{}

const InputState& InputSystem::GetState() const
{
  return m_State;
}
