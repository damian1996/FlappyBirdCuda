#include "main.h"
using namespace std;
int main(int argc, char **argv)
{
   srand(time(NULL));
   Game singleGame;
   singleGame.initAllegro();
   singleGame.initGame();
   Bird gil = {100.0, (float) SCREEN_H/2};
   singleGame.drawInitialBoard(gil);
   singleGame.createEventQueue();
   singleGame.startGame();
   singleGame.solve(gil);
   singleGame.reallocAll();
   return 0;
}
