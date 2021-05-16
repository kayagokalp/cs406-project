parentdir="$(dirname "$(pwd)")"
g++ $parentdir/cs406-project-main/src/main.cpp -o $parentdir/cs406-project-main/build/program.out -std=c++14 -O3 -fopenmp
