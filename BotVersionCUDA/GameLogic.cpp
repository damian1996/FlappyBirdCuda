#include "GameLogic.h"
GameLogic::GameLogic() {
  cudaObj = new CudaX();
  gil = new Bird*[NUMBER_DISPLAY_BOTS];
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
    gil[i] = new Bird();
  gameScore = 0;
  bestResult = 0;
}

GameLogic::~GameLogic() {
  delete cudaObj;
  delete [] gil;
}

bool GameLogic::droppedTooLow(int b) {
  if(gil[b]->y>=SCREEN_H) return false;
  else return true;
}

int GameLogic::getNumberOfTrees() {
  return list_trees.size();
}

std::pair<float, float> GameLogic::getIBirdPosition(int b) {
  return std::make_pair(gil[b]->x, gil[b]->y);
}

void GameLogic::createTree(float i) {
  Tree t(i, ++id);
  list_trees.push_back(t);
}


void GameLogic::update(int i) {
  float t = 1.0/FPS;
  gil[i]->ticks += 1;

  if(gil[i]->moveUp) {
    gil[i]->y += gil[i]->speed*t;
    gil[i]->speed = -180;
  } else {
    gil[i]->y += gil[i]->speed*t;
    gil[i]->speed += gil[i]->acc*t;
  }
}

void GameLogic::updateBirds() {
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
      if(gil[i]->active) {
          int idx = list_trees[0].x+WIDTH_TREE > gil[i]->x ? 0 : 1;
          float horizontal = list_trees[idx].x+WIDTH_TREE-gil[i]->x;
          float heighDiff = ((list_trees[idx].y1 + list_trees[idx].y2)/2) - gil[i]->y;
          float sp = gil[i]->speed;
          float val = gil[i]->net->sigmoid(horizontal, heighDiff, sp);
          if(val>=gil[i]->net->bias) gil[i]->moveUp = true;
          else gil[i]->moveUp = false;
          update(i);
      }
  }
}

bool GameLogic::intersects(int b, float CircX, float CircY) {
  float DeltaX = CircX - max(gil[b]->x, min(CircX, gil[b]->x + GIL_SIZE));
  float DeltaY = CircY - max(gil[b]->y, min(CircY, gil[b]->y + GIL_SIZE));
  return (DeltaX*DeltaX + DeltaY*DeltaY) < (CIRC_RAD*CIRC_RAD);
}

bool GameLogic::collisionCheck(int b) {
    for(auto& tree : list_trees)
    {
      float wspX = gil[b]->x + GIL_SIZE;
      float wspY = gil[b]->y + GIL_SIZE;
      if(tree.x<wspX && tree.x+WIDTH_TREE>gil[b]->x && tree.y1>gil[b]->y)
        return false;
      if(tree.x<wspX && tree.x+WIDTH_TREE>gil[b]->x && tree.y2<wspY)
        return false;

      float middleX = tree.x+WIDTH_TREE/2;
      float middleY = tree.y1+(2*CIRC_RAD)/3;
      bool res = intersects(b, middleX, middleY);
      if(res) return false;
      middleY = tree.y2-(2*CIRC_RAD)/3;
      res = intersects(b, middleX, middleY);
      if(res) return false;
    }
    return true;
}

void GameLogic::defeatTree()
{
    for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
    {
        if(gil[i]->active)
        {
            if(!list_trees[0].scored_tree && gil[i]->x>list_trees[0].x+WIDTH_TREE)
            {
               list_trees[0].scored_tree = true;
               gameScore++;
            }
        }
    }
}

bool GameLogic::allDead()
{
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
  {
    if(gil[i]->active) return false;
  }
  return true;
}

char* GameLogic::getTextForResult(int res)
{
  const char *start = "Twoj wynik to : ";
  std::string s = std::to_string(res);
  const char *pchar = s.c_str();
  char* result = (char*)malloc(100*1);
  strcpy(result,start);
  strcat(result,pchar);
  return (char*)result;
}

char* GameLogic::getTextForBestResult(int res) {
  const char *start = "Najlepszy wynik to : ";
  std::string s = std::to_string(res);
  const char *pchar = s.c_str();
  char* result = (char*)malloc(100*1);
  strcpy(result,start);
  strcat(result,pchar);
  return (char*)result;
}


void GameLogic::collisions() {
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
    if(gil[i]->active && !collisionCheck(i)) {
      gil[i]->active = false;
      int idx = list_trees[0].x+WIDTH_TREE > gil[i]->x ? 0 : 1;
      float heighDiff = ((list_trees[idx].y1 + list_trees[idx].y2)/2) - gil[i]->y;
      if(heighDiff<0) heighDiff = -heighDiff;
      gil[i]->fit += -heighDiff;
    }
  }
}

void GameLogic::belowBoardAllBirds() {
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
     gil[i]->active = gil[i]->active && droppedTooLow(i);
  }
}

void GameLogic::moveTrees() {
  for(auto& it : list_trees)
     it.x -= MOVE_PIXS_ON_SEC/FPS;
}

void GameLogic::checkNewTree() {
    int vec_size = list_trees.size();
    if(SCREEN_W - list_trees[vec_size-1].x >= DIST_BET_TWO) {
        createTree(list_trees[vec_size-1].x + DIST_BET_TWO);
    }
}

void GameLogic::removeTree() {
  if(list_trees[0].x + WIDTH_TREE < 0)
     list_trees.erase(list_trees.begin());
}

void GameLogic::initTrainingSession() {
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
     gil[i]->backInitialValues();
  list_trees.clear();
  id = 0;
  gameScore = 0;
  for(int i=0; i<TREES_INITIALLY; i++)
    createTree(initialCordX[i]);
}

void GameLogic::addFitForActiveBirds() {
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
    if(gil[i]->active) {
      gil[i]->fit +=  MOVE_PIXS_ON_SEC/FPS;
    }
  }
}

void GameLogic::training(int rounds) {
    if(cudaObj->trainPart==0) cudaObj->firstRewrite();
    vector<Bird*> tempBirds = cudaObj->hiddenRounds();
    for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
      gil[i] = tempBirds[i];
    }
}
