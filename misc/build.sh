parentdir="$(dirname "$(pwd)")"
nvcc -c $parentdir/cs406-project-main/src/kernel.cu
g++ $parentdir/cs406-project-main/src/main.cpp -o $parentdir/cs406-project-main/build/program.out -std=c++14 -O3 -fopenmp -L/usr/local/cuda/lib64 -lcuda -lcudart kernel.o