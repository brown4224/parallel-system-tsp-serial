#include <iostream>
#include <assert.h>
#include <climits>
#include <vector>
//For calculating Time
#include <chrono>
#include <ctime>
#include "stack.h"


using namespace std;
using namespace std::chrono;
//int const n_cities = 4;

int main() {

      high_resolution_clock::time_point t1 = high_resolution_clock::now();
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

    int* graph = new int[n_cities * n_cities];

    graph[0] = 0;

    for(int i = 1 ; i < (n_cities * n_cities), i++)
    {
        temp = rand() %50 + 1;

        if(temp%9 == 0)
        {
            temp = -temp;
        }

        if( ((n_cities + 1) % i) == 0 )
        {
            temp = 0;
        }
        graph[i] = temp;
    }


    OR


    int* lists = new int[graph];
    Make a array with different graphs, use a randomizer to choose a graph.
*/

    tour_t* best_tour = new_tour();
    tour_t* tour = new_tour();
    strack_t* stack = new_stack();
    freed_tours_t* freed_tours = new_freed_tour();
    int home_city = 0;
    tour->cities[0] = home_city;
    tour->size = 1;
    best_tour->cost = INT_MAX;

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
    print_tour(best_tour);


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
