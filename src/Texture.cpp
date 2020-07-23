#include "Texture.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
// #include <SOIL/SOIL.h> incompatable with mac, just cherry picking
// this should only be included in ONE file (this one)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture()
  :m_TextureId(0)
  , m_Width(0)
  , m_Height(0)
{}

Texture::~Texture()
{}

bool Texture::Load(const std::string& fileName)
{
  int channels = 0;

  unsigned char* image = SOIL_load_image(
    fileName.c_str()      // name of file
    , &m_Width
    , &m_Height
    , &channels
    , 0      // SOIL_LOAD_AUTO = 0, type of image file, auto = any
  );
  if (image == nullptr )
  {
    SDL_Log("SOIL failed to load image %s", fileName.c_str());
    return false;
  }

  // check channels to determine RGB vs RBGA
  int format = GL_RGB;
  if (channels == 4)
  {
    format = GL_RGBA;
  }

  // create OpenGL texture object and save id, bind and set active
  glGenTextures(1, &m_TextureId);
  glBindTexture(GL_TEXTURE_2D, m_TextureId);

  // copy raw image data into openGL texture object
  glTexImage2D(
    GL_TEXTURE_2D       // texture target
    , 0                 // level of detail
    , format            // color format of openGl
    , m_Width
    , m_Height
    , 0                 // border - this must be 0
    , format            // color format of input data
    , GL_UNSIGNED_BYTE  // bit depth of input data, unsigned = 8 bit channels
    , image             // pointer to image data
  );

  // once image data copied to openGL, free from memory
  SOIL_free_image_data(image);

  // enable bilinear filtering (blur on zoom in, rather than pixelate)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return true;
}

void Texture::Unload()
{
  glDeleteTextures(1, &m_TextureId);
}

void Texture::SetActive()
{
  glBindTexture(GL_TEXTURE_2D, m_TextureId);
}

int Texture::GetWidth() const
{
  return m_Width;
}

int Texture::GetHeight() const
{
  return m_Height;
}

unsigned char* Texture::SOIL_load_image(const char *fileName, int *width, int *height, int *channels, int force_channels)
{
	unsigned char *result = stbi_load( fileName, width, height, channels, force_channels );
	if( result == NULL )
	{
		SDL_Log("SOIL failed to load image: %s", fileName);
	}
	return result;
}

void Texture::SOIL_free_image_data(unsigned char *img_data)
{
	free( (void*)img_data );
}
