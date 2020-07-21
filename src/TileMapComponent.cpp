#include "TileMapComponent.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

TileMapComponent::TileMapComponent(class Actor* owner, int drawOrder)
  :SpriteComponent(owner, drawOrder)
  , m_ScreenRows(0)
  , m_ScreenCols(0)
  , m_TextureRows(0)
  , m_TextureCols(0)
{}

bool TileMapComponent::LoadCsv(std::string fileName)
{
  std::ifstream fin(fileName, std::ios::in);

  if (!fin)
  {
    std::cout<<"\nTile Map Component failed to load csv file: " << fileName;
    return false;
  }

  std::string line, word, temp;
  bool haveCountedCols = false;
  // loop over all rows
  while(std::getline(fin, line))
  {
    m_ScreenRows++; // count rows

    // break line into words
    // read every column data of a row and store it in string var 'word'
    std::stringstream ss(line);
    while(std::getline(ss, word, ',')) // TODO: breaks if spaces like ", " ?
    {
      if(!haveCountedCols)
      {
        ++m_ScreenCols;
      }
      // add word to vector as number
      csv.push_back(stoi(word));
    }
    if (!haveCountedCols) {haveCountedCols = true;}
  }

  fin.close();
  return true;
}

void TileMapComponent::Draw(Shader* shader)
{
  // TODO : update to use Shader
  /*
  if (m_Texture)
  {
    int screenCellId = 0; // draw left to right, top to bottom
    for(int textureCellId : csv)
    {
      if (textureCellId != -1) // -1 is empty
      {
        int texture_i, texture_j;
        ConvertIdToPosition(textureCellId, m_TextureRows, m_TextureCols, texture_i, texture_j);
        SDL_Rect textureRect;
        int textureTileWidth = m_TextureWidth / m_TextureCols;
        int textureTileHeight = m_TextureHeight / m_TextureRows;
        textureRect.w = static_cast<int>(textureTileWidth);
        textureRect.h = static_cast<int>(textureTileHeight);
        textureRect.x = static_cast<int>(texture_j * textureTileWidth);
        textureRect.y = static_cast<int>(texture_i * textureTileHeight);

        int screen_i, screen_j;
        ConvertIdToPosition(screenCellId, m_ScreenRows, m_ScreenCols, screen_i, screen_j);
        SDL_Rect screenRect;
        int screenTileWidth = SCREEN_WIDTH / m_ScreenCols;
        int screenTileHeight = SCREEN_HEIGHT / m_ScreenRows;
        screenRect.w = static_cast<int>(screenTileWidth);
        screenRect.h = static_cast<int>(screenTileHeight);
        screenRect.x = static_cast<int>(screen_j * screenTileWidth);
        screenRect.y = static_cast<int>(screen_i * screenTileHeight);

        SDL_RenderCopyEx(
           renderer
          , m_Texture
          , &textureRect
          , &screenRect
          , 0 // TODO: add rotation info to sprite sheet?
          , nullptr
          , SDL_FLIP_NONE
        );
      }

      ++screenCellId;
    }
  }
  */
}

void TileMapComponent::ConvertIdToPosition(int cellId, int rows, int cols, int& i, int& j)
{
  i = std::floor(cellId / cols);  // row = i = y
  j = cellId % cols;              // col = j = x
}

void TileMapComponent::SetTextureRowsCols(int rows, int cols)
{
  m_TextureRows = rows;
  m_TextureCols = cols;
}
