#include "consts.h"
#include <curand_kernel.h>
#include <cmath>

extern "C" {
	__global__
	void Init(curandState *states) {
		int thid = blockIdx.x*blockDim.x+threadIdx.x;
		curand_init(clock64(), thid, 0, &states[thid]);
		__syncthreads();
	}
}

extern "C" {
	__device__
	void randInt(float& toRand, int interval, curandState* state) {
				int tmp = curand(state);
				if(tmp<0) tmp = -tmp;
				toRand = (tmp)%interval;
	 }
}

extern "C" {
	__device__
	void randFloat(float& toRand, float l, float r, curandState* state) {
				int rand_max = 32767;
				float randomVarb;
				randInt(randomVarb, rand_max, state);
				toRand =  ((r - l) * (randomVarb / rand_max)) + l;
	 }
}

struct Bird {
public:
  float x, y, fit, acc, speed;
  bool active, moveUp;
	__device__
  Bird() {
    x = INITIAL_POSITION_BIRD_X;
    y = (float) SCREEN_H/2;
		fit = 0;
		speed = 0;
		acc = 550;
    active = true;
    moveUp = false;
  }
	__device__
	void backInitialValues() {
		x = INITIAL_POSITION_BIRD_X;
		y = (float) SCREEN_H/2;
		fit = 0;
		speed = 0;
		acc = 550;
		active = true;
		moveUp = false;
  }
};

struct Tree {
public:
  float x, h1, h2;
  bool scored_tree;
	__device__
	Tree() {}
  Tree(int x) : x(x){

  }
	__device__
  void heights(curandState* state) {
		h1 = 0.0;
		randInt(h1, RANGE_RAND_DIST_TREES, state);
    h1 = h1 + MIN_HEIGHT_FIRST_TREE;
    h2 = h1 + DIFF_BETWEEN_H;
		scored_tree = false;
  }
};

struct List {
public:
	Tree trees[6];
	int size;
	__device__
	List() {
		size = 0;
	}
	__device__
	void addTree(Tree t) {
		trees[size].x = t.x;
		trees[size].h1 = t.h1;
		trees[size].h2 = t.h2;
		trees[size++].scored_tree = t.scored_tree;
	}
	__device__
	void removeFromStart() {
		for(int i=1; i<size; i++) {
			trees[i-1].x = trees[i].x;
			trees[i-1].h1 = trees[i].h1;
			trees[i-1].h2 = trees[i].h2;
			trees[i-1].scored_tree = trees[i].scored_tree;
		}
		size--;
	}
};

extern "C" {
	__device__
	void createTree(List& list, float i, curandState* state) {
		{
			int thid = blockIdx.x*blockDim.x+threadIdx.x;
			Tree t;
			t.x = i;
		  t.heights(state);
		  list.addTree(t);
		}
	}
}

extern "C" {
	__device__
		void treeToRemove(List& list, Bird& b, int gameScore)
		{
        if(!list.trees[0].scored_tree && b.x>(list.trees[0].x+WIDTH_TREE))
        {
           list.trees[0].scored_tree = true;
           gameScore++;
        }
		}
}

extern "C" {
	__device__
	void update(Bird& b) {
		  float t = 1.0/FPS;
		  if(b.moveUp) {
		    	b.y += b.speed*t;
		   		b.speed = -180;
		  } else {
		    	b.y += b.speed*t;
		   		b.speed += b.acc*t;
		  }
	}
}

extern "C" {
	__device__
		void sigmoid(float* weights, float hor, float hei, float speed, float* res) {
			int thid = blockIdx.x*blockDim.x+threadIdx.x;
			float values[wx];
			int offset = thid*WEIGHTS_SIZE;
			for(int i=0; i<wx; i++) values[i] = 0;
			values[0] = 1/(1 + exp(-hor));
		  values[1] = 1/(1 + exp(-hei));
			values[2] = 1/(1 + exp(-speed));
			for(int i=0; i<inpVals; i++) {
		    for(int j=0; j<wy; j++) {
		      values[j+inpVals] += (weights[offset + i*6 + j]*values[i]);
		    }
		  }
			for(int i=inpVals; i<wx-1; i++) {
				values[i] = 1/(1 + exp(-values[i]));
				values[wx-1] +=  weights[offset+i*wy+0]*values[i];
			}
			values[wx-1] = 1/(1 + exp(-values[wx-1]));
			*res = values[wx-1];
		}
}

extern "C" {
	__device__
	void intersects(Bird& b, float CircX, float CircY, bool& r) {
	  float DeltaX = CircX - max(b.x, min(CircX, b.x + GIL_SIZE));
	  float DeltaY = CircY - max(b.y, min(CircY, b.y + GIL_SIZE));
		r = (DeltaX*DeltaX + DeltaY*DeltaY) < (CIRC_RAD*CIRC_RAD);
	}
}

