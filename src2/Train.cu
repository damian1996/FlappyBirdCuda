#include "consts.h"
#include <thrust/device_vector.h>
#include <vector>
#include <curand_kernel.h>
// https://stackoverflow.com/questions/8012304/why-defining-class-headers-without-cuda-device-attribute-works-c

__constant__ float FIRST_TREE = 400.0;
__constant__ float SECOND_TREE = 650.0;
__constant__ float THIRD_TREE = 900.0;



extern "C" {
	__device__
	void randInt(float& toRand, int interval) {
				int thid = blockIdx.x*blockDim.x+threadIdx.x;
				curandState state;
				curand_init(666, thid, 0, &state);
				toRand = curand(&state)%interval;
	 }
}

extern "C" {
	__device__
	void randFloat(float& toRand, int interval) {
				int thid = blockIdx.x*blockDim.x+threadIdx.x;
				curandState state;
				curand_init(666, thid, 0, &state);
				toRand = curand(&state)%interval;
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
  void heights() {
		h1 = 0.0;
		randInt(h1, 150);
    h1 = h1 + 100.0;
    h2 = h1 + DIFF_BETWEEN_H;
  }
};

struct List {
	Tree trees[5];
	int el = 0;
	int size = 0;
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
			trees[i-1] = trees[i]; // czy aby na pewno to dziala jak nalezy?
			el--;
			size--;
		}
	}
};

extern "C" {
	__device__
	 void fitness_function(int idx, float res) { // return float

	 }
}

extern "C" {
	__device__
   	void Mutation() {

		}
}

extern "C" {
	__device__
	void droppedTooLow(Bird& bird, bool r) { // bool
	  if(bird.y>=SCREEN_H) r = false;
	  else r = true;
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
		void collisionCheck(List& list, Bird& b, bool r) { // bool
			bool res;
			int siz = list.size;
			for(int i=0; i<siz; i++) {
	      float wspX = b.x + GIL_SIZE;
	      float wspY = b.y + GIL_SIZE;
	      if(list.trees[i].x<wspX && list.trees[i].x+WIDTH_TREE>b.x && list.trees[i].h1>b.y) { // && 0<wspY
					r = false;
					return;
				}
	      if(list.trees[i].x<wspX && list.trees[i].x+WIDTH_TREE>b.x && list.trees[i].h2<wspY) { // && SCREEN_H>gil[b].y
					r = false;
					return;
				}

	      float middleX = list.trees[i].x+WIDTH_TREE/2;
	      float middleY = list.trees[i].h1+(2*CIRC_RAD)/3;
				intersects(b, middleX, middleY, res);
	      if(res) {
					r = false;
					return;
				}
				middleY = list.trees[i].h2-(2*CIRC_RAD)/3;
	      intersects(b, middleX, middleY, res);
	      if(res) {
					r = false;
					return;
				}
	    }
	    r = true;
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
			// add?
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
		void moveUp(Bird b) {
		  b.moveUp = true;
		}
}

extern "C" {
	__device__
	void createTree(List& list ,float i) {
		{
			Tree t;
			t.x = i;
		  t.heights();
		  list.addTree(t);
		}
	}
}

extern "C" {
	__device__
		void treeToRemove(List& list, Bird& b, bool r, int gameScore)
		{
        if(!list.trees[0].add && b.x>list.trees[0].x+WIDTH_TREE)
        {
           list.trees[0].add = true;
           gameScore++;
           r = true;
        }
				else
					 r = false;
		}
}

extern "C" {
	__device__
		void sigmoid(float hor, float hei, float& res) {
		  /*
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
			*/
		}
}

extern "C" {
	__global__
   	void TrainBirds(float* weights, float* fitness) {
			int gameScore = 0;
			List list;
			createTree(list, FIRST_TREE);
			createTree(list, SECOND_TREE);
			createTree(list, THIRD_TREE);
			Bird bird;
		}
}

				// vector drzew + inicjalizacja (3 poczatkowe drzewka oraz ptaszek)
				/*while(1)
				{
						treeToRemove();

						float decr = SCREEN_H/(FPS*TIME_FALL_BIRD);
						//for(auto& it : list_trees) // przesuniecie drzew
							//it.x -= MOVE_PIXS_ON_SEC/FPS;
						if(SCREEN_W - list_trees[list_trees.size()-1].x >= DIST_BET_TWO) {
							 createTree(list_trees[list_trees.size()-1].x + DIST_BET_TWO);
						}
						for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
							if(gil[i].active) {
									int idx = list_trees[0].x > gil[i].x ? 0 : 1;
									float horizontal = list_trees[idx].x+WIDTH_TREE-gil[i].x;
									float heighDiff = ((list_trees[idx].y + list_trees[idx].height2)/2) - gil[i].y;
									float val = botToTrain.visibleNets[i].sigmoid(horizontal, heighDiff);
									if(val>=0) gil[i].moveUp = true;
									else gil[i].moveUp = false;
									//moveUp(i); // 0.5
									update(i);
									// to samo co na cudzie
							}
						}
						for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
							//if(gil[i].active)
							 gil[i].active = collisionCheck(i);
							//printf("aha.. %d\n", gil[i].active);
						}

						for(int i=0; i<NUMBER_DISPLAY_BOTS; i++)
						{
							if(gil[i].active==true)
								gil[i].active = droppedTooLow(i);
						}
								if(allDead()==false) {
										//printf("Huh?");
										if(list_trees[0].x + WIDTH_TREE < 0)
											 list_trees.erase(list_trees.begin());

										for(int i=0; i<NUMBER_DISPLAY_BOTS; i++) {
												if(gil[i].active) {
													 update(i);
												}
										}
								}
								else if(allDead()==true){
									break;
								}
						}*/
