#include "consts.h"
#include <thrust/device_vector.h>
#include <vector>
#include <curand_kernel.h>
// https://stackoverflow.com/questions/8012304/why-defining-class-headers-without-cuda-device-attribute-works-c

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
				int thid = blockIdx.x*blockDim.x+threadIdx.x;
				int tmp = curand(state);// %interval;
				if(tmp<0) tmp = -tmp;
				toRand = (tmp)%interval;
	 }
}

extern "C" {
	__device__
	void randFloat(float& toRand, float l, float r, curandState* state) {
				int thid = blockIdx.x*blockDim.x+threadIdx.x;
				int rand_max = 32767;
				float randomVarb, tmp;
				randInt(randomVarb, rand_max, state);
				toRand =  ((r - l) * (randomVarb / rand_max)) + l;
	 }
}

struct Bird {
public:
  float x, y;
  bool active, moveUp;
	__device__
  Bird() {
    x = 100.0;
    y = (float) SCREEN_H/2;
    active = true;
    moveUp = false;
  }
	__device__
	void backInitialValues() {
    x = 100.0;
    y = (float) SCREEN_H/2;
    active = true;
    moveUp = false;
  }
};

struct Tree {
public:
  float x, h1, h2;
  bool add;
	__device__
	Tree() {}
  Tree(int coX) : x(coX){
    add = false;
  }
	__device__
  void heights(curandState* state) {
		h1 = 0.0;
		randInt(h1, 150, state);
    h1 = h1 + 100.0;
    h2 = h1 + DIFF_BETWEEN_H;
  }
};

struct List {
public:
	Tree trees[5];
	int el, size;
	__device__
	List() {
		el = 0;
		size = 0;
	}
	__device__
	void addTree(Tree t) {
		trees[el].x = t.x;
		trees[el].h1 = t.h1;
		trees[el].h2 = t.h2;
		trees[el].add = t.add;
		el++;
		size += 1;
	}
	__device__
	void removeFromStart() {
		for(int i=1; i<size; i++) {
			trees[i-1].x = trees[i].x; // czy aby na pewno to dziala jak nalezy?
			trees[i-1].h1 = trees[i].h1;
			trees[i-1].h2 = trees[i].h2;
			trees[i-1].add = trees[i].add;
			el--;
			size--;
		}
	}
};

extern "C" {
	__device__
		void fitness_function(List& list, int gameScore, Bird& bird, float res) { // return float
				 int idx = list.trees[0].x > bird.x ? 0 : 1;
				 res = DIST_BET_TWO*gameScore + 200 + 250 - (list.trees[idx].x - bird.x);
		// fitness = total travelled distance - distance to the closest gap
	 }
}

extern "C" {
	__device__
	void droppedTooLow(Bird& bird) { // bool
	  if(bird.y>=SCREEN_H) bird.active = false;
	  else bird.active = true;
	}
}

extern "C" {
	__device__
	void intersects(Bird& b, float CircX, float CircY, bool r) { // bool
	  float DeltaX = CircX - max(b.x, min(CircX, b.x + GIL_SIZE));
	  float DeltaY = CircY - max(b.y, min(CircY, b.y + GIL_SIZE));
		r = (DeltaX*DeltaX + DeltaY*DeltaY) < (CIRC_RAD*CIRC_RAD);
	}
}

