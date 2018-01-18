#ifndef TRAIN_H
#define TRAIN_H

#include <cstdio>
#include "NeuralNetwork.h"
#include "consts.h"
#include <algorithm>
#include <curand_kernel.h>
#include "cuda.h"

class Train {
public:
  int nmbOfCrosses = 16, sizeNeuralNet = 54;
  int numberOfNeurons = 9, edgesForNeuron = 6;
  int threads_block = 1024, blocks_x = (UNITY_IN_TRAINING+1023)/threads_block;
  CUdevice cuDevice;
  CUcontext cuContext;
  CUmodule module;
  CUfunction trainBirds, mutation, initR;
  CUdeviceptr fitness, arrOfWeights, toMutation, randomStates;
  std::vector<NeuralNetwork> beasts;
  float *arr, *cross;
  // potencjalnie zrobic to dynamicznie
  NeuralNetwork trainNet[UNITY_IN_TRAINING];
  float fitCoefficients[UNITY_IN_TRAINING];
  int idForSortFits[UNITY_IN_TRAINING];

  Train();
  ~Train();
  void initCuda();
  void allocElements();
  void initRandom();
  void printErrors(CUresult r);
  std::vector<NeuralNetwork> training(int i);
  void crossover(float* arr, std::vector<NeuralNetwork> b);
};

#endif
