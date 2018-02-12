#include "GameRendering.h"

GameRendering::GameRendering() {
    logic = new GameLogic();
    initAllegro();
    initGameElements();
    createEventQueue();
}

GameRendering::~GameRendering() {
    al_destroy_bitmap(gilDead);
    al_destroy_bitmap(gilAlive);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_font(fontResult);
    al_destroy_font(fontTraining);
    delete logic;
}

void GameRendering::initAllegro() {
  if(!al_init()) {
     fprintf(stderr, "failed to initialize allegro!\n");
     exit(1);
  }

  if(!al_init_image_addon()) {
     fprintf(stderr, "Failed to initialize al_init_image_addon!\n");
     exit(1);
  }
  if(!al_init_primitives_addon()) {
     fprintf(stderr, "Failed to initialize al_init_primitives_addon!\n");
     exit(1);
  }

  if(!al_install_mouse()) {
     fprintf(stderr, "failed to initialize the mouse!\n");
     exit(1);
  }

  if(!al_install_keyboard()) {
     fprintf(stderr, "failed to initialize the keyboard!\n");
     exit(1);
  }
  al_init_font_addon();
  al_init_ttf_addon();
}


void GameRendering::initGameElements() {
  fontResult = al_load_font("../out/arial.ttf", SIZE_FONT_1, 0);
  fontTraining = al_load_font("../out/arial.ttf", SIZE_FONT_2, 0);

  timer = al_create_timer(1.0 / FPS);
  if(!timer) {
     fprintf(stderr, "failed to create timer!\n");
     exit(1);
  }

  display = al_create_display(SCREEN_W, SCREEN_H);
  if(!display) {
     fprintf(stderr, "failed to create display!\n");
     al_destroy_timer(timer);
     exit(1);
  }

  al_clear_to_color(al_map_rgb(50,100,150));

  gilAlive = al_load_bitmap("../out/gil1.png");
  if(!gilAlive) {
     fprintf(stderr, "failed to load image bitmap!\n");
     al_destroy_display(display);
     al_destroy_timer(timer);
     exit(1);
  }
  gilDead = al_load_bitmap("../out/gilDead.png");
  if(!gilDead) {
     fprintf(stderr, "failed to load image dead bitmap!\n");
     al_destroy_display(display);
     al_destroy_timer(timer);
     al_destroy_bitmap(gilAlive);
     exit(1);
  }
}

void GameRendering::drawTrees() {
  int numberOfTrees = logic->getNumberOfTrees();
  for(int i=0; i<numberOfTrees; i++)
    drawTree(i);
}

void GameRendering::drawTree(int i) {
    int x = logic->list_trees[i].x;
    int y1 = logic->list_trees[i].y1;
    int y2 = logic->list_trees[i].y2;
    al_draw_filled_rectangle(x, y1, x+WIDTH_TREE, 0, al_color_name("brown"));
    al_draw_filled_rectangle(x, SCREEN_H, x+WIDTH_TREE, y2, al_color_name("brown"));
    al_draw_filled_circle(x+WIDTH_TREE/2, y1+(2*CIRC_RAD)/3, CIRC_RAD, al_color_name("green"));
    al_draw_filled_circle(x+WIDTH_TREE/2, y2-(2*CIRC_RAD)/3, CIRC_RAD, al_color_name("green"));
}

void GameRendering::drawInitialBoard()
{
  al_clear_to_color(al_map_rgb(50,100,150));
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
    std::pair<float, float> b = logic->getIBirdPosition(i);
    al_draw_bitmap(gilAlive, b.first, b.second , 0);
  }
  for(int i=0; i<TREES_INITIALLY; i++)
    logic->createTree(logic->initialCordX[i]);
  drawTrees();
  al_flip_display();
}

void GameRendering::createEventQueue()
{
  event_queue = al_create_event_queue();
  if(!event_queue) {
     fprintf(stderr, "failed to create event_queue!\n");
     al_destroy_bitmap(gilDead);
     al_destroy_bitmap(gilAlive);
     al_destroy_display(display);
     al_destroy_timer(timer);
     exit(1);
  }

  al_register_event_source(event_queue, al_get_display_event_source(display));

  al_register_event_source(event_queue, al_get_timer_event_source(timer));

  al_register_event_source(event_queue, al_get_mouse_event_source());

  al_register_event_source(event_queue, al_get_keyboard_event_source());
}

void GameRendering::startGame() {
  al_start_timer(timer);
  ALLEGRO_EVENT eve;
  while(1)
  {
      al_wait_for_event(event_queue, &eve);
      if(eve.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        exit(1);
      else if(eve.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        return;
      else if(eve.type == ALLEGRO_EVENT_KEY_DOWN) {
        if(eve.keyboard.keycode == ALLEGRO_KEY_SPACE)
          return;
      }
   }
}

void GameRendering::init() {
   for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
      logic->gil[i]->backInitialValues();
   logic->list_trees.clear();
   logic->id = 0;
   drawInitialBoard();
}

void GameRendering::render() {
    al_clear_to_color(al_map_rgb(50,100,150));
    const char* res = logic->getTextForResult(logic->gameScore);
    const char* res1 = logic->getTextForBestResult(logic->bestResult);
    for(int i=0; i<logic->getNumberOfTrees(); i++) {
      drawTree(i);
    }
    for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
        if(logic->gil[i]->active) {
           al_draw_bitmap(gilAlive, logic->gil[i]->x, logic->gil[i]->y, 0);
        }
    }
    al_draw_text(fontResult, al_map_rgb(0, 0, 0), 0, 0, 0, res);
    al_draw_text(fontResult, al_map_rgb(0, 0, 0), 750, 0, 0, res1);
    al_flip_display();
    free((void*) res1);
    free((void *) res);
}

void GameRendering::mainLoop() {
  int round = 0;
  bool firstShowGame = false;
  while(1) {
      switch(round) {
          case SHOW_MOVEMENT:
              init();
              if(!firstShowGame) startGame();
              firstShowGame = true;
              logic->gameScore = 0;
              while(1) {
                ALLEGRO_EVENT ev;
                al_wait_for_event(event_queue, &ev);

                if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                    return;
                else if(ev.type == ALLEGRO_EVENT_TIMER) {
                    logic->defeatTree();
                    logic->moveTrees();
                    logic->checkNewTree();
                    logic->updateBirds();
                    render();
                    logic->collisions();
                    logic->belowBoardAllBirds();
                    logic->removeTree();
                 }
                 if(al_is_event_queue_empty(event_queue)) {
                     if(logic->allDead())
                        break;
                 }
              }
              if(logic->gameScore > logic->bestResult)
                  logic->bestResult = logic->gameScore;
              round = HIDDEN_BOT_TRAIN;
              break;
          case HIDDEN_BOT_TRAIN:
              logic->training(HIDDEN_ROUNDS);
              round = SHOW_MOVEMENT;
              break;
      }
  }
}
