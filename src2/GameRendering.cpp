#include "GameRendering.h"

GameRendering::GameRendering()
{
  initAllegro();
  initGameElements();
  createEventQueue();
}

GameRendering::~GameRendering()
{
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
    al_destroy_bitmap(gilDead[i]);
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
    al_destroy_bitmap(gilAlive[i]);
  al_destroy_timer(timer);
  al_destroy_display(display);
  al_destroy_event_queue(event_queue);
  al_destroy_font(font);
}

void GameRendering::initAllegro()
{
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

void GameRendering::initGameElements()
{
  font = al_load_font("../out/arial.ttf",24,0);
  font2 = al_load_font("../out/arial.ttf",50,0);

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

  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
    gilAlive[i] = al_load_bitmap("../out/gil1.png");
    if(!gilAlive[i]) {
       fprintf(stderr, "failed to load image bitmap!\n");
       al_destroy_display(display);
       al_destroy_timer(timer);
       exit(1);
    }
    gilDead[i] = al_load_bitmap("../out/gilDead.png");
    if(!gilDead[i]) {
       fprintf(stderr, "failed to load image dead bitmap!\n");
       al_destroy_display(display);
       al_destroy_timer(timer);
       for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
          al_destroy_bitmap(gilAlive[i]);
       exit(1);
    }
  }
}

void GameRendering::drawTrees()
{
  int numberOfTrees = logic.getNumberOfTrees();
  for(int i=0; i<numberOfTrees; i++)
    drawTree(i);
}

void GameRendering::drawTree(int i) {
    Tree t = logic.getTreePosition(i);
    al_draw_filled_rectangle(t.x, t.y, t.x+WIDTH_TREE, 0, al_color_name("brown"));
    al_draw_filled_rectangle(t.x, SCREEN_H, t.x+WIDTH_TREE, t.height2, al_color_name("brown"));
    al_draw_filled_circle(t.x+WIDTH_TREE/2, t.y+(2*CIRC_RAD)/3, CIRC_RAD, al_color_name("green"));
    al_draw_filled_circle(t.x+WIDTH_TREE/2, t.height2-(2*CIRC_RAD)/3, CIRC_RAD, al_color_name("green"));
}

void GameRendering::drawInitialBoard()
{
  al_clear_to_color(al_map_rgb(50,100,150));
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
    std::pair<float, float> b = logic.getIBirdPosition(i);
    al_draw_bitmap(gilAlive[i], b.first, b.second , 0); // +i*10, tablica bitmap
  }
  for(int i=0; i<TREES_INITIALLY; i++)
    logic.createTree(xes[i]);
  drawTrees();
  al_flip_display();
}

void GameRendering::createEventQueue()
{
  event_queue = al_create_event_queue();
  if(!event_queue) {
     fprintf(stderr, "failed to create event_queue!\n");
     for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
        al_destroy_bitmap(gilDead[i]);
     for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
        al_destroy_bitmap(gilAlive[i]);
     al_destroy_display(display);
     al_destroy_timer(timer);
     exit(1);
  }

  al_register_event_source(event_queue, al_get_display_event_source(display));

  al_register_event_source(event_queue, al_get_timer_event_source(timer));

  al_register_event_source(event_queue, al_get_mouse_event_source());

  al_register_event_source(event_queue, al_get_keyboard_event_source());
}

void GameRendering::startGame()
{
  al_start_timer(timer);
  ALLEGRO_EVENT eve;
  while(1)
  {
    al_wait_for_event(event_queue, &eve);
    if(eve.type == ALLEGRO_EVENT_DISPLAY_CLOSE) exit(1);
    else if(eve.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) return;
    else if(eve.type == ALLEGRO_EVENT_KEY_DOWN)
      if(eve.keyboard.keycode == ALLEGRO_KEY_SPACE)
        return;
   }
}

