#include "AnimSpriteComponent.h"
#include "Math.h"

AnimSpriteComponent::AnimSpriteComponent(class Actor* owner, int drawOrder)
  :SpriteComponent(owner, drawOrder)
  , m_CurrentFrame(0.0f)
  , m_AnimFPS(24.0f)
{
  m_CurrentAnimationClip = "1";
  m_PreviousAnimationClip = "0";
}

void AnimSpriteComponent::Update(float deltaTime)
{
  SpriteComponent::Update(deltaTime);

  // get current animationClip
  AnimationClip animClip = m_AnimationClips[m_CurrentAnimationClip];

  // if current clip != prevCurrentClip, set current frame based on clip being played
  if (m_CurrentAnimationClip != m_PreviousAnimationClip)
  {
    // get animationClip
    m_CurrentFrame = animClip.startFrame;

    m_PreviousAnimationClip = m_CurrentAnimationClip;
  }

  if(m_AnimTextures.size() > 0)
  {
    // update current frame based on frame rate and delta time
    m_CurrentFrame += m_AnimFPS * deltaTime;

    // wrap current frame if needed
    if(m_CurrentFrame > animClip.endFrame)
    {
      if (animClip.isLooping)
      {
        m_CurrentFrame = animClip.startFrame;
      } else {
        m_CurrentFrame = animClip.endFrame;
      }
    }

    // set the current texture
    SetTexture(m_AnimTextures[static_cast<int>(m_CurrentFrame)]);
  }
}

void AnimSpriteComponent::SetAnimTextures(const std::vector<Texture*>& textures)
{
  m_AnimTextures = textures;
  if(m_AnimTextures.size() > 0)
  {
    // set active texture to first frame
    m_CurrentFrame = 0.0f;
    SetTexture(m_AnimTextures[0]);
  }
}

void AnimSpriteComponent::SetAnimationClip(std::string name, int startFrame, int endFrame, bool isLooping)
{
  AnimationClip animationClip = {name, startFrame, endFrame, isLooping};
  m_AnimationClips[name] = animationClip;
}


void AnimSpriteComponent::SetCurrentAnimationClip(std::string name)
{
  m_CurrentAnimationClip = name;
}

std::string AnimSpriteComponent::GetPreviousAnimationClip() const
{
  return m_PreviousAnimationClip;
}

std::string AnimSpriteComponent::GetCurrentAnimationClip() const
{
  return m_CurrentAnimationClip;
}


float AnimSpriteComponent::GetAnimFPS() const { return m_AnimFPS; }

void AnimSpriteComponent::SetAnimFPS(float fps) { m_AnimFPS = fps; }
