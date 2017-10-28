#include <iostream>
#include <assert.h>
#include <climits>
#include <vector>
//For calculating Time
#include <chrono>
#include <ctime>
#include <omp>
#include "stack.h"
#include "graphs.h"


using namespace std;
using namespace std::chrono;

int get_number(char* s)
{
    char* temp;
    int number = strtol(s, &temp, 10);
    return number;
}

int main(int argc, char* argv[]) {

      high_resolution_clock::time_point t1 = high_resolution_clock::now();
    /*
      Time taken : 0.00011 seconds
    */


    int* graph = build_graph(n_cities);

    int thread_count = get_number(argv[1]);

    \\ theads have to be less than cities - 1;
    assert(thread_count < n_cities-1);


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
        //
        #pragma omp critical
        //
        if(tour->size == n_cities){
            if(is_best_tour(tour, best_tour, graph, home_city)) {
                // If best tour add home city and make new best tour
                add_city(graph, tour, home_city);
                copy_tour(tour, best_tour);
            }
        } else{
            /*
                I haven't run this? But this seems okay in parallel. The book asks us to split it into two stacks, which seems weird to me.
            */
            #pragma omp parallel for num_threads(n_cities - 1) shared(graph) private(tour)
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
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

    std::cout << "\nTime taken : " << time_span.count() << " seconds.";
    std::cout << std::endl;

    print_tour(best_tour);
    print_graph(graph);


    delete[] graph;
    delete freed_tours;
    delete best_tour;
    delete tour;
    delete stack;



    return 0;
}
