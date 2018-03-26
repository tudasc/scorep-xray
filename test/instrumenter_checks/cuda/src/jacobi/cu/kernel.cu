#include "definitions.cuh"

//Performs CFD calculation on global memory. This code does not use any advance optimization technique on GPU
// But still acheives many fold performance gain
__global__ void calculateCFD_V1( float* input,  float* output, unsigned int Ni, unsigned int Nj, 
								   float h)
{
	unsigned int i = blockDim.x * blockIdx.x + threadIdx.x; // Y - ID
	unsigned int j = blockDim.y * blockIdx.y + threadIdx.y; // X - ID

	unsigned int iPrev = i-1; // Previous Y element
	unsigned int iNext = i+1; // Next Y element

	unsigned int jPrev = j-1; //Previous X element
	unsigned int jNext = j+1; // Next X element


	unsigned int index = i * Nj + j;

	if( i > 0 && j > 0 && i < (Ni-1) && j <(Nj-1))
		output[index] = 0.25f * (input[iPrev * Nj + j] + input[iNext* Nj + j] + input[i * Nj+ jPrev] 
			+ input[i* Nj + jNext] - 4*h*h);
}

//This version of Kernel uses optimization by copying the data into shared memory and hence results in better performance
__global__ void calculateCFD_V2( float* input,  float* output, unsigned int Ni, unsigned int Nj, 
								   float h)
{

	//Current Global ID
	int i = blockDim.y * blockIdx.y + threadIdx.y; // Y - ID
	int j = blockDim.x * blockIdx.x + threadIdx.x; // X - ID
	
	//Current Local ID (lXX --> refers to local ID i.e. inside a block)
	int li = threadIdx.y;
	int lj = threadIdx.x;
	
	// e_XX --> variables refers to expanded shared memory location in order to accomodate halo elements
	//Current Local ID with radius offset.
	int e_li = li + RADIUS;
	int e_lj = lj + RADIUS;

	// Variable pointing at top and bottom neighbouring location
	int e_li_prev = e_li - 1;
	int e_li_next = e_li + 1;

	// Variable pointing at left and right neighbouring location
	int e_lj_prev = e_lj - 1;
	int e_lj_next = e_lj + 1;

	__shared__ float sData [THREADS_PER_BLOCK_Y + 2 * RADIUS][THREADS_PER_BLOCK_X + 2 * RADIUS];
	
	unsigned int index = (i)* Nj + (j) ;

	if( li<RADIUS ) // copy top and bottom halo
	{
		//Copy Top Halo Element
		if(blockIdx.y > 0) // Boundary check
			sData[li][e_lj] = input[index - RADIUS * Nj];

		//Copy Bottom Halo Element
		if(blockIdx.y < (gridDim.y-1)) // Boundary check
			sData[e_li+THREADS_PER_BLOCK_Y][e_lj] = input[index + THREADS_PER_BLOCK_Y * Nj];

	}

	if( lj<RADIUS ) // copy left and right halo
	{
		if( blockIdx.x > 0) // Boundary check
			sData[e_li][lj] = input[index - RADIUS];
		
		if(blockIdx.x < (gridDim.x-1)) // Boundary check
			sData[e_li][e_lj+THREADS_PER_BLOCK_X] = input[index + THREADS_PER_BLOCK_X];
	}
	
	// copy current location
	sData[e_li][e_lj] = input[index]; 

	__syncthreads( );

	if( i > 0 && j > 0 && i < (Ni-1) && j <(Nj-1))
		output[index] = 0.25f * (sData[e_li_prev][e_lj] + sData[e_li_next][e_lj] + sData[e_li][e_lj_prev] 
			+ sData[e_li][e_lj_next] - 4*h*h);
	
}
