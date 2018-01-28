#ifndef BOT_H
#define BOT_H

#include "Train.h"
#include <iostream>

class Bot {
public:
    Train trainForBird;
    NeuralNetwork visibleNets[NUMBER_DISPLAY_BOTS];
    void initBot();
    void computeHiddenRounds();
};

#endif
