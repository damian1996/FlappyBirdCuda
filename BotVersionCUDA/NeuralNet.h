#ifndef NEURALNET_H
#define NEURALNET_H

#include <vector>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include "consts.h"
#include <random>

class NeuralNet {
public:
   float bias = 0.5;
   float weights[wx+1][wy+1];
   int travelledTicks = 0;
   float values[wx];
   float distAfterTick = MOVE_PIXS_ON_SEC/FPS;

   NeuralNet();
   ~NeuralNet();
   void randInitialNetworks();
   float sigmoid(float inp1, float inp2, float inp3);
};

#endif
