#ifndef CUDAX_H
#define CUDAX_H

#include <cstdio>
#include <string>
#include "consts.h"
#include <algorithm>
#include <curand_kernel.h>
#include "cuda.h"
#include <cstdlib>
#include "bird.h"
using namespace std;

class CudaX {
public:
  int threads_block = 1024, blocks_x = (UNITY_IN_TRAINING+1023)/threads_block;
  int trainPart = 0;
  CUdevice cuDevice;
  CUcontext cuContext;
  CUmodule module;
  CUfunction trainBirds, mutation, initR;
  CUdeviceptr fitness, arrOfWeights, randomStates, biasesParallel;
  CUresult r;
  vector<Bird*> beasts, birdsToShow;
  float *biases;
  float *arr;
  Bird **trBirds;
  float *fitCoefs;
  int *idForSortFits;


  CudaX();
  ~CudaX();
  void initCuda();
  void allocElements();
  void initRandom();
  void printErrors(CUresult r);
  void checkErrors(CUresult r, std::string msg);
  void firstRewrite();
  void fitnessKernel();
  void chooseBestFitnesses(int winners, vector<Bird*>& vec);
  void mutationKernel();
  std::vector<Bird*> hiddenRounds();
  void crossover();
};


#endif
