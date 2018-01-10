#include "main.h"
using namespace std;
int main(int argc, char **argv)
{
   srand(time(NULL));
   while(1){
     Game singleGame;
     singleGame.mainLoop();
   }
   return 0;
}
