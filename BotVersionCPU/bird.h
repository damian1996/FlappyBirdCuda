#ifndef BIRD_H
#define BIRD_H

#include "NeuralNet.h"

class Bird {
public:
  float x, y, fit, acc, speed;
  int ticks;
  bool active, moveUp;
  NeuralNet *net;

  Bird();
  ~Bird();
  void backInitialValues();
};

#endif
