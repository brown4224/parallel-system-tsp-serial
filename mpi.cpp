#include <iostream>
#include <assert.h>
#include <climits>
#include <vector>
//For calculating Time
#include <chrono>
#include <ctime>
#include <mpi>
#include "stack.h"


using namespace std;
using namespace std::chrono;

typedef struct {

        // MPI Variables
        int my_rank;
        int comm_sz;

        int* graph;
        int* local_graph;

        freed_tours_t* freed_tours;
        strack_t* stack;
        tour*t tour;
        tour* best_tour;

} data;

void get_mpi_custom_data_type(freed_tour_t* datatype1, strack_t* datatype2, tour_t* datatype3, tour_t* datatype4)
{
    // Create custom data type here and broadcast data.
}

int main(int argc, char* argv[]) {

        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        // Possibly take cities from user?
        assert(argc == 3);

        data node;
        tour_t* best_tour = new_tour();
        tour_t* tour = new_tour();
        strack_t* stack = new_stack();
        freed_tours_t* freed_tours = new_freed_tour();
        int home_city = 0;
        tour->cities[0] = home_city;
        tour->size = 1;
        best_tour->cost = INT_MAX;

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &node.comm_sz);
        MPI_Comm_rank(MPI_COMM_WORLD, &node.rank);



    if(node.my_rank == 0)
    {
        get_mpi_custom_data_type(&freed_tours, &stack, &tour, &best_tour);
    }

    // Scatter the graph?
    MPI_Scatter();

    push_copy(stack, tour, freed_tours);
    while (stack->size > 0){
        tour = pop(stack);
        if(tour->size == n_cities){
            if(is_best_tour(tour, best_tour, graph, home_city)) {
                // If best tour add home city and make new best tour
                add_city(graph, tour, home_city);
                copy_tour(tour, best_tour);
            }
        } else{
            for(int neighbor = n_cities -1; neighbor >= 0; neighbor--){  //  book uses int neighbor; neighbor >= 1; neighbor--)  Possible bug??
                if(is_neighbor(graph, tour->cities[tour->size - 1], neighbor)){
                    if(!is_visited(tour, neighbor)){  // not in books code
                        add_city(graph, tour, neighbor );
                        push_copy(stack, tour, freed_tours);
                        remove_city(graph, tour);
                    }

                }
            }
        }
        push_freed_tour(freed_tours, tour);
    }
    MPI_Reduce();

    if(my_rank == 0)
    {
        print_tour(best_tour);
    }

    delete[] graph;
    delete freed_tours;
    delete best_tour;
    delete tour;
    delete stack;

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

    std::cout << "\nTime taken : " << time_span.count() << " seconds.";
    std::cout << std::endl;
    return 0;
}
