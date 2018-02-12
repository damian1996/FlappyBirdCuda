#include "NeuralNet.h"

NeuralNet::NeuralNet(){
  for(int i=0; i<wx; i++)
    for(int j=0; j<wy; j++)
      weights[i][j] = 0;
}

NeuralNet::~NeuralNet() {

}

void NeuralNet::randInitialNetworks()
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-4.0, 4.0);
  for(int i=0; i<wx; i++)
  {
    values[i] = 0.0;
    if(i<inpVals)
    {
      for(int j=0; j<wy; j++) {
        weights[i][j] =  dis(gen);
      }
    }
    else if(i>=inpVals && i<wx-1)
    {
      for(int j=0; j<1; j++)
        weights[i][j] =  dis(gen);
    }
  }
}

float NeuralNet::sigmoid(float hor, float hei, float speed) {
  for(int i=0; i<wx; i++) values[i] = 0;
  values[0] = 1/(1 + exp(-hor));
  values[1] = 1/(1 + exp(-hei));
  values[2] = 1/(1 + exp(-speed));
  for(int i=0; i<inpVals; i++){
    for(int j=0; j<wy; j++){
      values[j+inpVals] += weights[i][j]*values[i];
    }
  }
  for(int i=inpVals; i<wx-1; i++) {
    values[i] = 1/(1 + exp(-values[i]));
    values[wx-1] += weights[i][0]*values[i];
  }
  values[wx-1] = 1/(1 + exp(-values[wx-1]));
  return values[wx-1];
}
