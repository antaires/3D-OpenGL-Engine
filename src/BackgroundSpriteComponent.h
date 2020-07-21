#pragma once

#include "SpriteComponent.h"

class BackgroundSpriteComponent : public SpriteComponent
{
private:
  struct BGTexture
  {
    class Texture* m_Texture;
    Vector2 m_Offset;
  };
  std::vector<BGTexture> m_BGTextures;
  Vector2 m_ScreenSize;
  float m_Scrollspeed;
public:
  BackgroundSpriteComponent(class Actor* owner, int drawOrder = 10);
  void Update(float deltaTime) override;
  void Draw(class Shader* shader) override;
  void SetBGTextures(const std::vector<class Texture*>& textures);
  void SetScreenSize(const Vector2& size);
  void SetScrollSpeed(float speed);
  float GetScrollSpeed() const;
};
