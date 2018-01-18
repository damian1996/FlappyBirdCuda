#include "Train.h"

Train::Train() {
  //trainNet = malloc(sizeof(NeuralNetwork))
  beasts.resize(10);
  initCuda();
  allocElements();
  initRandom();
}

Train::~Train()
{
  cuMemFree(randomStates);
  cuMemFree(toMutation);
  cuMemFree(fitness);
  cuMemFree(arrOfWeights);
  //free(cross);
  //free(arr);
  //cuMemFreeHost(cross);
  //cuMemFreeHost(arr);
}

void Train::initCuda() {
    cuInit(0);
    CUresult r;
    r = cuDeviceGet(&cuDevice, 0);
    if (r != CUDA_SUCCESS){
        printf("Device get error\n");
        printErrors(r);
    }
    r = cuCtxCreate(&cuContext, 0, cuDevice);
    if (r != CUDA_SUCCESS){
        printf("Ctx create error\n");
        printErrors(r);
    }
    r = cuModuleLoad(&module, "Train.ptx");
    if (r != CUDA_SUCCESS){
        printf("Module load error\n");
        printErrors(r);
    }
    r = cuModuleGetFunction(&trainBirds, module, "TrainBirds");
    if (r != CUDA_SUCCESS){
        printf("trainBirds function error\n");
        printErrors(r);
    }
    r = cuModuleGetFunction(&mutation, module, "Mutation");
    if (r != CUDA_SUCCESS){
        printf("Mutation function error\n");
        printErrors(r);
    }
    r = cuModuleGetFunction(&initR, module, "Init");
    if (r != CUDA_SUCCESS){
        printf("Init function error\n");
        printErrors(r);
    }
}

void Train::allocElements() {
  CUresult r;
  //arr = (float*) malloc(UNITY_IN_TRAINING*sizeNeuralNet*sizeof(float));
  //cross = (float*) malloc(nmbOfCrosses*sizeNeuralNet*sizeof(float));
  r = cuMemAllocHost((void**)&arr, UNITY_IN_TRAINING*sizeNeuralNet*sizeof(float));
  if (r != CUDA_SUCCESS){
      printf("Alloc Weights Host error\n");
      printErrors(r);
  }
  r = cuMemAllocHost((void**)&cross, nmbOfCrosses*sizeNeuralNet*sizeof(float));
  if (r != CUDA_SUCCESS){
      printf("Alloc Cross Host error\n");
      printErrors(r);
  }
  r = cuMemAlloc(&arrOfWeights, UNITY_IN_TRAINING*sizeNeuralNet*sizeof(float));
  if (r != CUDA_SUCCESS){
      printf("Alloc Weights Device error\n");
      printErrors(r);
  }
  r = cuMemAlloc(&fitness, UNITY_IN_TRAINING*sizeof(float));
  if (r != CUDA_SUCCESS){
      printf("Alloc Fitness Device error\n");
      printErrors(r);
  }
  r = cuMemAlloc(&toMutation, nmbOfCrosses*sizeNeuralNet*sizeof(float));
  if (r != CUDA_SUCCESS){
      printf("Alloc Mutation Device error\n");
      printErrors(r);
  }
  r = cuMemAlloc(&randomStates, UNITY_IN_TRAINING*sizeof(curandState));
  if (r != CUDA_SUCCESS){
      printf("Alloc CurandState Device error\n");
      printErrors(r);
  }
}

void Train::initRandom() {
    void* argsI[1] = {&randomStates};
    cuLaunchKernel(initR, blocks_x, 1, 1, threads_block, 1, 1, 0, 0, argsI, 0);
    CUresult r = cuCtxSynchronize();
    if (r != CUDA_SUCCESS){
        printf("Ctx Synchronize error\n");
        printErrors(r);
    }
}

void Train::printErrors(CUresult r) {
  const char* errorName;
  const char* errorDescription;
  cuGetErrorName(r, &errorName);
  cuGetErrorString(r, &errorDescription);
  printf("%s \n", errorName);
  printf("%s \n", errorDescription);
}

