# CS406 Parallel Computing

# Term Project Final Report

##### Kaya Gökalp, İhsan Ufuk Dede, Celal Canol Taşgın, Alp Dinçer

## Problem Description

In this problem we are given a file containing information about an undirected graph G as well as an integer k which is between 2 and 6. Graph G has E edges and V vertices. In the file there are V+1 lines. V lines contain edges of the vertices and the remaining 1 line gives us the total number of vertices in the graph. Below we have given an undirected graph example to clarify the process.

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/Figure1.png" width="450px" alt="Figure 1"/>

As you can see in Figure 1 above, if we have an undirected graph like this and our k = 3 this means that we will search for cycles of length 3 inside our graph. Here we have 2 cycles with 3 vertices. First one is 2-3-4-2 and the other one is 6-7-8-6.

Also we’re supposed to run this program as .\executable path_to_file k

In the file, for each line ui < vi. You need to add the other orientation to the CSR data structure.

When working on small samples such as the above example, HPC is not needed at all, however when we tested our serial DFS version with the bigger matrices the calculation took so much time. Other than DFS, there were 2 options that were considered. One of them was BFS and the other one is a method which takes advantage of the properties of adjacency matrices. We realized that performance-wise BFS would not provide any considerable improvements compared to DFS, that is why the second mentioned method is used on the rest of the project. Throughout the project this method is referred to as the “multiplication method”.

## Solution Description & Other Existing Solutions

In order to find the number of k length cycles we used matrix representation of the undirected graphs. Then we’ve multiplied the matrices according to the k value given by the user. E.g. if k = 3, we multiply the matrix 3 times and divide the diagonals of the resulting matrix by 6. The corresponding row i and column i is the starting and ending index of the undirected graph. So if in the resulting matrix, [i,i] = 18, there are 3 (18/6) cycles which start from i and end at i.

There are also 2 other solutions that we’ve found worth mentioning. "Detecting Cycles in Graphs Using Parallel Capabilities of GPU" by Mahdi et al. talks about detecting cycles in undirected graphs in GPU. The process utilizes GPU’s high number of processing units by doing the same (simple) tasks on different data. The process as discussed by Mahdi et al. can result in a lot of iterations so they applied the pretesting via a method called virtual adjacency matrix. This method seems to be really effective and we are planning to use a similar method. Also, an approximation related to connections between nodes (rate of connectedness) of the graph is used to even lower the required iterations. Because even though the virtual adjacency matrix method is used, the resulting required iterations are proportional to c!/2c where c is the cycle length so long cycles result in a lot of iterations. But in our case, the max cycle length is 5. So we may not need to use the last method.

We also searched for some code examples to get familiar with the process of finding cycles in undirected graphs. One such example we found is from a site called “geeksforgeeks.org”.In this example they are working on an undirected graph which has V = 5 vertices and they are looking for cycles of length n = 4. They use DFS to find every path of length (n-1) from a vertex and check if the path leads back to the starting point. They keep track of the marked vertices using an array of size V. They iterate through each vertex and unmark the ones that do not form a cycle during the process, so that they can be used again. In the end count/2 is returned because every cycle is counted twice going from the opposite path as well. Although it is not a completely optimal example, it gave us some ideas that might be useful as we move forward with our project.

## Model & Data Description

CSR (Compressed Sparse Column) representation is a special representation where there are 2 arrays adj and xadj. Adj is used for holding the column indices for non zero values for each row and xadj is used for holding the beginning index of each row in the adj. For the graphs case, by using these arrays we can easily see which vertex has connections to the other vertices. This way we can move from one vertex k times and easily see if there is a cycle or not. Another benefit of CSR is that it is a lot cheaper to store since we only have to keep certain parts of the matrix.

Using the CSR representation, it is possible to utilize the multiplication method (which is discussed in the further passages) as the matrices are represented in CSR and they can be multiplied in that format. Which will result with further space and time complexity wise optimization of the solution. It is about parallelization of the multiplication of the CSRs. Which is a regular matrix multiplication parallelization.

