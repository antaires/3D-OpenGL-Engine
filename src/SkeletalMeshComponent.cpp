#include "SkeletalMeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Skeleton.h"
#include "Animation.h"

SkeletalMeshComponent::SkeletalMeshComponent(class Actor* owner)
  :MeshComponent(owner, true)
{}

void SkeletalMeshComponent::Draw(class Shader* shader)
{
  if (m_Mesh)
  {
    // Set the world transform
    shader->SetMatrixUniform("uWorldTransform", m_Owner->GetWorldTransform());

    // Set the matrix palette
    shader->SetMatrixUniforms("uMatrixPalette", &m_Palette.m_Entry[0], MAX_SKELETON_BONES);

    // Set specular power
    shader->SetFloatUniform("uSpecPower", m_Mesh->GetSpecPower());
    // Set the active texture
    Texture* t = m_Mesh->GetTexture(m_TextureIndex);
    if (t)
    {
      t->SetActive();
    }
    // Set the mesh's vertex array as active
    VertexArray* va = m_Mesh->GetVertexArray();
    va->SetActive();
    // Draw
    glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
  }
}

void SkeletalMeshComponent::ComputeMatrixPalette()
{
  const std::vector<Matrix4>& globalInvBindPoses = m_Skeleton->GetGlobalInvBindPoses();
  std::vector<Matrix4> currentPoses;
  m_Animation->GetGlobalPoseAtTime(currentPoses, m_Skeleton, m_AnimTime);

  // setup the palette for each bone
  for(size_t i = 0; i < m_Skeleton->GetNumBones(); ++i)
  {
    // global inverse bind pose matrix times current pose matrix
    m_Palette.m_Entry[i] = globalInvBindPoses[i] * currentPoses[i];
  }
}

float SkeletalMeshComponent::PlayAnimation(const Animation* anim, float playRate)
{
  m_Animation = anim;
  m_AnimTime = 0.0f;
  m_AnimPlayRate = playRate;

  if (!m_Animation) { return 0.0f; }
  ComputeMatrixPalette();

  return m_Animation->GetDuration();
}
