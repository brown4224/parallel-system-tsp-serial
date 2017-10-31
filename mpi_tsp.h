#include <mpi.h>

#include "stack.h"
#include "graphs.h"
#include <vector>


using namespace std;

//#ifndef TSP_MPI_TSP_H
//#define TSP_MPI_TSP_H
//
//
//
//
//
//#endif //TSP_MPI_TSP_H

// Input: 2 pointers to integer files and cluster root
// Groups values together and broadcast
void build_mpi_data_type(int *data_1, int *data_2, int root) {

    MPI_Datatype custom_type = NULL;

    MPI_Aint data_1_addr, data_2_addr;
    MPI_Get_address(data_1, &data_1_addr);
    MPI_Get_address(data_2, &data_2_addr);

    int array_of_blocklengths[2] = {1, 1};
    MPI_Datatype array_of_types[2] = {MPI_INT, MPI_INT};
    MPI_Aint array_of_displacements[2] = {0, data_2_addr - data_1_addr};
    MPI_Type_create_struct(2, array_of_blocklengths, array_of_displacements, array_of_types, &custom_type);
    MPI_Type_commit(&custom_type);

    MPI_Bcast(data_1, 1, custom_type, root, MPI_COMM_WORLD);
    MPI_Type_free(&custom_type);
}


stack_t1* scatter_tsp(int root, int my_rank, int comm_sz, int* graph, int& best_tour_cost, tour_t* best_tour, freed_tours_t* freed_tours, int home_city){

    // Sending
    int num_sent = 0;
    int stack_size = 0;
    int* stack_sent = nullptr;


    if(my_rank == root){
        // Build the list...
        tour_t* ts_tour = new_tour();
        ts_tour->cities[0] = home_city;
        ts_tour->size = 1;
        stack_t1  *stack = new_stack();
        push_copy(stack, ts_tour, freed_tours);

        while (stack->size < comm_sz) {

            process_stack(graph, stack, &best_tour_cost, best_tour, freed_tours, home_city);

        }
        for(int s = 0; s < stack->size; s++){
            if(stack_size < stack->list[s]->size)
                stack_size = stack->list[s]->size;  // Find max stack size
        }


        stack_size += 2;  // offset for cost and size

        // Calculations
        num_sent = (stack->size + comm_sz - 1) / comm_sz;  // Per node
        int arr_size =  stack_size *  num_sent;
        int total_size = arr_size * comm_sz;
        stack_sent = new int [total_size];  // need contigous memory
        for(int j = 0; j< total_size ; j++)
            stack_sent[j] = 0;  // Zero Array



        /**
         * Build an array to send to all clusters.
         * Make the array continues memory
         * Make the array evenly split between all node.
         *
         * Stack mapping:
         * 1st element:  tour size
         * 2nd element:  tour cost
         * 3rd element  --> (stack_size + 2):  tour cities
         * Tour data structure, init to -1 for cities by default.
         *
         * Note: If tour size is zero, there is no data and end of message.
         */

        int node = 0; // todo do we off set and evenly distribute array????
        int offset  = 0;
        for(int s = 0; s < stack->size; s++){
            int counter = (node * arr_size )+ ( offset * stack_size);


            stack_sent[counter] = stack->list[s]->size;
            counter++;

            stack_sent[counter] = stack->list[s]->cost;
            counter++;
            std::copy(stack->list[s]->cities, stack->list[s]->cities + stack_size - 2, stack_sent + counter);

            node++;
            if(node % comm_sz == 0) {
                node = 0;
                offset++;
            }

        }

        delete stack;
    }



    build_mpi_data_type(&num_sent, &stack_size, root);
    int list_size = num_sent * stack_size;
    int stack_received [list_size]; // allocate memory
    if (my_rank != root)
        stack_sent = new int[list_size];




    //*  SCATTER  *//
    MPI_Scatter(stack_sent, list_size, MPI_INT, &stack_received, list_size, MPI_INT, root, MPI_COMM_WORLD);
    int tmp [list_size];


    if(my_rank == root){
        delete[] stack_sent;
    }

    stack_t1* stack = new_stack();
    for(int j = 0; j< num_sent; j++){
        int pos = j * stack_size;
        if(stack_received[pos] > 0){
            stack->size++;
            stack->list[j] = new_tour();
            stack->list[j]->size = stack_received[pos];
            pos++;
            stack->list[j]->cost = stack_received[pos];
            pos++;
            std::copy(stack_received + pos, stack_received + pos + stack_size,  stack->list[j]->cities);
            stack->list[j]->cities;
        }
        else{
            break;
        }
    }

    return  stack;
}


void gather_best_tours(int* results, tour_t* best_tour, int root){
    int size = n_cities + 2;
    int local_results[size];

    local_results[0] = best_tour->cost;
    std::copy(best_tour->cities, best_tour->cities + n_cities + 1, local_results + 1);
    MPI_Gather(&local_results, size, MPI_INT, results, size, MPI_INT, root,  MPI_COMM_WORLD);
}

