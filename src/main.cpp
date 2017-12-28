#include "main.h"
using namespace std;
int main(int argc, char **argv)
{
   srand(time(NULL));
   Game singleGame;
   singleGame.mainLoop();
   return 0;
}
