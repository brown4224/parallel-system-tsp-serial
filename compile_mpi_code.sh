#!bin/bash
mpiCC -g -Wall -fopenmp main.cpp stack.cpp graphs.cpp -o tsp_mpi_code -std=c++0x
mpiexec -n 2  tsp_mpi_code  2
