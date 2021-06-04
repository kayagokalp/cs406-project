# CS406 Parallel Computing

# To Run Quickly

In misc there is a build.sh that can be used to compile the project. If parent folder name is different than cs406-project-main quick fix of build.sh can be made.

The executable appears in the build folder. 

General structure:
./program.out ./res/input.txt k nt

k is path length
nt is number of threads in CPU, if nt = 0 GPU implementation will work. default nt = 60

# Term Project Final Report

##### Kaya Gökalp, İhsan Ufuk Dede, Celal Canol Taşgın, Alp Dinçer

## Problem Description

In this problem we are given a file containing information about an undirected graph G as well as an integer k which is between 2 and 6. Graph G has E edges and V vertices. Lines contain edges of the graph. Below we have given an undirected graph example to clarify the process.

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/Figure1.png" width="450px" alt="Figure 1"/>

As you can see in Figure 1 above, if we have an undirected graph like this and our k = 3 this means that we will search for circuits of length 3 inside our graph. Here we have 2 circuits with 3 vertices. First one is 2-3-4-2 and the other one is 6-7-8-6.

When working on small samples such as the above example, parallel computing is not needed at all, however when we tested our serial DFS version with the bigger matrices the calculation took so much time. Other than DFS, there were 2 options that were considered. One of them was BFS and the other one is a method which takes advantage of the properties of adjacency matrices. We realized that performance-wise BFS would not provide any considerable improvements compared to DFS, that is why the second mentioned method is used on the rest of the project. 

#### Cycles vs. Circuits:
It should be considered that the final solution we have is for finding circuits and not cycles. So cases where edge or vertex repitions exits are counted in the results. But, potential solutions that count cycles are also discussed and presented briefly. 

## Solution Description & Other Existing Solutions

In order to find the number of k length circuits we used matrix representation of the undirected graphs. Then we’ve multiplied the matrices according to the k value given by the user. E.g. if k = 3, we multiply the matrix 3 times and divide the diagonals of the resulting matrix by 6. The corresponding row i and column i is the starting and ending index of the undirected graph. So if in the resulting matrix, [i,i] = 18, there are 3 (18/6) circuits which conatins i.

There are also 2 other solutions that we’ve found worth mentioning. "Detecting Cycles in Graphs Using Parallel Capabilities of GPU" by Mahdi et al. talks about detecting cycles in undirected graphs in GPU. The process utilizes GPU’s high number of processing units by doing the same (simple) tasks on different data. The process as discussed by Mahdi et al. can result in a lot of iterations so they applied the pretesting via a method called virtual adjacency matrix. Also, an approximation related to connections between nodes (rate of connectedness) of the graph is used to even lower the required iterations. Because even though the virtual adjacency matrix method is used, the resulting required iterations are proportional to c!/2c where c is the cycle length so long cycles result in a lot of iterations.

We also searched for some code examples to get familiar with the process of finding cycles in undirected graphs. One such example we found is from a site called “geeksforgeeks.org”.In this example they are working on an undirected graph which has V = 5 vertices and they are looking for cycles of length n = 4. They use DFS to find every path of length (n-1) from a vertex and check if the path leads back to the starting point. They keep track of the marked vertices using an array of size V. They iterate through each vertex and unmark the ones that do not form a cycle during the process, so that they can be used again. In the end count/2 is returned because every cycle is counted twice going from the opposite path as well. Although it is not a completely optimal example, it gave us some ideas that could have been useful for DFS implementation.

## Model & Data Description

CSR (Compressed Sparse Column) representation is a special representation where there are 2 arrays adj and xadj. Adj is used for holding the column indices for non zero values for each row and xadj is used for holding the beginning index of each row in the adj. For the graphs case, by using these arrays we can easily see which vertex has connections to the other vertices. This way we can move from one vertex k times and easily see if there is a circuit or not. Another benefit of CSR is that it is a lot cheaper to store since we only have to keep certain parts of the matrix.

Using the CSR representation, it is possible to utilize the multiplication method (which is discussed in the further passages) as the matrices are represented in CSR and they can be multiplied in that format. Which will result with further space and time complexity wise optimization of the solution. It is about parallelization of the multiplication of the CSRs. And multiplying two CSR's is not straighforward. CSR and CSG is multplied to obtain the result. Which is a regular multiplication operation. CSG is equal to CSR in adjcency matrix. Since adjacency matrix and its tranpose are equal. 

In the beginning, for our cpu parallel version, the parallelization of the DFS and BFS was attempted. In the DFS based solution it was found out that parallelizing a DFS solution is hard and inefficient. We tried task based parallelization to create tasks with each DFS call but due to the nature of the DFS it is not very good performing either. We could not provide any numerical results since the big dataset did not halt with our serial implementation and the example datasets we
created by hand are too small to give a meaningful result. We started to research a better parallelizable solution to our problem and we found out there are some solutions using BFS based solutions. From our research we understand that parallelizing a BFS solution is more common and preferred. To find cycles with small lengths searching the connected vertices to our vertex is required and one can try to reach the starting node with k (length of the cycle) steps. This makes us process all connected nodes and seems to be computationally heavy. 

