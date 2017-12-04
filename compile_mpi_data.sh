#!bin/bash
mpiCC -g -Wall -fopenmp main.cpp stack.cpp graphs.cpp mpi_tsp.cpp -o tsp_mpi_code -std=c++0x
for node_count in 100 80 60 40 20 10 5 1
do


for thread_count in 32 16 8 4
 do
mpirun --hostfile host_file -np $node_count --bynode  tsp_mpi_code $thread_count >> results.txt
echo "###################################################"
cat results.txt

done

done
