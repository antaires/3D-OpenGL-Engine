#include "Component.h"
#include "Actor.h"

Component::Component(class Actor* owner, int updateOrder)
  : m_Owner(owner)
  , m_UpdateOrder(updateOrder)
{
  // add to actor's vector of components
  m_Owner->AddComponent(this);
}

Component::~Component()
{
  m_Owner->RemoveComponent(this);
}

void Component::Update(float deltaTime)
{}

void Component::ProcessInput(const InputState& state)
{
  /* example keyboard input
  ButtonState buttonState = state.keyboard.GetKeyState(SDL_SCANCODE_W);
  if (buttonState == E_Pressed || buttonState == E_Held)
  {...}

    example mouse input
    // left : SDL_BUTTON_LEFT
    // right: SDL_BUTTON_RIGHT
    // middle : SDL_BUTTON_MIDDLE
    // mouse button 4 : SDL_BUTTON_X1
    // mouse button 5 : SDL_BUTTON_X2

    if (state.mouseState.GetButtonState(SDL_BUTTON_LEFT) == E_Pressed)
    {...}
  */
}

void Component::OnUpdateWorldTransform(){}

int Component::GetUpdateOrder() const {return m_UpdateOrder;}
