#include "Bot.h"

void Bot::initBot() {
  for(int i=0; i<UNITY_IN_TRAINING; i++){
    trainForBird.trainNet[i].randInitialNetworks();
  }
}

void Bot::computeHiddenRounds() {
    std::vector<NeuralNetwork> res = trainForBird.training(HIDDEN_ROUNDS);
    for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
      visibleNets[i] = res[i]; // operator = dla NeuralNetwork?
}
