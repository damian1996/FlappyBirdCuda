#include "CudaX.h"

CudaX::CudaX() {
    initCuda();
    allocElements();
    initRandom();
}

CudaX::~CudaX() {
  birdsToShow.clear();
  beasts.clear();
  cuMemFree(biasesParallel);
  cuMemFree(randomStates);
  cuMemFree(fitness);
  cuMemFree(arrOfWeights);
  delete [] biases;
  delete [] idForSortFits;
  delete [] fitCoefs;
  for(int i=0; i<UNITY_IN_TRAINING; i++)
    delete trBirds[i];
  delete [] trBirds;
}

void CudaX::initCuda() {
    cuInit(0);
    r = cuDeviceGet(&cuDevice, 0);
    checkErrors(r, "Device get error\n");

    r = cuCtxCreate(&cuContext, 0, cuDevice);
    checkErrors(r, "Ctx create error\n");

    r = cuModuleLoad(&module, "CudaX.ptx");
    checkErrors(r, "Module load error\n");

    r = cuModuleGetFunction(&trainBirds, module, "TrainBirds");
    checkErrors(r, "trainBirds function error\n");

    r = cuModuleGetFunction(&mutation, module, "Mutation");
    checkErrors(r, "Mutation function error\n");

    r = cuModuleGetFunction(&initR, module, "Init");
    checkErrors(r, "Init function error\n");
}

void CudaX::allocElements() {
    trBirds = new Bird*[UNITY_IN_TRAINING];
    for(int i=0; i<UNITY_IN_TRAINING; i++)
      trBirds[i] = new Bird();
    fitCoefs = new float[UNITY_IN_TRAINING];
    idForSortFits = new int[UNITY_IN_TRAINING];
    biases = new float[UNITY_IN_TRAINING];
    r = cuMemAllocHost((void**)&arr, UNITY_IN_TRAINING*WEIGHTS_SIZE*sizeof(float));
    checkErrors(r, "Alloc Weights Host error\n");

    r = cuMemAlloc(&arrOfWeights, UNITY_IN_TRAINING*WEIGHTS_SIZE*sizeof(float));
    checkErrors(r, "Alloc Weights Device error\n");

    r = cuMemAlloc(&fitness, UNITY_IN_TRAINING*sizeof(float));
    checkErrors(r, "Alloc Fitness Device error\n");

    r = cuMemAlloc(&randomStates, UNITY_IN_TRAINING*sizeof(curandState));
    checkErrors(r, "Alloc CurandState Device error\n");

    r = cuMemAlloc(&biasesParallel, UNITY_IN_TRAINING*sizeof(float));
    checkErrors(r, "Alloc Biases Device error\n");
}

void CudaX::initRandom() {
    void* argsI[1] = {&randomStates};
    cuLaunchKernel(initR, blocks_x, 1, 1, threads_block, 1, 1, 0, 0, argsI, 0);
    r = cuCtxSynchronize();
    checkErrors(r, "Ctx Synchronize error\n");
}

void CudaX::printErrors(CUresult r) {
  const char* errorName;
  const char* errorDescription;
  cuGetErrorName(r, &errorName);
  cuGetErrorString(r, &errorDescription);
  printf("%s \n", errorName);
  printf("%s \n", errorDescription);
}

void CudaX::checkErrors(CUresult r, string msg) {
  if (r != CUDA_SUCCESS){
      printf("%s", msg);
      printErrors(r);
  }
}

void CudaX::firstRewrite() {
  int ofSet = 0;
  for(int i=0; i<UNITY_IN_TRAINING; i++) {
    fitCoefs[i] = 0;
    ofSet = i*WEIGHTS_SIZE;
    for(int j=0; j<wx; j++)
      for(int k=0; k<wy; k++)
        arr[ofSet + j*wy + k] = trBirds[i]->net->weights[j][k];
    biases[i] = 0.5;
  }
  for(int i=0; i<WINNERS; i++)
    beasts.push_back(trBirds[i]);
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
    birdsToShow.push_back(trBirds[i]);
  trainPart++;
}

void CudaX::fitnessKernel() {
  r = cuMemcpyHtoD(arrOfWeights, arr, UNITY_IN_TRAINING*WEIGHTS_SIZE*sizeof(float));
  checkErrors(r, "cuMemcpyHtoD 1 weights error\n");

  r = cuMemcpyHtoD(fitness, fitCoefs, UNITY_IN_TRAINING*sizeof(float));
  checkErrors(r, "cuMemcpyHtoD 1 fitness error\n");

  r = cuMemcpyHtoD(biasesParallel, biases, UNITY_IN_TRAINING*sizeof(float));
  checkErrors(r, "cuMemcpyHtoD 1 biases error\n");

  void* argsT[4] = {&arrOfWeights, &biasesParallel, &fitness, &randomStates};

  r = cuLaunchKernel(trainBirds, blocks_x, 1, 1, threads_block, 1, 1, 0, 0, argsT, 0);
  checkErrors(r, "TrainBirds kernel error\n");

  r = cuCtxSynchronize();
  checkErrors(r, "Ctx Synchronize 2 error\n");

  r = cuMemcpyDtoH(fitCoefs, fitness, UNITY_IN_TRAINING*sizeof(float));
  checkErrors(r, "cuMemcpyDtoH fitness 2 error\n");
}

