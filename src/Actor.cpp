#include "Actor.h"
#include "Game.h"

Actor::Actor(class Game* game)
  : m_State(E_Active)
  , m_Position(Vector2(0.0f, 0.0f))
  , m_Scale(1.0f)
  , m_Rotation(0.0f)
  , m_RecomputeWorldTransform(true)
  , m_PrevPosition(Vector2(0.0f, 0.0f))
  , m_TopCornerPosition(Vector2(0.0f, 0.0f))
  , m_Velocity(Vector2(0.0f, 0.0f))
  , m_ForwardVector(Vector2(0.0f, 0.0f))
  , m_Width(0)
  , m_Height(0)
  , m_Game(game)
  , m_DyingStarted(false)
  , m_HasCollision(false)
  , m_GoalPosition(Vector2(0.0f, 0.0f))
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

  if (m_State == E_Dying)
  {
    HandleDeath();
    UpdateComponents(deltaTime);
  }
}

void Actor::UpdateComponents(float deltaTime)
{
    m_PrevPosition = m_Position;

    for (auto comp : m_Components)
    {
      comp->Update(deltaTime);
    }

    // update top corner position TODO: still used?
    m_TopCornerPosition.x = m_Position.x - (m_Width/2);
    m_TopCornerPosition.y = m_Position.y - (m_Height/2);

}

void Actor::UpdateActor(float deltaTime){}

void Actor::ProcessInput(const uint8_t* keyState)
{
  // called in Game not overridable
  if (m_State == E_Active)
  {
    for(auto comp: m_Components)
    {
      comp->ProcessInput(keyState);
    }
    ActorInput(keyState);
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

void Actor::ActorInput(const uint8_t* keyState)
{}

Vector2 Actor::GetForward() const
{
  return Vector2(Math::Cos(m_Rotation), Math::Sin(m_Rotation));
}

void Actor::ComputeWorldTransform()
{
  if(m_RecomputeWorldTransform)
  {
    m_RecomputeWorldTransform = false;
    // scale, rotate then translate
    m_WorldTransform = Matrix4::CreateScale(m_Scale);
    m_WorldTransform *= Matrix4::CreateRotationZ(m_Rotation);
    m_WorldTransform *= Matrix4::CreateTranslation(Vector3(m_Position.x, m_Position.y, 0.0f));

    // inform components world transform updated
    for(auto comp : m_Components)
    {
      comp->OnUpdateWorldTransform();
    }
  }
}


Actor::State Actor::GetState() const { return m_State;}

void Actor::SetState(State state) { m_State = state;}

Vector2 Actor::GetPosition() const { return m_Position;}

void Actor::SetPosition(const Vector2& pos)
{
  m_Position = pos;
  m_RecomputeWorldTransform = true;
}

Vector2 Actor::GetPrevPosition() const { return m_PrevPosition; }

float Actor::GetRotation() const { return m_Rotation;}

void Actor::SetRotation(float rotation)
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

Vector2 Actor::GetVelocity() const { return m_Velocity; }

void Actor::SetVelocity(Vector2 velocity) { m_Velocity = velocity; }

Vector2 Actor::GetForwardVector() const { return m_ForwardVector; }

void Actor::SetForwardVector(Vector2 forwardVector) { m_ForwardVector = forwardVector; }

float Actor::GetHeight() const { return m_Height;}

float Actor::GetWidth() const { return m_Width; }

void Actor::SetHeight(float height) { m_Height = height;}

void Actor::SetWidth(float width) { m_Width = width; }

void Actor::SetHasCollision(bool hasCollision) { m_HasCollision = hasCollision; }

Vector2 Actor::GetGoal() const { return m_GoalPosition; }

void Actor::SetGoal(Vector2 goal) { m_GoalPosition = goal; }

const Vector2& Actor::GetTopCornerPosition() const { return m_TopCornerPosition; }

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

bool Actor::CollidesWithBarrier(Vector2 position, float width, float height) const
{
  return m_Game->CollidesWithBarrier(position, width, height);
}

void Actor::HandleDeath(){}
