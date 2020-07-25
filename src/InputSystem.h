#pragma once

#include "Math.h"
#include <SDL2/SDL.h>

enum ButtonState
{
  E_None
  , E_Pressed
  , E_Released
  , E_Held
};

class KeyboardState
{
public:
  friend class InputSystem;
  bool GetKeyValue(SDL_Scancode keyCode) const;
  ButtonState GetKeyState(SDL_Scancode keyCode) const;

  const Uint8* m_CurrState;
  Uint8 m_PrevState[SDL_NUM_SCANCODES];
};

class MouseState
{
public:
  friend class InputSystem;

  // for mouse position
  const Vector2& GetPosition() const;
  bool GetButtonValue(int button) const;
  ButtonState GetButtonState(int button) const;

  Vector2 m_MousePosition;
private:
  // store button data
  Uint32 m_CurrButtons;
  Uint32 m_PrevButtons;

  Vector2 m_ScrollWheel;

  bool m_IsRelative;
};

// wrapper that contains current state of input
struct InputState
{
  KeyboardState keyboard;
  MouseState mouseState;
};

class InputSystem
{
public:
  bool Initialize();
  void ShutDown();

  // called right before SDL_PollEvents loop
  void PrepareForUpdate();
  // called right after SDL_PollEvents loop
  void Update();
  void ProcessEvent(SDL_Event& event);
  void SetRelativeMouseMode(bool value);

  const InputState& GetState() const;
private:
  InputState m_State;
};
