/*__constant__ float a = 5;
__constant__ float FPS = 60.0;
__constant__ float SCREEN_W = 1000.0;
__constant__ float SCREEN_H = 600.0;
__constant__ float GIL_SIZE = 45.0;
__constant__ float CIRC_RAD = 40.0;
__constant__ float WIDTH_TREE = 50.0;
__constant__ float DIST_BET_TWO = 250.0;
__constant__ float DIFF_BETWEEN_H = 265.0;
__constant__ int TREES_INITIALLY = 3;
__constant__ float TIME_FALL_BIRD = 4.0;
__constant__ float BIRD_IMPR = 50.0;
__constant__ float MOVE_PIXS_ON_SEC = 100.0;
__constant__ float BIRD_FALL_AFTER_HIT = 5.0;
*/

extern "C" {
	__global__
   	void TrainBirds(float* A, float* B) {

	}
}

extern "C" {
	__global__
   	void Mutation() {

	 }
}



/*__shared__ int shMat[32][33];
int thidy = blockIdx.x*32 + threadIdx.x;
int thidx = blockIdx.y*32 + threadIdx.y;
shMat[threadIdx.x][threadIdx.y] = A[thidx*cols+thidy];
__syncthreads();
int thid2y = blockIdx.y*32 + threadIdx.x;
int thid2x = blockIdx.x*32 + threadIdx.y;
AT[thid2x*rows + thid2y] = shMat[threadIdx.y][threadIdx.x];*/


/*int thid = blockIdx.x*blockDim.x+threadIdx.x;
__shared__ int prefBlock[2048];
int ibit1 = 1^((A[2*thid] >> i) & 1);
int ibit2 =  1^((A[2*thid+1] >> i) & 1);
prefBlock[2*threadIdx.x] = ibit1;
prefBlock[2*threadIdx.x+1] = ibit2;
__syncthreads();
int j = 1;
for(int i=0; i<=10; i++, j*=2)
{
	int temp1 = prefBlock[2*threadIdx.x];
	int temp2 = prefBlock[2*threadIdx.x+1];
	__syncthreads();
	if(2*threadIdx.x+j<2048){
		prefBlock[2*threadIdx.x+j] += temp1;
	}
	if(2*threadIdx.x+1+j<2048){
		prefBlock[2*threadIdx.x+1+j] += temp2;
	}
	__syncthreads();
}
prefSums[2*thid] = prefBlock[2*threadIdx.x];
prefSums[2*thid+1] = prefBlock[2*threadIdx.x + 1];
boundBlocks[blockIdx.x] = prefBlock[2047];
*/
