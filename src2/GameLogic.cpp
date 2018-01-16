#include "GameLogic.h"

GameLogic::GameLogic() {
  gameScore = 0;
  botToTrain.initBot();
}

bool GameLogic::droppedTooLow(int b) {
  if(gil[b].y>=SCREEN_H) return false;
  else return true;
}

bool GameLogic::intersects(int b, float CircX, float CircY) {
  float DeltaX = CircX - max(gil[b].x, min(CircX, gil[b].x + GIL_SIZE));
  float DeltaY = CircY - max(gil[b].y, min(CircY, gil[b].y + GIL_SIZE));
  return (DeltaX*DeltaX + DeltaY*DeltaY) < (CIRC_RAD*CIRC_RAD);
}

bool GameLogic::collisionCheck(int b) {
    for(auto& tree : list_trees)
    {
      // pnie
      float wspX = gil[b].x + GIL_SIZE;
      float wspY = gil[b].y + GIL_SIZE;
      //printf("%f %f %f\n", tree.x, wspX, tree.x+WIDTH_TREE);
      //printf("%f > %f\n", tree.y, gil[b].y);
      if(tree.x<wspX && tree.x+WIDTH_TREE>gil[b].x && tree.y>gil[b].y) // && 0<wspY
        return false;
      if(tree.x<wspX && tree.x+WIDTH_TREE>gil[b].x && tree.height2<wspY) // && SCREEN_H>gil[b].y
        return false;
      // okregi
      float middleX = tree.x+WIDTH_TREE/2;
      float middleY = tree.y+(2*CIRC_RAD)/3;
      bool res = intersects(b, middleX, middleY);
      if(res) return false;
      middleY = tree.height2-(2*CIRC_RAD)/3;
      res = intersects(b, middleX, middleY);
      if(res) return false;
    }
    return true;
}

std::pair<float, float> GameLogic::getIBirdPosition(int b) {
  return std::make_pair(gil[b].x, gil[b].y);
}

Tree GameLogic::getTreePosition(int i) {
  return list_trees[i].copyTree();
}

int GameLogic::getNumberOfTrees() {
  return list_trees.size();
}

void GameLogic::update(int i) {
  if(gil[i].moveUp) {
    gil[i].y -= BIRD_IMPR;
  } else {
    float decr = SCREEN_H/(FPS*TIME_FALL_BIRD);
    gil[i].y += decr;
  }
}

void GameLogic::moveUp(int b) {
  gil[b].moveUp = true;
}

int GameLogic::getScore() {
  return gameScore;
}

void GameLogic::createTree(float i) {
  Tree t(i);
  t.heights();
  list_trees.push_back(t);
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

bool GameLogic::treeToRemove()
{
    for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
    {
        if(gil[i].active)
        {
            if(!list_trees[0].add && gil[i].x>list_trees[0].x+WIDTH_TREE)
            {
               list_trees[0].add = true;
               gameScore++;
               return true;
            }
        }
    }
    return false;
}

bool GameLogic::allDead()
{
  int x=0;
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
  {
    if(gil[i].active) x++;
  }
  //printf("AKTYWNE %d\n", x);
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
  {
    if(gil[i].active) return false;
  }
  return true;
}
