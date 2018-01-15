#ifndef GAMERENDERING_H
#define GAMERENDERING_H

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "GameLogic.h"
#include <thread>
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "consts.h"
using namespace std;

class GameRendering {
public:
  ALLEGRO_DISPLAY *display = NULL;
  ALLEGRO_EVENT_QUEUE *event_queue = NULL;
  ALLEGRO_TIMER *timer = NULL;
  ALLEGRO_BITMAP *gilAlive[NUMBER_DISPLAY_BOTS] = { NULL };
  ALLEGRO_BITMAP *gilDead[NUMBER_DISPLAY_BOTS] = { NULL };
  ALLEGRO_FONT *font = NULL;
  ALLEGRO_FONT *font2 = NULL;
  GameLogic logic;
  float xes[4] = {400.0, 650.0, 900.0};

  GameRendering();
  ~GameRendering();
  void initAllegro();
  void initGameElements();
  void drawTrees();
  void drawTree(int i);
  void drawInitialBoard();
  void createEventQueue();
  void startGame();
  void init();
  void mainLoop();
  void threadFunction();
};

#endif
