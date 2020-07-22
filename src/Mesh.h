#pragma once

#include <vector>
#include <string>

class Mesh
{
private:
  // textures associated with this mesh
  std::vector<class Texture*> m_Textures;
  // vertex array associated with this mesh
  class VertexArray* m_VertexArray;
  // name of shader specified by mesh
  std::string m_ShaderName;
  // stores object space bounding sphere radius
  float m_Radius;
  float m_SpecPower; // specular power of surface
public:
  Mesh();
  ~Mesh();

  bool Load(const std::string & fileName, class Renderer* renderer);
  void Unload();

  class Texture* GetTexture(size_t index); // get texture from specified index
  class VertexArray* GetVertexArray();
  const std::string& GetShaderName() const;
  float GetRadius() const;
  float GetSpecPower() const;

};
