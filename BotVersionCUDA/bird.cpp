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

Bird::Bird(const Bird& b) {
  x = b.x;
  y = b.y;
  fit = b.fit;
  acc = b.acc;
  speed = b.speed;
  active = b.active;
  moveUp = b.moveUp;
  net = new NeuralNet();
  for(int i=0; i<wx; i++) {
    for(int j=0; j<wy; j++) {
      net->weights[i][j] = b.net->weights[i][j];
    }
  }
  net->bias = b.net->bias;
}

Bird& Bird::operator=(Bird b)
{
    using std::swap;
    swap(x, b.x);
    swap(y, b.y);
    swap(fit, b.fit);
    swap(acc, b.acc);
    swap(speed, b.speed);
    swap(active, b.active);
    swap(moveUp, b.moveUp);
    net = new NeuralNet();
    for(int i=0; i<wx; i++) {
      for(int j=0; j<wy; j++) {
        swap(net->weights[i][j], b.net->weights[i][j]);
      }
    }
    swap(net->bias, b.net->bias);
    return *this;
}

Bird::~Bird() {
  //delete net;
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