For our cpu parallel version, the parallelization of the DFS and BFS was attempted. In the DFS based solution it was found out that parallelizing a DFS solution is hard and inefficient. We tried task based parallelization to create tasks with each DFS call but due to the nature of the DFS it is not very good performing either. We could not provide any numerical results since the big dataset is not halting currently with our serial implementation and the example datasets we are creating by hand are too small to give a meaningful result. We started to research a better parallelizable solution to our problem and we found out there are some solutions using BFS based solutions since from our research we understand that parallelizing a BFS solution is more common and preferred. To find cycles with small lengths searching the connected vertices to our vertex is required and one can try to reach the starting node with k (length of the cycle) steps. This makes us process all connected nodes and seems to be very computationally heavy. But this solution is easier to parallelize with tasks. And we thought of a way to reduce the number of searches needed to be made. First the distances of each node from the starting node should be found with parallel BFS, since we know the distance we can just work the previous algorithm from nodes that are at least k-2 nodes distant to the previous iteration. Since when we apply the finding algorithm we will find the possible cycles that can be formed using the nodes that are maximum k-2 distanced from the initial node. There are several techniques applied to have a good performing parallel bfs implementation such as bagging each layer and process layers sequentially but inside the bag, parallelism will be achieved.

We realized we do not have to find out all possible cycles with length k to obtain our required result. We just needed to find out for each vertex how many times a vertex is present in a cycle with length k. We came across a really interesting solution that we are currently working on. Which is using adjacency matrix multiplications to determine the number of each vertex in a cycle. When we take the power of k of a adjacency matrix the resulting matrix is giving the result we need in diagonal entries. aii (subindex) is the 6\*number of cycles containing vertex i. Using that property we realized that we can write a massively parallel application since sparse-sparse matrix multiplication is one of the main areas of parallel computing and it can be parallelized in a very good manner. An example for our current approach would be the following. (using the graph in Figure 1.1). Adjacency matrix for that graph is shown in Figure 2.1.

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/Figure2_1.png" width="300px" alt="Figure 2.1"/>

For example for k = 4, we have 3 unique cycles. 0 - 1 - 2 - 3 , 0 - 1 - 4 - 3, 1 - 2 - 3 - 4
Our output:

        0 - 2
        1 - 3
        2 - 2
        3 - 3
        4 - 2

To use the “multiplication method” we need to multiply our matrix with itself 4 times. The result is shown at the Figure 2.2

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/Figure2_2.png" width="300px" alt="Figure 2.2"/>

To find out how many times 0 vertex is used we need to look at the a00 (subindex) which is 12. And according to the multiplication method we use 12/6 = 2 unique cycles with length 4 has vertex 0. So our output for 0 is 2. To demonstrate another example, the appearance of vertex 1 can be used. a11(subset) is 18. 18/6 = 3 unique cycles contains vertex 1.

We are currently implementing parallel sparse sparse matrix multiplication to be used to find out the number of appearances of any vertex in required length cycles.

## Technical Description of the Parallel Application

The parallelization of the application is parallelization of the matrix matrix multiplication in abstract terms. For this purpose, OpenMP and Cuda interfaces were utilized for CPU and GPU implementations respectively.

For the CPU implementation with OpenMP, the parallelization was utilized using guided scheduling for for loop. Guided scheduling over dynamic scheduling was used for convenience in load balancing. Also it has a good overhead/ load balancing ratio. For the results of the multiplication, vectors were used as the size that is going to be needed is not known prior to the execution of the multiplication operation. So, for each thread a separate vector is created and results are combined after every thread completes its task.

In the GPU implementation with Cuda, each line in the matrix was assigned to a thread. The thread and block structure is 32 x 1024 where 32 is the amount of the blocks and 1024 is the amount of threads per block. Multiplication operation for each line is executed by a different thread in the structure. In the GPU implementation, instead of vectors, arrays were used since they are more efficient and convenient when it comes to memory allocation and memory sharing between the source and device. Since the amount of the multiplication results is still unknown and a certain amount of memory has to be allocated prior to saving the results, first the amount that is going to be allocated is calculated. After that is determined, multiplication is executed. Also, for the parallelization purpose, since the array that holds the results is shared between the threads, a look up table is used which holds the start and end positions for each thread.

## Techincal Description of Software

#### Folder Structure:

images: Some example images of graph representation in matrix.
Misc: Build script
Res: Test graphs represented in txt files.
Src: Source code for GPU and CPU implementation.

#### Description and Guide:

