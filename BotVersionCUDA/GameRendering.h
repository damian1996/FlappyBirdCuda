#ifndef GAMERENDERING_H
#define GAMERENDERING_H

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include "consts.h"
#include "GameLogic.h"

class GameRendering {
public:
  ALLEGRO_DISPLAY *display = NULL;
  ALLEGRO_EVENT_QUEUE *event_queue = NULL;
  ALLEGRO_TIMER *timer = NULL;
  ALLEGRO_BITMAP *gilAlive = NULL;
  ALLEGRO_BITMAP *gilDead = NULL;
  ALLEGRO_FONT *fontResult = NULL;
  ALLEGRO_FONT *fontTraining = NULL;
  GameLogic *logic;

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
  void render();
  void mainLoop();
};

#endif
