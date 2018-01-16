#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork(){
  for(int i=0; i<9; i++)
    for(int j=0; j<6; j++)
      weights[i][j] = 0;
}

NeuralNetwork NeuralNetwork::operator=(const NeuralNetwork& nn){
    for(int i=0; i<9; i++)
      for(int j=0; j<6; j++)
        weights[i][j] = nn.weights[i][j];
}

void NeuralNetwork::randInitialNetworks()
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-1.0, 1.0);
  for(int i=0; i<9; i++)
  {
    values[i] = 0.0;
    if(i<2)
    {
      for(int j=0; j<6; j++) {
        weights[i][j] =  dis(gen);
      }
    }
    else if(i>=2 && i<8)
    {
      for(int j=0; j<1; j++)
        weights[i][j] =  dis(gen);

    }
  }
}

float NeuralNetwork::sigmoid(float hor, float hei) {
  for(int i=0; i<9; i++) values[i] = 0;
  values[0] = 1/(1 + exp(-hor-bias));
  values[1] = 1/(1 + exp(-hei-bias));
  for(int i=0; i<2; i++){
    for(int j=0; j<6; j++){
      values[j+2] += weights[i][j]*values[i];
    }
  }
  for(int i=2; i<8; i++) {
    values[i] = 1/(1 + exp(-values[i]-bias));
    values[8] += weights[i][0]*values[i];
  }
  values[8] = 1/(1 + exp(-values[8]-bias));
  return values[8];
}
