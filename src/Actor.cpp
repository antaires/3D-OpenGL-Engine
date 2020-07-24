#include "Actor.h"
#include "Game.h"
#include "Math.h"
#include "Component.h"

Actor::Actor(class Game* game)
  : m_State(E_Active)
  , m_Position(Vector3::Zero)
  , m_Rotation(Quaternion::Identity)
  , m_Scale(1.0f)
  , m_RecomputeWorldTransform(true)
  , m_Game(game)
{
  m_Game->AddActor(this);
}

Actor::~Actor()
{
  m_Game->RemoveActor(this);

  while(!m_Components.empty())
  {
    delete m_Components.back();
  }
}

void Actor::Update(float deltaTime)
{
  if(m_State == E_Active)
  {
    ComputeWorldTransform();

    UpdateComponents(deltaTime);
    UpdateActor(deltaTime);

    ComputeWorldTransform();
  }
}

void Actor::UpdateComponents(float deltaTime)
{
    for (auto comp : m_Components)
    {
      comp->Update(deltaTime);
    }
}

void Actor::UpdateActor(float deltaTime){}

void Actor::ProcessInput(const InputState& state)
{
  // called in Game not overridable
  if (m_State == E_Active)
  {
    for(auto comp: m_Components)
    {
      comp->ProcessInput(state);
    }
    ActorInput(state);
  }
}

void Actor::ProcessMouse(const uint32_t mouseState, const int x, const int y)
{
  // called in game not overridable
  if (m_State == E_Active)
  {
    for(auto comp: m_Components)
    {
      comp->ProcessMouse(mouseState, x, y);
    }
  }
}

void Actor::ActorInput(const InputState& state)
{}

Vector3 Actor::GetForward() const
{
  return Vector3::Transform(Vector3::UnitX, m_Rotation);
}

void Actor::ComputeWorldTransform()
{
  if(m_RecomputeWorldTransform)
  {
    m_RecomputeWorldTransform = false;
    // scale, rotate then translate
    m_WorldTransform = Matrix4::CreateScale(m_Scale);
    m_WorldTransform *= Matrix4::CreateFromQuaternion(m_Rotation);
    m_WorldTransform *= Matrix4::CreateTranslation(m_Position);

    // inform components world transform updated
    for(auto comp : m_Components)
    {
      comp->OnUpdateWorldTransform();
    }
  }
}


Actor::State Actor::GetState() const { return m_State;}

void Actor::SetState(State state) { m_State = state;}

const Vector3& Actor::GetPosition() const { return m_Position;}

void Actor::SetPosition(const Vector3& pos)
{
  m_Position = pos;
  m_RecomputeWorldTransform = true;
}

const Quaternion& Actor::GetRotation() const { return m_Rotation;}

void Actor::SetRotation(const Quaternion& rotation)
{
  m_Rotation = rotation;
  m_RecomputeWorldTransform = true;
}

float Actor::GetScale() const { return m_Scale; }

void Actor::SetScale(float scale)
{
  m_Scale = scale;
  m_RecomputeWorldTransform = true;
}

class Game* Actor::GetGame(){return m_Game;}

Matrix4 Actor::GetWorldTransform() const
{
  return m_WorldTransform;
}

void Actor::AddComponent(class Component* component)
{
  // add components in sorted order
  int myOrder = component->GetUpdateOrder();
  auto it = m_Components.begin();
  for(; it != m_Components.end(); ++it)
  {
    if(myOrder < (*it)->GetUpdateOrder())
    {
      break;
    }
  }

  m_Components.insert(it, component);
}

void Actor::RemoveComponent(class Component* component)
{
  // TODO just set to nullptr and remove later?
  auto it = find(m_Components.begin(), m_Components.end(), component);
  if (it != m_Components.end())
  {
    m_Components.erase(it);
  }
}

void Actor::ClampToScreen(float& pos, int objSize, float lowerLimit, float upperLimit)
{
  if (pos < lowerLimit + (objSize/2.0f))
  {
    pos = lowerLimit + (objSize/2.0f);
  }
  if (pos > upperLimit - (objSize / 2.0f))
  {
    pos = upperLimit - (objSize/2.0f);
  }
}
