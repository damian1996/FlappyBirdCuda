#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "consts.h"
#include "bird.h"
#include "tree.h"
#include "Bot.h"
using namespace std;

class GameLogic {  
public:
  vector<Tree> list_trees;
  Bird gil[10];
  int gameScore;
  Bot botToTrain;

  GameLogic();
  bool droppedTooLow(int bird);
  bool intersects(int bird, float CircX, float CircY);
  bool collisionCheck(int bird);
  std::pair<float, float> getIBirdPosition(int i);
  Tree getTreePosition(int i);
  int getNumberOfTrees();
  void update(int i);
  bool moveUp(int bird);
  int getScore();
  void createTree(float x);
  char* getTextForResult(int res);
  bool treeToRemove();
  bool allDead();
};

#endif
