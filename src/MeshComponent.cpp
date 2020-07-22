#include "MeshComponent.h"
#include "Actor.h"
#include "Shader.h"
#include "Mesh.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"

MeshComponent::MeshComponent(class Actor* owner)
  :Component(owner)
  , m_Mesh(nullptr)
  , m_TextureIndex(0)
{
  m_Owner->GetGame()->GetRenderer()->AddMeshComp(this);
}

MeshComponent::~MeshComponent()
{
  m_Owner->GetGame()->GetRenderer()->RemoveMeshComp(this);
}

// draw this mesh with provided shader
void MeshComponent::Draw(class Shader* shader)
{
  if(m_Mesh)
  {
    // set the world transform
    shader->SetMatrixUniform("uWorldTransform", m_Owner->GetWorldTransform());

    // set specular power
    // TODO: uncomment 
    // shader->SetFloatUniform("uSpecPower", m_Mesh->GetSpecPower());
    // set the active texture
    Texture* tex = m_Mesh->GetTexture(m_TextureIndex);
    if (tex)
    {
      tex->SetActive();
    }
    // set the mesh's vertex array as active
    VertexArray* va = m_Mesh->GetVertexArray();
    va->SetActive();
    // draw
    glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
  }
}

// set the mesh/ texture index used by the mesh comp
void MeshComponent::SetMesh(class Mesh* mesh)
{
  m_Mesh = mesh;
}

void MeshComponent::SetTextureIndex(size_t index)
{
  m_TextureIndex = index;
}
