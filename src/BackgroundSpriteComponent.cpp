#include "BackgroundSpriteComponent.h"


BackgroundSpriteComponent::BackgroundSpriteComponent(class Actor* owner, int drawOrder)
  :SpriteComponent(owner, drawOrder)
  , m_Scrollspeed(0.0f)
{}

void BackgroundSpriteComponent::Update(float deltaTime)
{
  SpriteComponent::Update(deltaTime);
  for (auto& bg : m_BGTextures)
  {
    bg.m_Offset.x += m_Scrollspeed * deltaTime;
    // if bg completely offscreen, reset m_Offset
    if(bg.m_Offset.x < -m_ScreenSize.x)
    {
      bg.m_Offset.x = (m_BGTextures.size() - 1) * m_ScreenSize.x - 1;
    }
  }
}

void BackgroundSpriteComponent::Draw(Shader* shader)
{
  // TODO : use shader
  /*
  // draw each background texture
  for(auto& bg : m_BGTextures)
  {
    SDL_Rect r;
    r.w = static_cast<int>(m_ScreenSize.x);
    r.h = static_cast<int>(m_ScreenSize.y);
    // center rect around position of othe owner
    r.x = static_cast<int>(m_Owner->GetPosition().x - r.w / 2 + bg.m_Offset.x);
    r.y = static_cast<int>(m_Owner->GetPosition().y - r.h / 2 + bg.m_Offset.y);

    // draw this background
    SDL_RenderCopy(renderer, bg.m_Texture, nullptr, &r);
  }
  */
}

void BackgroundSpriteComponent::SetBGTextures(const std::vector<Texture*>& textures)
{
  int count = 0;
  for(auto tex : textures)
  {
    BGTexture temp;
    temp.m_Texture = tex;
    // each texture is screen width in offset
    temp.m_Offset.x = count * m_ScreenSize.x;
    temp.m_Offset.y = 0;
    m_BGTextures.emplace_back(temp);
    count++;
  }
}

void BackgroundSpriteComponent::SetScreenSize(const Vector2& size)
{
  m_ScreenSize = size;
}

void BackgroundSpriteComponent::SetScrollSpeed(float speed)
{
  m_Scrollspeed = speed;
}

float BackgroundSpriteComponent::GetScrollSpeed() const
{
  return m_Scrollspeed;
}
