#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <vector>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include "consts.h"
#include "bird.h"
#include <random>

class NeuralNetwork {
public:
   float bias = 0.5;
   float weights[9][6];
   int travelledTicks = 0;
   float values[9];
   float distAfterTick = MOVE_PIXS_ON_SEC/FPS;
   NeuralNetwork();
   NeuralNetwork operator=(const NeuralNetwork&);
   void randInitialNetworks();
   float sigmoid(float inp1, float inp2);
};

#endif
