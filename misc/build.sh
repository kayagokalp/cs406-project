parentdir="$(dirname "$(pwd)")"
nvcc -c $parentdir/cs406-project/src/kernel.cu
g++ $parentdir/cs406-project/src/main.cpp -o $parentdir/cs406-project/build/program.out -std=c++14 -O3 -fopenmp -L/usr/local/cuda/lib64 -lcuda -lcudart kernel.o

