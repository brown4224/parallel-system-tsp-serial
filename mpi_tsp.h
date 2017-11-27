#include "stack.h"

// INPUT:  Takes in MPI Struct Pointer
// Changes KEEP Alive flag and begins early shutdown
void io_error_occur(mpi_data_t *mpi_data);

// INPUT:  Takes in MPI Struct Pointer
// Checks if Keep Alive flag has been changed.  Shuts down the stack
void io_error_handling(mpi_data_t *mpi_data);


// Input: 2 pointers to integer files and cluster root
// Groups values together and broadcast
void build_mpi_data_type(int *data_1, int *data_2, int root);

// Input:  Takes in mpi, graph, best tour cost, best tour pointer, freed tours
// Output:  Returns a stack after scatter
//  Takes in a stack and generates enough tours for the cluster
//  The stack is distributed among the cluster a padded array.
// Each node rebuilds the 'Visited' array and returns the new stack for further processing
stack_t1 *
scatter_tsp(mpi_data_t *mpi_data, int *graph, int &best_tour_cost, tour_t *best_tour, freed_tours_t *freed_tours,
            int home_city);


//  Input:  results, size, best tour
// Broadcast the best tour to root node
void gather_best_tours(int *results, int size, tour_t *best_tour, int root);

//  INPUT:  mpi cluster size
//  Calculate the buffer needed for asny message passing
int mpi_calculate_buffer_size_integer(int const mpi_comm_size);


// INPUT:  mpi pointer and best tour cost
//  Asyn receives all best tour messages
void mpi_tsp_async_recieve(mpi_data_t *mpi_data, int *best_tour_cost);


// INPUT:  mpi pointer and best tour cost
//  Asyn  sends best tour cost and continues
void mpi_tsp_async_send(mpi_data_t *mpi_data, int *best_tour_cost);

// INPUT: MPI pointer and best tour
//  Takes the best tours and prints the results
void mpi_tsp_print_best_tour(mpi_data_t *mpi_data, tour_t *best_tour);

// INPUT:  best tour pointer and MPI data
// Sends the cost of the best tour as a reduce all
void mpi_tsp_sync_best_cost(int* best_tour_cost, mpi_data_t* mpi_data);