We realized we do not have to find out all possible circuits with length k to obtain our required result. We just needed to find out for each vertex how many times a vertex is present in a circuit with length k. And the algorithm that has been used in the project was implemented in this manner. Which is using adjacency matrix multiplications to determine the number of each vertex in a circuit. When we take the power of k of a adjacency matrix the resulting matrix is giving the result we need in diagonal entries. aii (subindex) is the 6\*number of circuits containing vertex i for k=3. Using that property we realized that we can write a massively parallel application since sparse-sparse matrix multiplication is one of the main areas of parallel computing and it can be parallelized in a very well manner. An example for our current approach would be the following. (using the graph in Figure 1.1). Adjacency matrix for that graph is shown in Figure 2.1.

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/Figure2_1.png" width="300px" alt="Figure 2.1"/>

For example for k = 4, we have 3 unique cycles (but we found circuits in the actual implementation). 0 - 1 - 2 - 3 , 0 - 1 - 4 - 3, 1 - 2 - 3 - 4
Our output:

        0 - 2
        1 - 3
        2 - 2
        3 - 3
        4 - 2

To use the “multiplication method” we need to multiply our matrix with itself 4 times. The result is shown at the Figure 2.2

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/Figure2_2.png" width="300px" alt="Figure 2.2"/>

To find out how many times 0 vertex is used we used a00 (subindex) which is 12. 

## Technical Description of the Parallel Application

The parallelization of the application is parallelization of the matrix matrix multiplication in abstract terms. For this purpose, OpenMP and Cuda interfaces were utilized for CPU and GPU implementations respectively.

For the CPU implementation with OpenMP, the parallelization was utilized using guided scheduling for the for loop. Guided scheduling over dynamic scheduling was used for convenience in load balancing. Also it has a good overhead / load balancing ratio. For the results of the multiplication, vectors were used as the size that is going to be needed is not known prior to the execution of the multiplication operation. So, for each thread a separate vector is created and results are combined after every thread completes its task.

In the GPU implementation with Cuda, each line in the matrix was assigned to a thread. The thread and block structure is 64 x 1024 where 64 is the amount of the blocks and 1024 is the amount of threads per block. Multiplication operation for each line is executed by a different thread in the structure if the total amount of threads are sufficient for this. In the GPU implementation, instead of vectors, arrays were used as there is no built in implementation of vectos on GPU. Since the amount of the multiplication results is still unknown and a certain amount of memory has to be allocated prior to saving the results, first the amount that is going to be allocated is calculated. After that is determined, multiplication is executed. Also, for the parallelization purpose, since the array that holds the results is shared between the threads, a look up table is used which holds the start and end positions for each thread.

## Technical Description of Software

#### Folder Structure:

images: Some example images of graph representation in matrix.

Misc: Build script

Res: Test graphs represented in txt files.

Src: Source code for GPU and CPU implementation.

#### Description and Guide:

C++ was used as the programming language along with Cuda and OpenMP interfaces. OpenMP, Cuda 10.0 and Gcc 7.5.0 has to be installed before building. To build the code, sh misc/build.sh can be run. Executable will be under /build. To run the executable, ./output.exe graphfile.txt [cycle_length] [num_of_threads] where “output” has to be replaced with the executable name, ”graphfile” has to be replaced with the graph file in question, “cycle_length” is the number of cycles that the program will look for, and “num_of_threads” is the number of parallel threads in CPU. If the number of threads given is 0, the calculation will be executed on GPU. To reproduce the test results;

    g++ ./main.cpp -o a.exe -std=c++14 -O3 -fopenmp
    ./program.out amazon.txt 3 1

has to be run respectively and changing the num_of_threads and cycle_length values. 

## Performance Evaluation

Our previous DFS trial primarily had synchronization overheads. The way DFS handles our problem is by moving one vertex at a time while looking for a cycle, thus a considerable part of the runtime was spent waiting for the previous task to be completed. Although it was not as significant as synchronization, load balancing was also an overhead we had with this model. Unless it is a perfectly balanced graph some branches will be longer than others, which in return leaves us with unbalanced workloads between threads.

In our latest model we are dealing with sparse matrix multiplication. The biggest drawback of this method is that it gets slower as the matrices get denser. For example if our matrix has more 1’s than 0’s it will be considerably slower. Another major overhead is communication which comes with matrix-matrix multiplication. 

<br/>

## amazon.txt

### Circuits of length 3

| #Threads | Runtime | Speedup |
| -------- | ------- | ------- |
| 1        | 0.81    | -       |
| 4        | 0.52    | 1.55    |
| 8        | 0.50    | 1.62    |
| 16       | 0.48    | 1.68    |
| 32       | 0.47    | 1.72    |
| 60       | 0.45    | 1.8     |

<br/>

### Circuits of length 4

