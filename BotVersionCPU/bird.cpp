#include "bird.h"

Bird::Bird() {
  x = INITIAL_POSITION_BIRD_X;
  y = (float) SCREEN_H/2;
  fit = 0.0f;
  acc = 550;
  speed = 0.0f;
  ticks = 0;
  active = true;
  moveUp = false;
  net = new NeuralNet();
  net->randInitialNetworks();
}
Bird::~Bird() {
  delete net;
}
void Bird::backInitialValues() {
  x = INITIAL_POSITION_BIRD_X;
  y = (float) SCREEN_H/2;
  fit = 0.0f;
  acc = 550;
  speed = 0.0f;
  ticks = 0;
  active = true;
  moveUp = false;
}
