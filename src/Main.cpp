#include "Game.h"

int main(int argc, char* args[]){

  Game game;
  
  if (game.Initialize())
  {
    game.RunLoop();
  }

  game.ShutDown();

  return 0;
}
