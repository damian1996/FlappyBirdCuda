#ifndef TREE_H
#define TREE_H

#include "consts.h"
#include <cstdlib>

class Tree {
public:
  float x, y1, y2;
  bool scored_tree;
  Tree(int x, int id) : x(x){
    y1 = MIN_HEIGHT_FIRST_TREE + (rand())%RANGE_RAND_DIST_TREES;
    y2 = y1 + DIFF_BETWEEN_H;
    scored_tree = false;
  }
};

#endif
