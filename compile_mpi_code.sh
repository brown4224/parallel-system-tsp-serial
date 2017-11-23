#!bin/bash
mpiCC -g -Wall -fopenmp main.cpp stack.cpp graphs.cpp mpi_tsp.cpp -o tsp_mpi_code -std=c++0x
mpiexec -n 4  tsp_mpi_code  4
mpirun --hostfile host_file -np 10 tsp_mpi_code 4
