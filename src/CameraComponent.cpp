#include "CameraComponent.h"
#include "Actor.h"
#include "Renderer.h"
#include "Game.h"

CameraComponent::CameraComponent(class Actor* owner, int updateOrder)
  :Component(owner, updateOrder)
{}

void CameraComponent::SetViewMatrix(const Matrix4& view)
{
  // pass view matrix to renderer
  Game* game = m_Owner->GetGame();
  game->GetRenderer()->SetViewMatrix(view);
}
