#pragma once

#include "Actor.h"

class Player: public Actor
{
private:
  class AnimSpriteComponent* m_AnimSpriteComponent;
  class CircleComponent* m_Circle; // TODO Remove
  class RectangleComponent* m_Rectangle;
  class InputComponent* m_InputComponent;
public:
  Player(class Game* game);
  void UpdateActor(float deltaTime) override;
  void ProcessKeyboard(const uint8_t* state);
  class CircleComponent* GetCircle() const;
  class RectangleComponent* GetRectangle() const;

  void HandleDeath() override;
};

// TODO add global func to access player actor pointer
// for enemy AI etc
