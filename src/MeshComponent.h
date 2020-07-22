#pragma once

#include "Component.h"
#include <cstddef>

class MeshComponent : public Component
{
public:
  MeshComponent(class Actor* owner);
  ~MeshComponent();

  // draw this mesh with provided shader
  virtual void Draw(class Shader* shader);

  // set the mesh/ texture index used by the mesh comp
  virtual void SetMesh(class Mesh* mesh);
  void SetTextureIndex(size_t index);

protected:
  class Mesh* m_Mesh;
  size_t m_TextureIndex;
};
