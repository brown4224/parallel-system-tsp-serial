#!bin/bash
#  sudo yum install -y install valgrind kcachegrind graphviz
#  https://baptiste-wicht.com/posts/2011/09/profile-c-application-with-callgrind-kcachegrind.html
mpiCC -g -Wall -fopenmp main.cpp stack.cpp graphs.cpp mpi_tsp.cpp -o tsp_mpi_code -std=c++0x
valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes   mpiexec -n 1  tsp_mpi_code  1

 kcachegrind
