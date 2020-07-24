#pragma once

#include <SDL2/SDL.h>

enum ButtonState
{
  E_None
  , E_Pressed
  , E_Released
  , E_Held
};

struct KeyboardState
{
  ButtonState m_ButtonState;
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
