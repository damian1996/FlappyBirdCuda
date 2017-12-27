#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include "consts.h"
#include "bird.h"
#include "tree.h"
using namespace std;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *gilAlive = NULL;
ALLEGRO_BITMAP *gilDead = NULL;
vector<Tree> list_trees;

void initAllegro()
{
  if(!al_init()) {
     fprintf(stderr, "failed to initialize allegro!\n");
     exit(1);
  }

  if(!al_init_image_addon()) {
     al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_image_addon!",
                                NULL, ALLEGRO_MESSAGEBOX_ERROR);
     exit(1);
  }
  if(!al_init_primitives_addon()) {
     al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_primitives_addon!",
                                NULL, ALLEGRO_MESSAGEBOX_ERROR);
     exit(1);
  }

  if(!al_install_mouse()) {
     fprintf(stderr, "failed to initialize the mouse!\n");
     exit(1);
  }
}

void initGame()
{
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

void reallocAll()
{
  al_destroy_bitmap(gilDead);
  al_destroy_bitmap(gilAlive);
  al_destroy_timer(timer);
  al_destroy_display(display);
  al_destroy_event_queue(event_queue);
}

int main(int argc, char **argv)
{
   srand(time(NULL));

   initAllegro();

   initGame();

   Bird gil = {100.0, (float) SCREEN_H/2};
   al_draw_bitmap(gilAlive, gil.x, gil.y, 0);

   for(int i=0; i<TREES_INITIALLY; i++)
   {
     Tree t(xes[i]);
     t.drawTree();
     list_trees.push_back(t);
   }

   event_queue = al_create_event_queue();
   if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_bitmap(gilDead);
      al_destroy_bitmap(gilAlive);
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
   }

   al_register_event_source(event_queue, al_get_display_event_source(display));

   al_register_event_source(event_queue, al_get_timer_event_source(timer));

   al_register_event_source(event_queue, al_get_mouse_event_source());

   al_flip_display();

   al_start_timer(timer);

   while(1)
   {
     ALLEGRO_EVENT ev;
     al_wait_for_event(event_queue, &ev);
     int64_t currTime = al_get_timer_count(timer);
     if(ev.type == ALLEGRO_EVENT_TIMER) {
        float decr = SCREEN_H/(FPS*TIME_FALL_BIRD);
        gil.y += decr;
        for(auto& it : list_trees)
          it.x -= MOVE_PIXS_ON_SEC/FPS;
        if(SCREEN_W - list_trees[list_trees.size()-1].x >= DIST_BET_TWO) {
           Tree t(list_trees[list_trees.size()-1].x + DIST_BET_TWO);
           t.drawTree();
           list_trees.push_back(t);
        }
     }
     else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
       break;
     }
     else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
          gil.y -= BIRD_IMPR;
          for(auto& it : list_trees)
            it.x -= MOVE_PIXS_ON_SEC/FPS;
          if(SCREEN_W - list_trees[list_trees.size()-1].x >= DIST_BET_TWO) {
             Tree t(list_trees[list_trees.size()-1].x + DIST_BET_TWO);
             t.drawTree();
             list_trees.push_back(t);
          }
     }

     if(al_is_event_queue_empty(event_queue)) {
        al_clear_to_color(al_map_rgb(50,100,150));

        al_draw_bitmap(gilAlive, gil.x, gil.y, 0);

        for(int i=0; i<list_trees.size(); i++)
          list_trees[i].drawTree();

        al_flip_display();
     }
   }

   reallocAll();

   return 0;
}