extern "C" {
	__device__
		void collisionCheck(List& list, Bird& b) { // bool
			bool res;
			int siz = list.size;
			for(int i=0; i<siz; i++) {
	      float wspX = b.x + GIL_SIZE;
	      float wspY = b.y + GIL_SIZE;
	      if(list.trees[i].x<wspX && list.trees[i].x+WIDTH_TREE>b.x && list.trees[i].h1>b.y) { // && 0<wspY
					b.active = false;
					return;
				}
	      if(list.trees[i].x<wspX && list.trees[i].x+WIDTH_TREE>b.x && list.trees[i].h2<wspY) { // && SCREEN_H>gil[b].y
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
		void getIBirdPosition(Bird& b, float wspX, float wspY) { // pair<float, float>
			wspX = b.x;
			wspY = b.y;
		}
}

extern "C" {
	__device__
		void getTreePosition(List& list, int i, Tree& t) {
			t.x = list.trees[i].x;
			t.h1 = list.trees[i].h1;
			t.h2 = list.trees[i].h2;
			t.add = list.trees[i].add;
		}
}

extern "C" {
	__device__
		void getNumberOfTrees(List& list, int siz) {
		   siz = list.size;
		}
}

extern "C" {
	__device__
	void update(Bird& b) {
	  if(b.moveUp) {
	    b.y -= BIRD_IMPR;
	  } else {
	    float decr = SCREEN_H/(FPS*TIME_FALL_BIRD);
	    b.y += decr;
	  }
	}
}

extern "C" {
	__device__
	void createTree(List& list, float i, curandState* state) {
		{
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
        if(!list.trees[0].add && b.x>list.trees[0].x+WIDTH_TREE)
        {
           list.trees[0].add = true;
           gameScore++;
        }
		}
}

extern "C" {
	__device__
		void sigmoid(float* weights, float hor, float hei, float& res) {
			int thid = threadIdx.x + blockIdx.x*blockDim.x;
			float values[9];
			int offset = thid*WEIGHTS_SIZE;
			for(int i=0; i<9; i++) values[i] = 0;
		  values[0] = 1/(1 + exp(-hor-1));
		  values[1] = 1/(1 + exp(-hei-1));
		  for(int i=0; i<2; i++) {
		    for(int j=0; j<6; j++) {
		      values[j+2] += weights[offset+i*6+j]*values[i];
		    }
		  }
		  for(int i=2; i<8; i++) {
		    values[i] = 1/(1 + exp(-values[i]-1));
		    values[8] +=  weights[offset+i*6+0]*values[i];
		  }
			res = 1/(1 + exp(-values[8]-0.5));
		}
}

extern "C" {
	__global__
   	void TrainBirds(float* weights, float* fitness, curandState* states) {
			int thid = blockIdx.x*blockDim.x+threadIdx.x;
			int gameScore = 0;
			List list;
			createTree(list, FIRST_TREE, &states[thid]);
			createTree(list, SECOND_TREE, &states[thid]);
			createTree(list, THIRD_TREE, &states[thid]);
			Bird bird;
			while(1) {
				treeToRemove(list, bird, gameScore);
				float decr = SCREEN_H/(FPS*TIME_FALL_BIRD);
				for(int i=0; i<list.size; i++)
					list.trees[i].x -= MOVE_PIXS_ON_SEC/FPS;
				if(SCREEN_W - list.trees[list.size-1].x >= DIST_BET_TWO) {
					 createTree(list, list.trees[list.size-1].x + DIST_BET_TWO, &states[thid]);
				}
				if(bird.active) {
					int idx = list.trees[0].x > bird.x ? 0 : 1;
					float horizontal = list.trees[idx].x + WIDTH_TREE - bird.x;
					float heighDiff = ((list.trees[idx].h1 + list.trees[idx].h2)/2) - bird.y;
					float val = 0.0;
				  sigmoid(weights, horizontal, heighDiff, val);
					//if(thid<10) {
						//printf("%d %f\n", thid, val);
				  //}
					if(val>0.5) bird.moveUp = true;
					else bird.moveUp = false;
				}
				collisionCheck(list, bird);
				if(bird.active)
					droppedTooLow(bird);
				if(bird.active) {
						if(list.trees[0].x + WIDTH_TREE < 0) {
							list.removeFromStart();
						}
						if(bird.active)
							update(bird);
				} else {
					break;
				}
			}
			float res;
			fitness_function(list, gameScore, bird, res);
			fitness[thid] = res;
			__syncthreads();
	 }
}

extern "C" {
	__global__
   	void Mutation(float* weights, float* crosses, curandState *states) {
				int thid = blockDim.x*blockIdx.x + threadIdx.x;
				int offset = thid*WEIGHTS_SIZE;
				int ms = thid/NMB_CROSSES;
				int off_crs = ms*WEIGHTS_SIZE;
				for(int i=0; i<9; i++) {
						for(int j=0; j<6; j++) {
							float tmp, prev = crosses[off_crs+i*6+j];
							randFloat(tmp, prev-EPS, prev+EPS, &states[thid]);
							weights[offset + i*6 + j] = tmp;
						}
				}
				__syncthreads();
		}
}
