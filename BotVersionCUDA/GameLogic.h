#ifndef GAMELOGIC_H
#define GAMELOGIC_H

using namespace std;
#include "tree.h"
#include "bird.h"
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include "CudaX.h"

class GameLogic {
public:
  CudaX *cudaObj;
  vector<Tree> list_trees;
  Bird **gil;
  int gameScore, bestResult;
  float initialCordX[3] = {400.0, 650.0, 900.0};
  int id = 0;

  GameLogic();
  ~GameLogic();
  bool droppedTooLow(int b);
  int getNumberOfTrees();
  std::pair<float, float> getIBirdPosition(int b);
  void createTree(float i);
  void update(int i);
  void updateBirds();
  bool intersects(int b, float CircX, float CircY);
  bool collisionCheck(int b);
  void defeatTree();
  bool allDead();
  char* getTextForResult(int res);
  char* getTextForBestResult(int res);
  void collisions();
  void belowBoardAllBirds();
  void moveTrees();
  void checkNewTree();
  void removeTree();
  void initTrainingSession();
  void addFitForActiveBirds();
  void training(int i);
};

#endif
