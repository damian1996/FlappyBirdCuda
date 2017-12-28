#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "consts.h"
#include "bird.h"
#include "tree.h"
using namespace std;

class Game {
  ALLEGRO_DISPLAY *display = NULL;
  ALLEGRO_EVENT_QUEUE *event_queue = NULL;
  ALLEGRO_TIMER *timer = NULL;
  ALLEGRO_BITMAP *gilAlive = NULL;
  ALLEGRO_BITMAP *gilDead = NULL;
  ALLEGRO_FONT *font = NULL;
  vector<Tree> list_trees;
  float xes[4] = {400.0, 650.0, 900.0};
public:
  Game(){};
  void initAllegro();
  void initGame();
  void drawInitialBoard(Bird &gil);
  void createEventQueue();
  char* getTextForResult(int res);
  void startGame();
  bool droppedTooLow(Bird& bird);
  bool intersects(Bird& bird, float CircX, float CircY);
  bool collisionCheck(Bird& bird);
  void reallocAll();
  void solve(Bird &gil);
};

#endif
