#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <math.h>       /* fabsf */
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DEBUG 0

//Error check-----
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
  if (code != cudaSuccess) 
    {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
    }
}
//Error check-----
//This is a very good idea to wrap your calls with that function.. Otherwise you will not be able to see what is the error.
//Moreover, you may also want to look at how to use cuda-memcheck and cuda-gdb for debugging.


__global__ void find_result_from_matrix(int *adj, int *xadj, int *values, int size, int *cycle_result, int cycle_length)
{
	int step = 32*1024;
	int id = blockDim.x * blockIdx.x + threadIdx.x; //Global id for the thread
	for(int i = id; i<size; i+= step)
	{
		cycle_result[i] = 0;
		for(int col_iter = xadj[i]; col_iter < xadj[i+1]; col_iter++)
		{
			int col_ptr = adj[col_iter];
			if(col_ptr == i)
			{
				cycle_result[i] = values[col_iter]/(2*cycle_length);
			}
		}
	}
}


__global__ void multiply_matrix(int *adj, int *xadj, int *tadj, int *txadj,int* values, int *tvalues, int size, int * lookup_place, int *res_adj, int *res_values)
{
	int step = 32 * 1024;
	int id = blockDim.x * blockIdx.x + threadIdx.x; //Global id for the thread
	for(int i = id; i<size; i+= step)
	{
		int result_size = 0;
		//For each non zero element 
		for(int col_iter = xadj[i]; col_iter < xadj[i+1]; col_iter++)
		{
			int col_ptr = adj[col_iter];
			int place = 0;
			//for each non zero element
			for(int row_iter = txadj[col_ptr]; row_iter < txadj[col_ptr+1]; row_iter++)
			{
				int row_ptr = tadj[row_iter];
				int value = values[col_iter] * tvalues[row_iter];
				int place_to_enter_data = lookup_place[i]+place;
				if(result_size <= place)
				{
					res_adj[place_to_enter_data] = row_ptr;
					res_values[place_to_enter_data] = value;
					result_size++;
				}else
				{
					res_values[place_to_enter_data] += value;
				}
				place++;
			}
		}
	}
}

__global__ void find_required_memory(int *adj, int *xadj, int *tadj, int *txadj, int size, int* result)
{
	int step = 32 * 1024;
	int id = blockDim.x * blockIdx.x + threadIdx.x; //Global id for the thread
	for(int i = id; i<size; i+= step)
	{
		int result_size = 0;
		//For each non zero element 
		for(int col_iter = xadj[i]; col_iter < xadj[i+1]; col_iter++)
		{
			int col_ptr = adj[col_iter];
			int place = 0;
			//for each non zero element
			for(int row_iter = txadj[col_ptr]; row_iter < txadj[col_ptr+1]; row_iter++)
			{
				if(result_size <= place)
				{
					result_size++;
				}
				place++;
			}
		}
		result[i] += result_size;
	}
}



