#include "Shader.h"

#include <SDL2/SDL.h>
#include <fstream>
#include <sstream>

Shader::Shader()
  :m_ShaderProgram(0)
  , m_VertexShader(0)
  , m_FragShader(0)
{}

Shader::~Shader()
{}

bool Shader::Load(const std::string& vertName, const std::string& fragName)
{
  // tries to link both vertex & fragment shaders together
  // returns false if either vert or frag shader failed to load

  // compile vertex and fragment shaders
  if(!CompileShader(vertName, GL_VERTEX_SHADER, m_VertexShader) ||
     !CompileShader(fragName, GL_FRAGMENT_SHADER, m_FragShader))
     {
       return false;
     }

  // create a shader program that links vert / frag shaders
  m_ShaderProgram = glCreateProgram();
  glAttachShader(m_ShaderProgram, m_VertexShader);
  glAttachShader(m_ShaderProgram, m_FragShader);
  glLinkProgram(m_ShaderProgram);

  // verify program linked correctly
  if (!IsValidProgram())
  {
    return false;
  }
  return true;
}

void Shader::SetActive()
{
  glUseProgram(m_ShaderProgram);
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
{
  // find the uniform by this name
  GLuint loc = glGetUniformLocation(m_ShaderProgram, name);

  // send the matrix data to the uniform
  glUniformMatrix4fv(
    loc                      // unform ID 
    , 1                      // number of matrices
    , GL_TRUE                // set to true if using row vectors
    , matrix.GetAsFloatPtr() // pointer to matrix data
  );
}


bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
  std::ifstream shaderFile(fileName);
  if(shaderFile.is_open())
  {
    // read all etxt into a string
    std::stringstream sstream;
    sstream << shaderFile.rdbuf();
    std::string contents = sstream.str();
    const char* contentsChar = contents.c_str();

    // create a shader of the specified type
    outShader = glCreateShader(shaderType);

    // set the source characters and try to compile
    glShaderSource(outShader, 1, &(contentsChar), nullptr);
    glCompileShader(outShader);
    if (!IsCompiled(outShader))
    {
      SDL_Log("Failed to compile shader %s", fileName.c_str());
      return false;
    }
  }
  else
  {
    SDL_Log("Shader file not found %s", fileName.c_str());
    return false;
  }
  return true;
}

bool Shader::IsCompiled(GLuint shader)
{
  GLint status;
  // query compile status
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if(status != GL_TRUE)
  {
    char buffer[512];
    memset(buffer, 0, 512);
    glGetShaderInfoLog(shader, 511, nullptr, buffer);
    SDL_Log("GLSL Compile Failed:\n%s", buffer);
    return false;
  }
  return true;
}

bool Shader::IsValidProgram()
{
  GLint status;
  // query compile status
  glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &status);
  if(status != GL_TRUE)
  {
    char buffer[512];
    memset(buffer, 0, 512);
    glGetProgramInfoLog(m_ShaderProgram, 511, nullptr, buffer);
    SDL_Log("GLSL shader program linking Failed:\n%s", buffer);
    return false;
  }
  return true;
}

void Shader::Unload()
{
  glDeleteProgram(m_ShaderProgram);
  glDeleteShader(m_VertexShader);
  glDeleteShader(m_FragShader);
}