C++ was used as the programming language along with Cuda and OpenMP interfaces. OpenMP, Cuda 10.0 and Gcc 7.5.0 has to be installed before building. To build the code, sh misc/build.sh can be run. Executable will be under /build. To run the executable, ./output.exe graphfile.txt [cycle_length] [num_of_threads] where “output” has to be replaced with the executable name, ”graphfile” has to be replaced with the graph file in question, “cycle_length” is the number of cycles that the program will look for, and “num_of_threads” is the number of parallel threads. To reproduce the test results;

    g++ ./main.cpp -o a.exe -std=c++14 -O3 -fopenmp
    ./a.exe amazon.txt 3 1

has to be run respectively and changing the num_of_threads and cycl_length values. To run the GPU implementation, find_result_gpu function in the main should be uncommented. And the algorithm will run with 32 bocks and 1024 threads per block independent from the thread number given when running the executable.

## Performance Evaluation

Both of our previous DFS and BFS trials primarily had synchronization overheads. The way DFS and BFS handle our problem is by moving one vertex at a time while looking for a cycle, thus a considerable part of the runtime was spent waiting for the previous task to be completed. Although it was not as significant as synchronization, load balancing was also an overhead we had with these models. Unless it is a perfectly balanced graph some branches will be longer than others, which in return leaves us with unbalanced workloads between threads.

In our latest model we are dealing with sparse matrix multiplication. The biggest drawback of this method is that it gets slower as the matrices get denser. For example if our matrix has more 1’s than 0’s it will be considerably slower. Another major overhead is communication which comes with matrix-matrix multiplication. Most of the running-time of parallel sparse matrix multiplication is spent on inter-process communication, due to the need of exchanging partial results between processors while forming the final product matrix.

One way to overcome these obstacles is to store only the non-zero data elements of the sparse matrix. We can use a list to store these elements along with their row and column indices., which is where CSR format comes into play. Thanks to the CSR format these matrix multiplications we use in our solution become much more manageable.
<br/>

### Cycle of 3

| #Threads | Runtime | Speedup | Throughput |
| -------- | ------- | ------- | ---------- |
| 1        | 3.08    | -       | 1.41       |
| 4        | 1.97    | 1.56    | 1.25       |
| 8        | 1.79    | 1.71    | 1.19       |
| 16       | 1.81    | 1.702   | 0.89       |
| 32       | 1.80    | 1.709   | 0.98       |
| 60       | 1.75    | 1.759   | 1.07       |

<br/>

### Cycle of 4

| #Threads | Runtime | Speedup | Throughput |
| -------- | ------- | ------- | ---------- |
| 1        | 6.53    | -       | 1.61       |
| 4        | 3.04    | 2.14    | 1.45       |
| 8        | 2.44    | 2.67    | 1.39       |
| 16       | 2.29    | 2.85    | 1.09       |
| 32       | 2.24    | 2.90    | 1.14       |
| 60       | 2.27    | 2.86    | 0.94       |

<br/>

### Cycle of 5

| #Threads | Runtime | Speedup | Throughput |
| -------- | ------- | ------- | ---------- |
| 1        | 19.05   | -       | 1.84       |
| 4        | 6.79    | 2.80    | 1.72       |
| 8        | 4.61    | 4.13    | 1.68       |
| 16       | 3.65    | 5.21    | 1.44       |
| 32       | 3.30    | 5.76    | 1.58       |
| 60       | 3.23    | 5.90    | 1.48       |

When it comes to the scaling, our program starts good but as the thread number increases the speed-up rate does not increase proportionally. That is why we think our parallel version has weak scaling.

## References

- Cycles of length n in an undirected and connected graph. GeeksforGeeks. (2019, July 24). https://www.geeksforgeeks.org/cycles-of-length-n-in-an-undirected-and-connected-graph/.
- Some interesting properties of adjacency matrices. First Principles. (2008, March 30). https://1stprinciples.wordpress.com/2008/03/30/some-interesting-properties-of-adjacency-matrices/.
- Buluc, A. (n.d.). Parallel Sparse Matrix-Vector and Matrix-Transpose-Vector Multiplication Using Compressed Sparse Blocks.
- TY - JOUR AU - Ahmed, Md Salman AU - Houser, Jennifer AU - Hoque, Mohammad AU - Raju, Rezaul AU - Pfeiffer, Phil PY - 2017/07/01 SP - 46 EP - 59 T1 - Reducing Inter-Process Communication Overhead in Parallel Sparse Matrix-Matrix Multiplication VL - 9 DO - 10.4018/IJGHPC.2017070104 JO - International Journal of Grid and High Performance Computing ER -