extern "C" {
	__device__
		void collisionCheck(List& list, Bird& b) {
				bool res;
				int siz = list.size;
				for(int i=0; i<siz; i++) {
			      float wspX = b.x + GIL_SIZE;
			      float wspY = b.y + GIL_SIZE;
			      if(list.trees[i].x<wspX && list.trees[i].x+WIDTH_TREE>b.x && list.trees[i].h1>b.y) {
								b.active = false;
								return;
						}
			      if(list.trees[i].x<wspX && list.trees[i].x+WIDTH_TREE>b.x && list.trees[i].h2<wspY) {
								b.active = false;
								return;
						}

			      float middleX = list.trees[i].x+WIDTH_TREE/2;
			      float middleY = list.trees[i].h1+(2*CIRC_RAD)/3;
						intersects(b, middleX, middleY, res);
			      if(res) {
								b.active = false;
								return;
						}
						middleY = list.trees[i].h2-(2*CIRC_RAD)/3;
			      intersects(b, middleX, middleY, res);
			      if(res) {
								b.active = false;
								return;
						}
		    }
		    b.active = true;
		}
}

extern "C" {
	__device__
	void droppedTooLow(Bird& bird) {
	  if(bird.y>=SCREEN_H) bird.active = false;
	  else bird.active = true;
	}
}

extern "C" {
	__global__
   	void TrainBirds(float* weights, float* biases, float* fitness, curandState* states) {
			int thid = blockIdx.x*blockDim.x+threadIdx.x;
			int gameScore = 0;
			List list;
			createTree(list, FIRST_TREE, &states[thid]);
			createTree(list, SECOND_TREE, &states[thid]);
			createTree(list, THIRD_TREE, &states[thid]);
			Bird bird;
			while(1) {
				treeToRemove(list, bird, gameScore);
				for(int i=0; i<list.size; i++)
					list.trees[i].x -= MOVE_PIXS_ON_SEC/FPS;

				if(list.trees[0].x + WIDTH_TREE < 0) {
					list.removeFromStart();
				}

				if(SCREEN_W - list.trees[list.size-1].x >= DIST_BET_TWO) {
					 createTree(list, list.trees[list.size-1].x + DIST_BET_TWO, &states[thid]);
				}

				if(bird.active) {
						int idx = ((list.trees[0].x + WIDTH_TREE) > bird.x) ? 0 : 1;
						float horizontal = list.trees[idx].x + WIDTH_TREE - bird.x;
						float heighDiff = ((list.trees[idx].h1 + list.trees[idx].h2)/2) - bird.y;
						float sp = bird.speed;
						float val = 0.0;
					  sigmoid(weights, horizontal, heighDiff, sp, &val);
						if(val>biases[thid]) bird.moveUp = true;
						else bird.moveUp = false;
						update(bird);
						collisionCheck(list, bird);
				}
				if(bird.active) {
					droppedTooLow(bird);
				}

				if(!bird.active) {
					int idx = list.trees[0].x+WIDTH_TREE > bird.x ? 0 : 1;
					float heighDiff = ((list.trees[idx].h1 + list.trees[idx].h2)/2) - bird.y;
					if(heighDiff<0) heighDiff = -heighDiff;
					bird.fit += -heighDiff;
					break;
				} else {
					float end = (MOVE_PIXS_ON_SEC/FPS)*250;
					bird.fit += MOVE_PIXS_ON_SEC/FPS;
					if(bird.fit > end) break;
				}
			}
			fitness[thid] = bird.fit;
			__syncthreads();
	 }
}

extern "C" {
	__global__
	void Mutation(float* weights, float* biases, curandState *states) {
		int thid = blockDim.x*blockIdx.x + threadIdx.x;
		int offset = thid*WEIGHTS_SIZE;
		int divider = UNITY_IN_TRAINING/CROSSOVER_BIRDS;
		int block = thid/divider;
		int main_id = block*divider;
		if((thid%divider)>0) {
				for(int i=0; i<wx; i++) {
						for(int j=0; j<wy; j++) {
								float tmp = 0;
								float prev = weights[main_id*WEIGHTS_SIZE + i*wy + j];
								randFloat(tmp, prev-EPS, prev+EPS, &states[thid]);
								weights[offset + i*wy + j] = tmp;
						}
				}
				float temp;
				randFloat(temp, biases[main_id]-0.05, biases[main_id]+0.05, &states[thid]);
				biases[thid] = temp;
		}
		__syncthreads();
	}
}