void wrapper(int* adj, int* xadj, int* tadj, int* txadj, int* values, int *tvalues, int* &res_adj,int *&res_xadj, int  *&res_values, int size, int adj_size, int xadj_size, int tadj_size, int txadj_size, int values_size, int tvalues_size, int &result_adj_size, int &result_xadj_size, int &result_values_size){
  
  int* d_adj, *d_xadj, *d_tadj, *d_txadj, *d_values, *d_tvalues, *d_res_adj, *d_res_values,*d_res_size;

  int *res_size = (int*)malloc(size * sizeof(int));
  memset(res_size, 0 ,(size)*sizeof(int));

  cudaMalloc(&d_adj, adj_size * sizeof(int));
  cudaMalloc(&d_xadj, xadj_size * sizeof(int));
  cudaMalloc(&d_tadj, tadj_size * sizeof(int));
  cudaMalloc(&d_txadj, txadj_size * sizeof(int));
  cudaMalloc(&d_values, values_size * sizeof(int));
  cudaMalloc(&d_tvalues, tvalues_size * sizeof(int));
  cudaMalloc(&d_res_size, size * sizeof(int));

  cudaMemcpy(d_res_size, res_size, (size) * sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_adj,adj,adj_size*sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_xadj,xadj,xadj_size*sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_tadj,tadj,tadj_size*sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_txadj,txadj,txadj_size*sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_values,values,values_size*sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_tvalues,tvalues,tvalues_size*sizeof(int), cudaMemcpyHostToDevice);
  
  find_required_memory<<<32,1024>>>(d_adj, d_xadj, d_tadj, d_txadj,size,d_res_size);
  gpuErrchk( cudaDeviceSynchronize() );
  cudaMemcpy(res_size, d_res_size, size * sizeof(int), cudaMemcpyDeviceToHost);
  int sum = 0;
  #pragma omp parallel for reduction(+: sum)
  for(int i = 0; i< size; i++)
  {
	sum+=res_size[i];
  }
  cudaMalloc(&d_res_values, (sum) * sizeof(int));
  cudaMalloc(&d_res_adj, (sum) * sizeof(int));

  //TODO(kaya) : find another solution for this = parallel.
  int *place_look_up = (int*)malloc(size * sizeof(int));
  int *d_place_look_up;
  cudaMalloc(&d_place_look_up, size*sizeof(int));
  place_look_up[0] = 0; 
  for(int i = 1; i< size; i++)
  {
	place_look_up[i] = place_look_up[i-1] + res_size[i-1];
  } 
  cudaMemcpy(d_place_look_up, place_look_up, size*sizeof(int), cudaMemcpyHostToDevice);
  //place_look_up will be used to determine where to enter the adj and values data for each line
  multiply_matrix<<<32,1024>>>(d_adj,d_xadj,d_tadj,d_txadj,d_values,d_tvalues,size,d_place_look_up,d_res_adj,d_res_values);
  gpuErrchk( cudaDeviceSynchronize() );
  res_adj = (int*)(malloc(sum * sizeof(int)));
  res_values = (int*)(malloc(sum * sizeof(int)));
  cudaMemcpy(res_adj,d_res_adj, sum*sizeof(int), cudaMemcpyDeviceToHost);  
  cudaMemcpy(res_values,d_res_values, sum*sizeof(int), cudaMemcpyDeviceToHost);  
  res_xadj = (int*)malloc((size+1) * sizeof(int));
  for(int i = 0; i<size; i++)
  {
	res_xadj[i] = place_look_up[i];
  } 
  res_xadj[size] = sum;
  result_adj_size = sum;
  result_xadj_size = size+1;
  result_values_size = sum;
  printf("multiplication done\n");
  free(place_look_up);
  free(res_size);
  cudaFree(d_adj);
  cudaFree(d_xadj);
  cudaFree(d_tadj);
  cudaFree(d_txadj);
  cudaFree(d_values);
  cudaFree(d_tvalues);
  cudaFree(d_res_size);
  cudaFree(d_place_look_up);
}
void find_result_gpu(std::vector<int> &adj, std::vector<int> &xadj, std::vector<int> &values, int size, int cycle_length)
{
  	float elapsedTime;
  	cudaEvent_t start,stop; 
  	cudaEventCreate(&start);
  	cudaEventRecord(start, 0);
	int *adj_ptr = &adj[0];
	int *xadj_ptr = &xadj[0];
	int *values_ptr = &values[0];

	int *result_adj;
	int  result_adj_size = 0;
	int *result_xadj;
	int  result_xadj_size = 0;
	int *result_values;
	int  result_values_size = 0;
	if(cycle_length == 3)
	{
		int *pa_result_adj;
		int  pa_result_adj_size = 0;
		int *pa_result_xadj;
		int  pa_result_xadj_size = 0;
		int *pa_result_values;
		int  pa_result_values_size = 0;
		wrapper(adj_ptr, xadj_ptr, adj_ptr, xadj_ptr, values_ptr, values_ptr,pa_result_adj,pa_result_xadj,pa_result_values,size,adj.size(), xadj.size(),adj.size(),xadj.size(), values.size(), values.size(),pa_result_adj_size,pa_result_xadj_size,pa_result_values_size);
		wrapper(pa_result_adj, pa_result_xadj, adj_ptr, xadj_ptr, pa_result_values, values_ptr,result_adj,result_xadj,result_values,size,pa_result_adj_size, pa_result_xadj_size,adj.size(),xadj.size(), pa_result_values_size, values.size(),result_adj_size,result_xadj_size,result_values_size);
		//A*A*A => R
		
		free(pa_result_adj);
		free(pa_result_xadj);
		free(pa_result_values);
	}else if(cycle_length == 4)
	{
		int *pa_result_adj;
		int  pa_result_adj_size = 0;
		int *pa_result_xadj;
		int  pa_result_xadj_size = 0;
		int *pa_result_values;
		int  pa_result_values_size = 0;
		wrapper(adj_ptr, xadj_ptr, adj_ptr, xadj_ptr, values_ptr, values_ptr,pa_result_adj,pa_result_xadj,pa_result_values,size,adj.size(), xadj.size(),adj.size(),xadj.size(), values.size(), values.size(),pa_result_adj_size,pa_result_xadj_size,pa_result_values_size);
		wrapper(pa_result_adj, pa_result_xadj, pa_result_adj, pa_result_xadj, pa_result_values, pa_result_values,result_adj,result_xadj,result_values,size,pa_result_adj_size, pa_result_xadj_size,pa_result_adj_size,pa_result_xadj_size, pa_result_values_size, pa_result_values_size,result_adj_size,result_xadj_size,result_values_size);

		free(pa_result_adj);
		free(pa_result_xadj);
		free(pa_result_values);
		//A*A => A2 * A2 => R
	}else if(cycle_length == 5)
	{
		int *pa_result_adj;
		int  pa_result_adj_size = 0;
		int *pa_result_xadj;
		int  pa_result_xadj_size = 0;
		int *pa_result_values;
		int  pa_result_values_size = 0;
		int *pa_result2_adj;
		int  pa_result2_adj_size = 0;
		int *pa_result2_xadj;
		int  pa_result2_xadj_size = 0;
		int *pa_result2_values;
		int  pa_result2_values_size = 0;
		wrapper(adj_ptr, xadj_ptr, adj_ptr, xadj_ptr, values_ptr, values_ptr,pa_result_adj,pa_result_xadj,pa_result_values,size,adj.size(), xadj.size(),adj.size(),xadj.size(), values.size(), values.size(),pa_result_adj_size,pa_result_xadj_size,pa_result_values_size);
		wrapper(pa_result_adj, pa_result_xadj, pa_result_adj, pa_result_xadj, pa_result_values, pa_result_values,pa_result2_adj,pa_result2_xadj,pa_result2_values,size,pa_result_adj_size, pa_result_xadj_size,pa_result_adj_size,pa_result_xadj_size, pa_result_values_size, pa_result_values_size,pa_result2_adj_size,pa_result2_xadj_size,pa_result2_values_size);
		wrapper(pa_result2_adj, pa_result2_xadj, adj_ptr, xadj_ptr, pa_result2_values, values_ptr,result_adj,result_xadj,result_values,size,pa_result2_adj_size, pa_result2_xadj_size,adj.size(),xadj.size(), pa_result2_values_size, values.size(),result_adj_size,result_xadj_size,result_values_size);

		//TODO(kaya): find the cycle result
		free(pa_result_adj);
		free(pa_result_xadj);
		free(pa_result_values);
		free(pa_result2_adj);
		free(pa_result2_xadj);
		free(pa_result2_values);
		//A*A => A2 * A2 * A => R
	}
	int *d_adj, *d_xadj, *d_values, *d_cycle_result;
	cudaMalloc(&d_adj, result_adj_size * sizeof(int));
	cudaMalloc(&d_xadj, result_xadj_size * sizeof(int));
	cudaMalloc(&d_values, result_values_size * sizeof(int));
	cudaMalloc(&d_cycle_result, size * sizeof(int));
	cudaMemcpy(d_adj, result_adj, result_adj_size * sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_xadj, result_xadj, result_xadj_size * sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_values, result_values, result_values_size * sizeof(int), cudaMemcpyHostToDevice);
	find_result_from_matrix<<<32,1024>>>(d_adj, d_xadj, d_values, size, d_cycle_result,cycle_length);	
  	gpuErrchk( cudaDeviceSynchronize() );
	int* result_cycle = (int*)malloc(size * sizeof(int));	
	cudaMemcpy(result_cycle, d_cycle_result, size * sizeof(int), cudaMemcpyDeviceToHost);
  	
	cudaEventCreate(&stop);
  	cudaEventRecord(stop, 0);
  	cudaEventSynchronize(stop);
  	
	cudaEventElapsedTime(&elapsedTime, start, stop);
	printf("Elapsed time: %f\n",elapsedTime);
	/*
	for(int i = 0; i<size; i++)
	{
		printf("%d	%d\n", i, result_cycle[i]);
	}
	*/
	cudaFree(d_adj);
	cudaFree(d_xadj);
	cudaFree(d_values);
	cudaFree(d_cycle_result);
	free(result_adj);
	free(result_xadj);
	free(result_values);
	free(result_cycle);
}
