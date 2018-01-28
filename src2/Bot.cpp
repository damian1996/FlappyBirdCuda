#include "Bot.h"
void Bot::initBot() {
  for(int i=0; i<UNITY_IN_TRAINING; i++){
    trainForBird.trainNet[i].randInitialNetworks();
  }
  for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
    visibleNets[i].randInitialNetworks();
}

void Bot::computeHiddenRounds() {
    trainForBird.training(HIDDEN_ROUNDS);
    for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
      for(int i=0; i<9; i++)
        for(int j=0; j<6; j++)
          visibleNets[i].weights[i][j] = trainForBird.beasts[i].weights[i][j];
    }

    /*for(int i=0; i<9; i++) {
      for(int j=0; j<6; j++)
        std::cout << visibleNets[i].weights[i][j] << " " ;
      std::cout << std::endl;
    }
    std::cout << std::endl;  */
}
