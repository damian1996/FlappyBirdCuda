#include "game.h"

Game::Game()
{
  Game::gil.x = 100.0;
  Game::gil.y =  (float) SCREEN_H/2;
  initAllegro();
  initGame();
  drawInitialBoard(gil);
  createEventQueue();
  startGame();
}

Game::~Game()
{
  al_destroy_bitmap(gilDead);
  al_destroy_bitmap(gilAlive);
  al_destroy_timer(timer);
  al_destroy_display(display);
  al_destroy_event_queue(event_queue);
  al_destroy_font(font);
}

void Game::initAllegro()
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

void Game::initGame()
{
  font = al_load_font("../out/arial.ttf",24,0);
  //font = al_create_builtin_font();

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

void Game::drawInitialBoard(Bird &gil)
{
  al_draw_bitmap(gilAlive, gil.x, gil.y, 0);

  for(int i=0; i<TREES_INITIALLY; i++)
  {
    Tree t(xes[i]);
    t.drawTree();
    list_trees.push_back(t);
  }
  al_flip_display();
}

void Game::createEventQueue()
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

char* Game::getTextForResult(int res)
{
  const char *start = "Twoj wynik to : ";
  std::string s = std::to_string(res);
  const char *pchar = s.c_str();
  char* result = (char*)malloc(100*1);
  strcpy(result,start);
  strcat(result,pchar);
  return (char*)result;
}

void Game::startGame()
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

bool Game::droppedTooLow(Bird& bird)
{
  if(bird.y>=SCREEN_H) return true;
  else return false;
}

bool Game::intersects(Bird& bird, float CircX, float CircY)
{
    float DeltaX = CircX - max(bird.x, min(CircX, bird.x + GIL_SIZE));
    float DeltaY = CircY - max(bird.y, min(CircY, bird.y + GIL_SIZE));
    return (DeltaX*DeltaX + DeltaY*DeltaY) < (CIRC_RAD*CIRC_RAD);
}

bool Game::collisionCheck(Bird& bird)
{
  for(auto& tree : list_trees)
  {
    // pnie
    float wspX = bird.x + GIL_SIZE;
    float wspY = bird.y + GIL_SIZE;
    if(tree.x<wspX && tree.x+WIDTH_TREE>bird.x && 0<wspY && tree.y>bird.y)
      return true;
    if(tree.x<wspX && tree.x+WIDTH_TREE>bird.x && tree.height2<wspY && SCREEN_H>bird.y)
      return true;
    // okregi
    float middleX = tree.x+WIDTH_TREE/2;
    float middleY = tree.y+(2*CIRC_RAD)/3;
    bool res = intersects(bird, middleX, middleY);
    if(res) return true;
    middleY = tree.height2-(2*CIRC_RAD)/3;
    res = intersects(bird, middleX, middleY);
    if(res) return true;
  }
  return false;
}

void Game::mainLoop()
{
  bool hit = false;
  int result = 0;
  while(1)
  {
    ALLEGRO_EVENT ev;
    al_wait_for_event(event_queue, &ev);

    if(!list_trees[0].add && gil.x>list_trees[0].x+WIDTH_TREE)  {
      list_trees[0].add = true;
      result++;
    }

    if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
      break;
    }
    else if(ev.type == ALLEGRO_EVENT_TIMER) {
       float decr = SCREEN_H/(FPS*TIME_FALL_BIRD);
       gil.y += decr;
       for(auto& it : list_trees)
         it.x -= MOVE_PIXS_ON_SEC/FPS;
       if(SCREEN_W - list_trees[list_trees.size()-1].x >= DIST_BET_TWO) {
          Tree t(list_trees[list_trees.size()-1].x + DIST_BET_TWO);
          t.drawTree();
          list_trees.push_back(t);
       }
       hit = collisionCheck(gil);
    }
    else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
         gil.y -= BIRD_IMPR;
    }
    else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
      if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
        gil.y -= BIRD_IMPR;
      }
    }
    if(!hit) hit = droppedTooLow(gil);

    if(al_is_event_queue_empty(event_queue)) {
       if(hit) {
          float diff_between_frames = SCREEN_H/(FPS*BIRD_FALL_AFTER_HIT);

          while(gil.y<SCREEN_H) {
            ALLEGRO_EVENT ev;
            al_wait_for_event(event_queue, &ev);
            if(ev.type == ALLEGRO_EVENT_TIMER) {
               al_clear_to_color(al_map_rgb(50,100,150));
               const char* res = this->getTextForResult(result);

               for(int i=0; i<list_trees.size(); i++)
                 list_trees[i].drawTree();
               al_draw_bitmap(gilDead, gil.x, gil.y, 0);
               gil.y += diff_between_frames;

               al_draw_text(font, al_map_rgb(0, 0, 0), 0, 0, 0, res);
               al_flip_display();
            }
          }
          break;
       }
       else {
         if(list_trees[0].x + WIDTH_TREE < 0)
           list_trees.erase(list_trees.begin());

         al_clear_to_color(al_map_rgb(50,100,150));

         const char* res = this->getTextForResult(result);

         al_draw_bitmap(gilAlive, gil.x, gil.y, 0);

         for(int i=0; i<list_trees.size(); i++)
           list_trees[i].drawTree();

         al_draw_text(font, al_map_rgb(0, 0, 0), 0, 0, 0, res);
         al_flip_display();

       }
    }
  }
}
