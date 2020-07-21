#pragma once

#include <string>

class Texture
{
public:
  Texture();
  ~Texture();

  bool Load(const std::string& fileName);
  void Unload();
  void SetActive();

  int GetWidth() const;
  int GetHeight() const;

private:
  // openGL ID of this texture
  unsigned int m_TextureId;
  int m_Width;
  int m_Height;

  // cherry-picked SOIL functs
  unsigned char* SOIL_load_image(const char *filename, int *width, int *height, int *channels, int force_channels);
  void SOIL_free_image_data(unsigned char *img_data);
};
