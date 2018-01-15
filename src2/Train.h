#ifndef TRAIN_H
#define TRAIN_H

#include <cstdio>
#include "NeuralNetwork.h"
#include "consts.h"
#include <algorithm>

class Train {
public:
  NeuralNetwork trainNet[UNITY_IN_TRAINING];
  float fitCoefficients[UNITY_IN_TRAINING];
  int idForSortFits[UNITY_IN_TRAINING];
  std::vector<NeuralNetwork> training(int i);
  float fitness_function();
  float crossover();
  void mutation();
};

#endif
