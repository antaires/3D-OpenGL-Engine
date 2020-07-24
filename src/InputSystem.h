#pragma once

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

  const Uint8* m_CurrentState;
  Uint8 m_PrevState[SDL_NUM_SCANCODES];
};

// wrapper that contains current state of input
struct InputState
{
  KeyboardState keyboard;
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

  const InputState& GetState() const;
private:
  InputState m_State;
};
