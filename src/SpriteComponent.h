#pragma once

#include <SDL2/SDL.h>
#include "Component.h"

class SpriteComponent: public Component {
 private:
 protected:
   class Texture* m_Texture;
   int m_DrawOrder;
   int m_TextureWidth;
   int m_TextureHeight;
 public:
   SpriteComponent(class Actor* owner, int drawOrder = 100);
   ~SpriteComponent();
   virtual void Draw(class Shader* shader);
   virtual void SetTexture(class Texture* texture);

   int GetDrawOrder() const;
   int GetTextureHeight() const;
   int GetTextureWidth() const;
 };
