#ifndef CONSTS_H
#define CONSTS_H

const float FPS = 60.0;
const float SCREEN_W = 1000.0;
const float SCREEN_H = 600.0;
const float GIL_SIZE = 45.0;
const float CIRC_RAD = 40.0;
const float WIDTH_TREE = 50.0;
const float DIST_BET_TWO = 265.0;
const float DIFF_BETWEEN_H = 265.0;
const int TREES_INITIALLY = 3;
const float TIME_FALL_BIRD = 4.0;
const float BIRD_IMPR = 50.0/FPS;
const float MOVE_PIXS_ON_SEC = 100.0;
const float BIRD_FALL_AFTER_HIT = 5.0;
const int NUMBER_DISPLAY_BOTS = 10; // 10
const int NUMBER_HIDDEN_ROUNDS = 20;
const int SHOW_MOVEMENT = 0;
const int HIDDEN_BOT_TRAIN = 1;
const int END_OF_COMPUTATIONS = 2;
const int UNITY_IN_TRAINING = 1024;
const int HIDDEN_ROUNDS = 20;
const float EPS = 0.1;
const float FIRST_TREE = 400.0;
const float SECOND_TREE = FIRST_TREE + DIST_BET_TWO;
const float THIRD_TREE = SECOND_TREE + DIST_BET_TWO;
const int WEIGHTS_SIZE = 54;
const int NMB_CROSSES = 16;

#endif
