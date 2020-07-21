#pragma once

#include "SpriteComponent.h"
#include <vector>
#include <string>

// reads in a CSV file and draws map using Tile.png
class TileMapComponent: public SpriteComponent {
private:
  int m_ScreenRows;
  int m_ScreenCols;
  int m_TextureRows;
  int m_TextureCols;
  std::vector<int> csv; // TODO choose size of tile map and set size at start
    // TODO once working, switch to 2D vector OR store width by counting chars until first \n char
    // and store height by counting all \n

  void ConvertIdToPosition(int cellId, int rows, int cols, int& i, int& j);
public:
  TileMapComponent(class Actor* owner, int drawOrder = 10);
  bool LoadCsv(std::string fileName);
  void SetTextureRowsCols(int rows, int cols);
  void Draw(class Shader* shader) override;
};