void GameRendering::init() {
   for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
      logic.gil[i].backInitialValues();
   logic.list_trees.clear();
   drawInitialBoard();
 }

void GameRendering::threadFunction() {
  logic.botToTrain.computeHiddenRounds();
}

void GameRendering::mainLoop()
{
  bool start = false;
  int round = 0;
  while(1)
  {
    switch(round) {
        case SHOW_MOVEMENT:
            init();

            for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
              if(i < NUMBER_DISPLAY_BOTS/2)
                logic.gil[i].y -= i*15.0;
              else
                logic.gil[i].y += (i - NUMBER_DISPLAY_BOTS/2)*15.0;
            }

            if(!start) {
              startGame();
              start = true;
            }
            logic.gameScore = 0;
            while(1)
            {
                ALLEGRO_EVENT ev;
                al_wait_for_event(event_queue, &ev);
                logic.treeToRemove();

                if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                  return;
                }
                else if(ev.type == ALLEGRO_EVENT_TIMER) {
                    float decr = SCREEN_H/(FPS*TIME_FALL_BIRD);
                    for(auto& it : logic.list_trees) // przesuniecie drzew
                      it.x -= MOVE_PIXS_ON_SEC/FPS;
                    if(SCREEN_W - logic.list_trees[logic.list_trees.size()-1].x >= DIST_BET_TWO) {
                       logic.createTree(logic.list_trees[logic.list_trees.size()-1].x + DIST_BET_TWO);
                       drawTree(logic.getNumberOfTrees()-1);
                    }
                    for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
                      if(logic.gil[i].active) {
                          int idx = logic.list_trees[0].x > logic.gil[i].x ? 0 : 1;
                          float horizontal = logic.list_trees[idx].x+WIDTH_TREE-logic.gil[i].x;
                          float heighDiff = ((logic.list_trees[idx].y + logic.list_trees[idx].height2)/2) - logic.gil[i].y;
                          //if(heighDiff<0) heighDiff = -heighDiff;
                          float val = logic.botToTrain.visibleNets[i].sigmoid(horizontal, heighDiff);
                          //if(val>=0.5) logic.gil[i].moveUp = true;
                          if(val>=0) logic.gil[i].moveUp = true;
                          else logic.gil[i].moveUp = false;
                      }
                    }
                    for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
                       logic.gil[i].active = logic.collisionCheck(i);
                    }
                }

                for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
                {
                  if(logic.gil[i].active==true)
                    logic.gil[i].active = logic.droppedTooLow(i);
                }
                if(al_is_event_queue_empty(event_queue)) {
                    if(logic.allDead()==false) {
                        if(logic.list_trees[0].x + WIDTH_TREE < 0)
                           logic.list_trees.erase(logic.list_trees.begin());
                        al_clear_to_color(al_map_rgb(50,100,150));
                        const char* res = logic.getTextForResult(logic.gameScore);

                        for(int i=0; i<logic.getNumberOfTrees(); i++) {
                          drawTree(i);
                        }

                        for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
                            if(logic.gil[i].active) {
                               logic.update(i);
                               al_draw_bitmap(gilAlive[i], logic.gil[i].x, logic.gil[i].y, 0);
                            }
                        }

                        al_draw_text(font, al_map_rgb(0, 0, 0), 0, 0, 0, res);
                        al_flip_display();
                    }
                    else if(logic.allDead()==true){
                      break;
                    }
                }
            }
            al_clear_to_color(al_map_rgb(50,100,150));
            al_flip_display();
            round = HIDDEN_BOT_TRAIN;
            break;
        case HIDDEN_BOT_TRAIN:
          const char* res2 = "Chwilka postoju, trwa trening.";
          al_draw_text(font2, al_map_rgb(0, 0, 0), 200, 250, 0, res2);
          al_flip_display();
          logic.botToTrain.computeHiddenRounds();
          round = SHOW_MOVEMENT;
          break;
    }
  }
}
