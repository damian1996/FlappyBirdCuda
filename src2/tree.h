#ifndef TREE_H
#define TREE_H

class Tree {
public:
  float x, y, height2;
  bool add;
  Tree(int coX) : x(coX){
    add = false;
  }
  void heights() {
    y = 100.0 + rand()%150;
    height2 = y + DIFF_BETWEEN_H;
  }
  Tree copyTree() {
    Tree tmp(x);
    tmp.y = y;
    tmp.height2 = height2;
    return tmp;
  }
};

#endif
