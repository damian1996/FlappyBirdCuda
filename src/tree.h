#ifndef TREE_H
#define TREE_H

struct Tree {
  float x, y;
  float height2;
  Tree(int xd) : x(xd){
    y = 100.0 + rand()%100;
    height2 = 100.0 + rand()%100;
  }
  void drawTree()
  {
    al_draw_filled_rectangle(x, y, x+WIDTH_TREE, 0, al_color_name("brown"));
    al_draw_filled_rectangle(x, SCREEN_H, x+WIDTH_TREE, SCREEN_H-height2, al_color_name("brown"));
    al_draw_filled_circle(x+WIDTH_TREE/2, y+(2*CIRC_RAD)/3, CIRC_RAD, al_color_name("green"));
    al_draw_filled_circle(x+WIDTH_TREE/2, SCREEN_H-height2-(2*CIRC_RAD)/3, CIRC_RAD, al_color_name("green"));
  }
};

#endif
