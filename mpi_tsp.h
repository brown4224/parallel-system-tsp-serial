#include "stack.h"

void io_error_occur(mpi_data_t* mpi_data);
void io_error_handling(mpi_data_t* mpi_data);
void build_mpi_data_type(int *data_1, int *data_2, int root);
stack_t1* scatter_tsp(mpi_data_t* mpi_data, int* graph, int& best_tour_cost, tour_t* best_tour, freed_tours_t* freed_tours, int home_city);
void gather_best_tours(int* results, int size, tour_t* best_tour, int root);
