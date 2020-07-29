#include "BallActor.h"
#include "Game.h"
#include "MeshComponent.h"
#include "BallMoveComp.h"
#include "Mesh.h"
#include "Renderer.h"

BallActor::BallActor(Game* game)
  :Actor(game)
  , m_MyMove(nullptr)
  , m_LifeSpan(20.0f)
{
  // SetScale(10.0f);
  MeshComponent* mc = new MeshComponent(this);
  Mesh* mesh = GetGame()->GetRenderer()->GetMesh("assets/Sphere.gpmesh");
  mc->SetMesh(mesh);
  m_MyMove = new BallMoveComp(this);
  m_MyMove->SetForwardSpeed(1500.0f);
}

void BallActor::UpdateActor(float deltaTime)
{
  Actor::UpdateActor(deltaTime);

  m_LifeSpan -= deltaTime;
  if(m_LifeSpan < 0.0f)
  {
    SetState(E_Dead);
  }
}

void BallActor::SetPlayer(Actor* player)
{
  m_MyMove->SetPlayer(player);
}

void BallActor::HitTarget()
{
  // play audio / explosion
}
