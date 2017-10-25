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
//int const n_cities = 4;

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


    /*
      Time taken : 0.00011 seconds
    */

    int* graph = new int[n_cities * n_cities]
    {
            0, 1,  3,  8,
            5, 0,  2,  6,
            1, 18, 0, 10,
            7, 4,  12, 0,
    };

            /*
    //2nd test case : Symmetric tsp. Time taken : 6.44 x 10^-5 seconds
      int* graph = new int[n_cities * n_cities]
          {
                  0, 29, 82, 46,
                  29, 0, 55, 46,
                  82, 55, 0, 68,
                  46, 46, 68, 0,
          };


      //3rd case : Assymetric tsp with 6 cities. Time taken : 0.00013401
      int* graph = new int[n_cities * n_cities]
      {
            0, 42, 23, 31, 77, 37,
            42, 0, 33, 15, 37, 33,
            23, 33, 0, 29, 62, 46,
            31, 15, 29, 0, 51, 21,
            77, 37, 62, 51, 0, 65,
            37, 33, 46, 21, 65, 0,
      };


// 4th case : Either a. Taking too long or b. Not working for n_cities = 15
    int* graph = new int[n_cities * n_cities]
    {
        0, 29, 82, 46, 68, 52, 72, 42, 51, 55, 29, 74, 23, 72, 46,
        29, 0, 55, 46, 42, 43, 43, 23, 23, 31, 41, 51, 11, 52, 21,
        82, 55, 0, 68, 46, 55, 23, 43, 41, 29, 79, 21, 64, 31, 51,
        46, 46, 68, 0, 82, 15, 72, 31, 62, 42, 21, 51, 51, 43, 64,
        68, 42, 46, 82, 0, 74, 23, 52, 21, 46, 82, 58, 46, 65, 23,
        52, 43, 55, 15, 74, 0, 61, 23, 55, 31, 33, 37, 51, 29, 59,
        72, 43, 23, 72, 23, 61, 0, 42, 23, 31, 77, 37, 51, 46, 33,
        42, 23, 43, 31, 52, 23, 42, 0, 33, 15, 37, 33, 33, 31, 37,
        51, 23, 41, 62, 21, 55, 23, 33, 0, 29, 62, 46, 29, 51, 11,
        55, 31, 29, 42, 46, 31, 31, 15, 29, 0, 51, 21, 41, 23, 37,
        29, 41, 79, 21, 82, 33, 77, 37, 62, 51, 0, 65, 42, 59, 61,
        74, 51, 21, 51, 58, 37, 37, 33, 46, 21, 65, 0, 61, 11, 55,
        23, 11, 64, 51, 46, 51, 51, 33, 29, 41, 42, 61, 0, 62, 23,
        72, 52, 31, 43, 65, 29, 46, 31, 51, 23, 59, 11, 62, 0, 59,
        46, 21, 51, 64, 23, 59, 33, 37, 11, 37, 61, 55, 23, 59, 0,
    };
*/
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