| #Threads | Runtime | Speedup |
| -------- | ------- | ------- |
| 1        | 5.60    | -       |
| 4        | 2.17    | 2.55    |
| 8        | 1.84    | 3.04    |
| 16       | 1.49    | 3.75    |
| 32       | 1.41    | 3.97    |
| 60       | 1.32    | 4.24    |

<br/>

### Circuits of length 5

| #Threads | Runtime | Speedup |
| -------- | ------- | ------- |
| 1        | 18.17   | -       |
| 4        | 5.88    | 3.09    |
| 8        | 4.12    | 4.41    |
| 16       | 2.81    | 6.46    |
| 32       | 2.61    | 6.96    |
| 60       | 2.35    | 7.73    |

## dblp.txt

### Circuits of length 3

| #Threads | Runtime | Speedup |
| -------- | ------- | ------- |
| 1        | 1.02    | -       |
| 4        | 0.61    | 1.67    |
| 8        | 0.59    | 1.72    |
| 16       | 0.61    | 1.67    |
| 32       | 0.62    | 1.64    |
| 60       | 0.69    | 1.47    |

<br/>

### Circuits of length 4

| #Threads | Runtime | Speedup |
| -------- | ------- | ------- |
| 1        | 9.83    | -       |
| 4        | 3.90    | 2.52    |
| 8        | 3.21    | 3.06    |
| 16       | 2.75    | 3.58    |
| 32       | 2.72    | 3.61    |
| 60       | 2.70    | 3.64    |

<br/>

### Circuits of length 5

| #Threads | Runtime | Speedup |
| -------- | ------- | ------- |
| 1        | 50.32   | -       |
| 4        | 15.66   | 3.21    |
| 8        | 9.28    | 5.42    |
| 16       | 6.73    | 7.47    |
| 32       | 5.95    | 8.45    |
| 60       | 6.00    | 8.38    |

When it comes to the scaling, our program starts good but as the thread number increases the speed-up rate does not increase proportionally. That is why we think our parallel version has weak scaling.

## GPU Performance:

## amazon.txt

#### GPU - Cycle of 3 => 465.82ms

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/im/Cycle3GPU.png" alt="Gpu 3 Cycle"/>

#### GPU - Cycle of 4 => 2389.72ms

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/im/Cycle4GPU.png" alt="Gpu 3 Cycle"/>

#### GPU - Cycle of 5 => 3124.56ms

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/im/Cycle5GPU.png" alt="Gpu 3 Cycle"/>

## dblp.txt

#### GPU - Cycle of 3 => 1116.0ms

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/im/Cycle3GPU-DBLP.png" alt="Gpu 3 Cycle"/>

#### GPU - Cycle of 4 => 4320.0ms

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/im/Cycle4GPU-DBLP.png" alt="Gpu 3 Cycle"/>

#### GPU - Cycle of 5 => 7896.0ms

<img src="https://raw.githubusercontent.com/kayagokalp/cs406-project/main/images/im/Cycle5GPU-DBLP.png" alt="Gpu 3 Cycle"/>

## Conclusion

Within the scope of the project, parallelization of the algorithm was succeeded, although as it was intended in the beginning, it does not calculate the number of circuits for a vertex i for all i. Rather, it calculates the number of circuits for a vertex i for all i. There seems to be no solutions for that as the algorithm only counts the amount of circuits and does not list the circuits. For finding circuits, performance wise, considerable amount of improvements were recorded both in the CPU and GPU implementations with the parallelization. One of the improvements to be made is implementing the CPU parallelization using dynamic scheduling. Although with guided scheduling the load balacning is good, it has some overheads compared to the dynamic scheduling. With a fine-tuned dynamic scheduling we may have a better performance. For the GPU part we did not see the speed-ups we expected. We were planning to use shared memory for furhter improvement in our GPU performance. Due to time constraints we could not implement that. Also, for calculating cycles instead of circuits, the mentioned parallel BFS algorithm should be considered where from each vertex a BFS search is applied.

## Resources

- [Github Repository](https://github.com/kayagokalp/cs406-project)
- [Dataset](https://github.com/kayagokalp/cs406-project/tree/main/res)

## References

- Cycles of length n in an undirected and connected graph. GeeksforGeeks. (2019, July 24). https://www.geeksforgeeks.org/cycles-of-length-n-in-an-undirected-and-connected-graph/.
- Some interesting properties of adjacency matrices. First Principles. (2008, March 30). https://1stprinciples.wordpress.com/2008/03/30/some-interesting-properties-of-adjacency-matrices/.
- Buluc, A. (n.d.). Parallel Sparse Matrix-Vector and Matrix-Transpose-Vector Multiplication Using Compressed Sparse Blocks.
- TY - JOUR AU - Ahmed, Md Salman AU - Houser, Jennifer AU - Hoque, Mohammad AU - Raju, Rezaul AU - Pfeiffer, Phil PY - 2017/07/01 SP - 46 EP - 59 T1 - Reducing Inter-Process Communication Overhead in Parallel Sparse Matrix-Matrix Multiplication VL - 9 DO - 10.4018/IJGHPC.2017070104 JO - International Journal of Grid and High Performance Computing ER -
