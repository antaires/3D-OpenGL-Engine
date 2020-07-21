#include "VertexArray.h"

#include <GL/glew.h>

VertexArray::VertexArray(const float* verts, unsigned int numVerts, const unsigned int* indices, unsigned int numIndices)
  :m_NumVerts(numVerts)
  , m_NumIndices(numIndices)
{
  glGenVertexArrays(1, &m_VertexArray);
  glBindVertexArray(m_VertexArray);

  // create vertex buffer
  glGenBuffers(1, &m_VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
  // copy data into vertex buffer
  glBufferData(
    GL_ARRAY_BUFFER                 // active buffer type to write to
    , numVerts * 5 * sizeof(float)  // number of bytes to copy
    , verts                         // source to copy from (pointer)
    , GL_STATIC_DRAW                // how will we use this data?
  );

  // create index buffer
  glGenBuffers(1, & m_IndexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
  // copy indices data into index buffer
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER             // index buffer
    , numIndices * sizeof(unsigned int) // size of data
    , indices
    , GL_STATIC_DRAW
  );

  // specify a vertex layout aka vertex attributes
  // current layout is a position with three float values
  glEnableVertexAttribArray(0); // enable first attribute 0
  // specify size, type and format of attribute
  glVertexAttribPointer(
    0                     // attribute index
    , 3                   // number of components
    , GL_FLOAT            // type of components
    , GL_FALSE            // used only for integral types
    , sizeof(float) * 5   // stride (usually size of each vertex)
    , 0                   // offset from start of vertex to this attrib
  );

  glEnableVertexAttribArray(1); // enable UV attrib
  glVertexAttribPointer(
    1                     // attrib. index
    , 2                   // only 2 components in UV
    , GL_FLOAT
    , GL_FALSE
    , sizeof(float) * 5
    , reinterpret_cast<void*>(sizeof(float) * 3) // offest pointer 
  );
}

VertexArray::~VertexArray()
{
  glDeleteBuffers(1, &m_VertexBuffer);
  glDeleteBuffers(1, &m_IndexBuffer);
  glDeleteVertexArrays(1, &m_VertexArray);
}

void VertexArray::SetActive()
{
  // specify which vertex array we're currently using
  glBindVertexArray(m_VertexArray);
}

unsigned int VertexArray::GetNumIndices() const
{
  return m_NumIndices;
}

unsigned int VertexArray::GetNumVertices() const
{
  return m_NumVerts;
}
