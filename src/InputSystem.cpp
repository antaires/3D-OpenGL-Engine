#include "InputSystem.h"
#include "Constants.h"

// KEYBOARD STATE
bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
  return m_CurrState[keyCode];
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
{
  if (m_PrevState[keyCode] == 0)
  {
    if (m_CurrState[keyCode] == 0)
    {
      return E_None;
    } else
    {
      return E_Pressed;
    }
  } else  // prev state must be 1
  {
    if (m_CurrState[keyCode] == 0)
    {
      return E_Released;
    } else
    {
      return E_Held;
    }
  }
}

// MOUSE STATE
const Vector2& MouseState::GetPosition() const
{
  return m_MousePosition;
}

bool MouseState::GetButtonValue(int button) const
{
  return (m_CurrButtons & SDL_BUTTON(button) == 1);

  /* button key:
  // left : SDL_BUTTON_LEFT
  // right: SDL_BUTTON_RIGHT
  // middle : SDL_BUTTON_MIDDLE
  // mouse button 4 : SDL_BUTTON_X1
  // mouse button 5 : SDL_BUTTON_X2
  */
}

ButtonState MouseState::GetButtonState(int button) const
{
  int mask = SDL_BUTTON(button);
  if ((mask & m_PrevButtons) == 0)
  {
    if((mask & m_CurrButtons) == 0)
    {
      return E_None;
    } else
    {
      return E_Pressed;
    }
  } else
  {
    if((mask & m_CurrButtons) == 0)
    {
      return E_Released;
    }
    else
    {
      return E_Held;
    }
  }
}

// INPUT SYSTEM
bool InputSystem::Initialize()
{
  // set up keyboard
  // assign curr state to pointer
  m_State.keyboard.m_CurrState = SDL_GetKeyboardState(NULL);
  // clear prev state memory
  memset(m_State.keyboard.m_PrevState, 0, SDL_NUM_SCANCODES);

  // hide cursor
  SDL_ShowCursor(SDL_FALSE);

  m_State.mouseState.m_IsRelative = false;
}

void InputSystem::ShutDown()
{}

// called right before SDL_PollEvents loop
void InputSystem::PrepareForUpdate()
{
  memcpy(m_State.keyboard.m_PrevState, m_State.keyboard.m_CurrState, SDL_NUM_SCANCODES);

  m_State.mouseState.m_PrevButtons = m_State.mouseState.m_CurrButtons;

  // reset scroll wheel
  m_State.mouseState.m_ScrollWheel = Vector2::Zero;
}

// called right after SDL_PollEvents loop
void InputSystem::Update()
{
  int x = 0, y = 0;
  if(m_State.mouseState.m_IsRelative)
  {
    m_State.mouseState.m_CurrButtons = SDL_GetRelativeMouseState(&x, &y);
  } else
  {
    m_State.mouseState.m_CurrButtons = SDL_GetMouseState(&x, &y);
    // convert from SDL to OpenGL coords.
    x = x - SCREEN_WIDTH / 2;
    y = SCREEN_HEIGHT / 2 - y;
  }

  m_State.mouseState.m_MousePosition.x = static_cast<float>(x);
  m_State.mouseState.m_MousePosition.y = static_cast<float>(y);
}

void InputSystem::ProcessEvent(SDL_Event& event)
{
  switch(event.type)
  {
    case SDL_MOUSEWHEEL:
      m_State.mouseState.m_ScrollWheel = Vector2(
        static_cast<float>(event.wheel.x)
        , static_cast<float>(event.wheel.y)
      );
      break;
    default:
      break;
  }
}

const InputState& InputSystem::GetState() const
{
  return m_State;
}

void InputSystem::SetRelativeMouseMode(bool value)
{
  // turn on/off relative mouse
  SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
  SDL_SetRelativeMouseMode(set);

  m_State.mouseState.m_IsRelative = value;
}
