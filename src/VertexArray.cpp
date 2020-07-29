#include "VertexArray.h"

#include <GL/glew.h>

// assumes:
// position (3), normal (3), texUV (2)
VertexArray::VertexArray(const void* verts
  , unsigned int numVerts
  , const unsigned int* indices
  , unsigned int numIndices
  , Layout layout
)
  :m_NumVerts(numVerts)
  , m_NumIndices(numIndices)
{
  // create vertex array
  glGenVertexArrays(1, &m_VertexArray);
  glBindVertexArray(m_VertexArray);

  unsigned vertexSize = 8 * sizeof(float);
  if (layout == PosNormSkinTex)
  {
    vertexSize = 8 * sizeof(float) + 8 * sizeof(char);
  }

  // create vertex buffer
  glGenBuffers(1, &m_VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
  // copy data into vertex buffer
  glBufferData(
    GL_ARRAY_BUFFER                 // active buffer type to write to
    , numVerts * 8 * sizeof(float)  // number of bytes to copy
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

  // specify vertex attributes
  if (layout == PosNormTex)
  {
    // specify a vertex layout aka vertex attributes
    // position
    glEnableVertexAttribArray(0); // enable first attribute 0
    glVertexAttribPointer(
      0                     // attribute index
      , 3                   // number of components
      , GL_FLOAT            // type of components
      , GL_FALSE            // used only for integral types
      , vertexSize          // stride (usually size of each vertex)
      , 0                   // offset from start of vertex to this attrib
    );

    // normal
    glEnableVertexAttribArray(1); // enable UV attrib
    glVertexAttribPointer(
      1                     // attrib. index
      , 3                   // only 2 components in UV
      , GL_FLOAT
      , GL_FALSE
      , vertexSize
      , reinterpret_cast<void*>(sizeof(float) * 3) // offest pointer
    );

    // tex UV
    glEnableVertexAttribArray(2); // enable UV attrib
    glVertexAttribPointer(
      2                     // attrib. index
      , 2                   // only 2 components in UV
      , GL_FLOAT
      , GL_FALSE
      , vertexSize
      , reinterpret_cast<void*>(sizeof(float) * 6) // offest pointer
    );
  }
  else if (layout == PosNormSkinTex)
  {
    // specify a vertex layout aka vertex attributes
    // position
    glEnableVertexAttribArray(0); // enable first attribute 0
    glVertexAttribPointer(
      0                     // attribute index
      , 3                   // number of components
      , GL_FLOAT            // type of components
      , GL_FALSE            // used only for integral types
      , vertexSize          // stride (usually size of each vertex)
      , 0                   // offset from start of vertex to this attrib
    );

    // normal
    glEnableVertexAttribArray(1); // enable UV attrib
    glVertexAttribPointer(
      1                     // attrib. index
      , 3                   // only 2 components in UV
      , GL_FLOAT
      , GL_FALSE
      , vertexSize
      , reinterpret_cast<void*>(sizeof(float) * 3) // offest pointer
    );

    // skinning indices (keep as ints)
    glEnableVertexAttribArray(2); // enable UV attrib
    glVertexAttribIPointer(
      2
      , 4
      , GL_UNSIGNED_BYTE
      , vertexSize
      , reinterpret_cast<void*>(sizeof(float) * 6) // offest pointer
    );

    // skinning weights (convert to floats)
    glEnableVertexAttribArray(3); // enable UV attrib
    glVertexAttribPointer(
      3                     // attrib. index
      , 4                   // only 2 components in UV
      , GL_UNSIGNED_BYTE
      , GL_TRUE
      , vertexSize
      , reinterpret_cast<void*>(sizeof(float) * 6 + sizeof(char) * 4) // offest pointer
    );

    // tex UV
    glEnableVertexAttribArray(4); // enable UV attrib
    glVertexAttribPointer(
      4                     // attrib. index
      , 2                   // only 2 components in UV
      , GL_FLOAT
      , GL_FALSE
      , vertexSize
      , reinterpret_cast<void*>(sizeof(float) * 6 + sizeof(char) * 8) // offest pointer
    );
  }

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
