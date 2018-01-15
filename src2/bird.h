#ifndef BIRD_H
#define BIRD_H

class Bird {
public:
  float x, y;
  bool active, moveUp;

  Bird() {
    x = 100.0;
    y = (float) SCREEN_H/2;
    active = true;
    moveUp = false;
  }
  void backInitialValues() {
    x = 100.0;
    y = (float) SCREEN_H/2;
    active = true;
    moveUp = false;
  }
};

#endif
