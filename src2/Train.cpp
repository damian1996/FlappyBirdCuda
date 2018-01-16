#include "Train.h"
#include "cuda.h"

std::vector<NeuralNetwork> Train::training(int rounds)
{
    cuInit(0);

    CUresult r;
    CUdevice cuDevice;
    cuDeviceGet(&cuDevice, 0);
    CUcontext cuContext;
    cuCtxCreate(&cuContext, 0, cuDevice);
    CUmodule module;
    cuModuleLoad(&module, "Train.ptx");
    CUfunction trainBirds, mutation;
    cuModuleGetFunction(&trainBirds, module, "TrainBirds");
    cuModuleGetFunction(&mutation, module, "Mutation");

    int threads_block = 1024, blocks_x = UNITY_IN_TRAINING/threads_block;
    int sizeNeuralNet = 54, numberOfNeurons = 9, edgesForNeuron = 6;
    float *arr; // 65kx9x6
    std::vector<NeuralNetwork> beasts;

    CUdeviceptr arrOfWeights, fitness; // ew stale
    cuMemAllocHost((void**)&arr, UNITY_IN_TRAINING*54*sizeof(float));
    cuMemAlloc(&arrOfWeights, UNITY_IN_TRAINING*54*sizeof(float));
    cuMemAlloc(&fitness, UNITY_IN_TRAINING*sizeof(float));


    for(int i=0; i<UNITY_IN_TRAINING; i++) // aktualne wagi
    {
        for(int j=0; j<9; j++)
        {
            for(int k=0; k<6; k++)
            {
              arr[i*sizeNeuralNet+j*edgesForNeuron+k] = trainNet[i].weights[j][k];
            }
        }
    }
    cuMemcpyHtoD(arrOfWeights, arr, UNITY_IN_TRAINING*54*sizeof(float));
    cuMemcpyHtoD(fitness, fitCoefficients, UNITY_IN_TRAINING*sizeof(float));

    for(int l=0; l<HIDDEN_ROUNDS; l++)
    {

        void* argsT[4] = {&arrOfWeights, &fitness}; // ewentualnie stale
        cuLaunchKernel(trainBirds, UNITY_IN_TRAINING, 1, 1, 1024, 1, 1, 0, 0, argsT, 0);

        cuMemcpyDtoH(fitCoefficients, fitness, UNITY_IN_TRAINING*sizeof(float));

        std::vector< std::pair<float, int> > coefToSort;
        for(int i=0; i<UNITY_IN_TRAINING; i++) {
            coefToSort.push_back(std::make_pair(fitCoefficients[i], i));
        }
        std::sort(coefToSort.begin(), coefToSort.end());
        beasts.clear();
        for(int j=0, i=UNITY_IN_TRAINING-j; j<=9; j++)
          beasts.push_back(trainNet[coefToSort[i].second]);

        //void* argsM[4] = {&arrOfWeights, &fitness}; // ewentualnie stale
        //cuLaunchKernel(mutation, UNITY_IN_TRAINING, 1, 1, 1024, 1, 1, 0, 0, argsM, 0);
    }

    return beasts;
}
//cuMemcpyDtoH(arr, arrOfWeights, UNITY_IN_TRAINING*54*sizeof(float));

float Train::crossover() {
  return 0.0f;
}