void CudaX::chooseBestFitnesses(int winners, vector<Bird*>& vec) {
    std::vector< std::pair<float, int> > coefToSort;
    for(int i=0; i<UNITY_IN_TRAINING; i++) {
        coefToSort.push_back(std::make_pair(fitCoefs[i], i));
    }
    std::sort(coefToSort.begin(), coefToSort.end(), std::greater<pair<float, int>>());
    int maxi = 1;
    for(int i=1; i<UNITY_IN_TRAINING; i++)
    {
      if(coefToSort[i].first == coefToSort[0].first)
        maxi++;
    }

    for(int j=0; j<winners; j++) {
        int offset = coefToSort[j].second * WEIGHTS_SIZE;
        for(int l=0; l<wx; l++) {
            for(int k=0; k<wy; k++) {
                vec[j]->net->weights[l][k] = arr[offset + l*wy + k];
            }
        }
        vec[j]->net->bias = biases[coefToSort[j].second];
    }
}

void CudaX::mutationKernel() {
  r = cuMemcpyHtoD(arrOfWeights, arr, UNITY_IN_TRAINING*WEIGHTS_SIZE*sizeof(float));
  checkErrors(r, "cuMemcpyHtoD 1 weights error\n");

  r = cuMemcpyHtoD(biasesParallel, biases, UNITY_IN_TRAINING*sizeof(float));
  checkErrors(r, "cuMemcpyHtoD 1 biases error\n");

  void* argsM[3] = {&arrOfWeights, &biasesParallel, &randomStates};
  r = cuLaunchKernel(mutation, blocks_x, 1, 1, threads_block, 1, 1, 0, 0, argsM, 0);
  checkErrors(r, "mutation kernel error\n");

  r = cuCtxSynchronize();
  checkErrors(r, "Ctx Synchronize 3 error\n");

  r = cuMemcpyDtoH(biases, biasesParallel, UNITY_IN_TRAINING*sizeof(float));
  checkErrors(r, "cuMemcpyDtoH 1 biases error\n");

  r = cuMemcpyDtoH(arr, arrOfWeights, UNITY_IN_TRAINING*WEIGHTS_SIZE*sizeof(float));
  checkErrors(r, "cuMemcpyDtoH 1 weights error\n");
}

std::vector<Bird*> CudaX::hiddenRounds() {
    for(int l=0; l<HIDDEN_ROUNDS; l++)
    {
        fitnessKernel();
        chooseBestFitnesses(WINNERS, beasts);
        crossover();
        mutationKernel();
        /*float *tempFitness = new float[UNITY_IN_TRAINING];
        for(int i=0; i<UNITY_IN_TRAINING; i++)
            tempFitness[i] = 0;
        for(int i=0; i<5; i++) {
            fitnessKernel();
            for(int i=0; i<UNITY_IN_TRAINING; i++) {
                tempFitness[i] += fitCoefs[i];
            }
        }
        for(int i=0; i<UNITY_IN_TRAINING; i++)
            fitCoefs[i] = tempFitness[i]/5;
        delete [] tempFitness;*/
    }
    fitnessKernel();
    chooseBestFitnesses(NUMBER_DISPLAY_BOTS, birdsToShow);
    return birdsToShow;
}

void CudaX::crossover() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> disInt(0,2);
  Bird** newgil = new Bird*[CROSSOVER_BIRDS];
  for(int i=0; i<CROSSOVER_BIRDS; i++)
    newgil[i] = new Bird();
  for(int k = 0; k < WINNERS; ++k) {
    *newgil[k] = *beasts[k];
  }
  int w1, w2;

  for(int i=0; i<wx; i++) {
    for(int j=0; j<wy; j++) {
       newgil[3]->net->weights[i][j] = (newgil[0]->net->weights[i][j] + newgil[1]->net->weights[i][j])/2;
    }
  }
  newgil[3]->net->bias = (newgil[0]->net->bias + newgil[1]->net->bias)/2;

  for(int k = 0; k < 2; ++k) {
    w1 = disInt(gen);
    while(true) {
      w2 = disInt(gen);
      if(w1!=w2) break;
    }
    for(int i=0; i<wx; i++) {
      for(int j=0; j<wy; j++) {
        newgil[4+k]->net->weights[i][j] = (newgil[w1]->net->weights[i][j] + newgil[w2]->net->weights[i][j])/2;
      }
    }
    newgil[4+k]->net->bias = (newgil[w1]->net->bias + newgil[w2]->net->bias)/2;
  }

  for(int k = 0; k < 2; ++k) {
    w1 = disInt(gen);
    for(int i=0; i<wx; i++) {
      for(int j=0; j<wy; j++) {
        std::uniform_real_distribution<> dis(-4, 4);
        newgil[6+k]->net->weights[i][j] = dis(gen);
      }
    }
    newgil[6+k]->net->bias = newgil[w1]->net->bias;
  }

  for(int z=4; z<CROSSOVER_BIRDS; z++) {
      for(int i=0; i<wx; i++) {
          for(int j=0; j<wy; j++) {
              std::uniform_real_distribution<> dis(newgil[z]->net->weights[i][j]-EPS, newgil[z]->net->weights[i][j]+EPS);
              newgil[z]->net->weights[i][j] = dis(gen);
          }
      }
      std::uniform_real_distribution<> dis(newgil[z]->net->bias-0.05, newgil[z]->net->bias+0.05);
      newgil[z]->net->bias = dis(gen);
  }

  for(int k = 0; k < CROSSOVER_BIRDS; ++k) {
      int offset = k*(UNITY_IN_TRAINING/CROSSOVER_BIRDS)*WEIGHTS_SIZE;
      int biasOffset = k*(UNITY_IN_TRAINING/CROSSOVER_BIRDS);
      for(int i=0; i<wx; i++) {
          for(int j=0; j<wy; j++) {
              arr[offset + i*wy + j] = newgil[k]->net->weights[i][j];
          }
      }
      biases[biasOffset] = newgil[k]->net->bias;
  }
  for(int i=0; i<CROSSOVER_BIRDS; i++)
    delete newgil[i];
  delete [] newgil;
}