std::vector<NeuralNetwork> Train::training(int rounds)
{
    for(int i=0; i<UNITY_IN_TRAINING; i++) // aktualne wagi
    {
        fitCoefficients[i] = 0;
        for(int j=0; j<9; j++)
        {
            for(int k=0; k<6; k++)
            {
              arr[i*sizeNeuralNet+j*edgesForNeuron+k] = trainNet[i].weights[j][k];
            }
        }
    }

    CUresult r;
    for(int l=0; l<HIDDEN_ROUNDS; l++)
    {
        // czy musze za kazdym razem?
        r = cuMemcpyHtoD(arrOfWeights, arr, UNITY_IN_TRAINING*sizeNeuralNet*sizeof(float));
        if (r != CUDA_SUCCESS){
            printf("cuMemcpyHtoD weights error\n");
            printErrors(r);
        }
        r = cuMemcpyHtoD(fitness, fitCoefficients, UNITY_IN_TRAINING*sizeof(float));
        if (r != CUDA_SUCCESS){
            printf("cuMemcpyHtoD fitness error\n");
            printErrors(r);
        }
        void* argsT[3] = {&arrOfWeights, &fitness, &randomStates}; // ewentualnie stale
        cuLaunchKernel(trainBirds, blocks_x, 1, 1, threads_block, 1, 1, 0, 0, argsT, 0);

        r = cuCtxSynchronize();
        if (r != CUDA_SUCCESS){
            printf("Ctx Synchronize error\n");
            printErrors(r);
        }

        r = cuMemcpyDtoH(fitCoefficients, fitness, UNITY_IN_TRAINING*sizeof(float));
        if (r != CUDA_SUCCESS){
            printf("cuMemcpyDtoH fitness error\n");
            printErrors(r);
        }
        std::vector< std::pair<float, int> > coefToSort;
        for(int i=0; i<UNITY_IN_TRAINING; i++) {
            coefToSort.push_back(std::make_pair(fitCoefficients[i], i));
        }

        std::sort(coefToSort.begin(), coefToSort.end());
        beasts.clear();
        for(int j=0, i=UNITY_IN_TRAINING-j-1; j<=9; j++){
          beasts.push_back(trainNet[coefToSort[i].second]);
        }
        crossover(cross, beasts);
        r = cuMemcpyHtoD(toMutation, cross, nmbOfCrosses*sizeNeuralNet*sizeof(float));
        if (r != CUDA_SUCCESS){
            printf("cuMemcpyDtoH mutation error\n");
            printErrors(r);
        }
        void* argsM[3] = {&arrOfWeights, &toMutation, &randomStates}; // ewentualnie stale
        cuLaunchKernel(mutation, blocks_x, 1, 1, threads_block, 1, 1, 0, 0, argsM, 0);

        r = cuCtxSynchronize();
        if (r != CUDA_SUCCESS){
            printf("Ctx Synchronize error\n");
            printErrors(r);
        }

        r = cuMemcpyDtoH(arr, arrOfWeights, UNITY_IN_TRAINING*54*sizeof(float));
        if (r != CUDA_SUCCESS){
            printf("cuMemcpyHtoD weights error\n");
            printErrors(r);
        }
    }
    return beasts;
}

void Train::crossover(float* cross, std::vector<NeuralNetwork> b) {
  // 2X 1x2, 1x3, 1x4, 2X3, 3X4, 2X4, 1x5, 2x5
  for(int i=0, p=nmbOfCrosses-1; i<nmbOfCrosses/2; i++, p--)
  {
    int c1, c2;
    if(i<4) {
      c1 = 1;
      c2 = i+2;
    }
    else if(i<7) {
      c1 = 2;
      c2 = i-1;
    }
    else {
      c1 = 3;
      c2 = 4;
    }
    for(int k=0; k<9; k++) {
      for(int j=0; j<6; j++) {
        float tmp = (b[c1].weights[k][j] + b[c1].weights[k][j])/2;
        cross[i*54 + k*6 + j] = tmp;
        cross[p*54 + k*6 + j] = tmp;
      }
    }
    /*for(int i=0; i<16; i++) {
      for(int k=0; k<9; k++) {
        for(int j=0; j<6; j++) {
          printf("%f ", cross[i*54 + k*6 + j]);
        }
        printf("\n");
      }
      printf("\n");
    }*/
  }
}
