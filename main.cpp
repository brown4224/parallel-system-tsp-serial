#include <iostream>
#include <assert.h>
#include <climits>
#include <vector>
//For calculating Time
#include <chrono>
#include <ctime>
#include "stack.h"
#include "graphs.h"

#ifdef _OPENMP

#include <omp.h>
#include <deque>

#endif

using namespace std;
using namespace std::chrono;


void get_rank_thread_count(int *my_rank, int *thread_count) {
#ifdef _OPENMP
    *my_rank = omp_get_thread_num();
    *thread_count = omp_get_num_threads();
#else
    *my_rank = 0;
    *thread_count = 1;
#endif
}

int get_number(char *s) {
    char *temp;
    int number = strtol(s, &temp, 10);
    if (!*s || *temp) {
        cerr << "Invalid number " << *s << '\n';
        exit(1);
    }
    return number;
}

void
process_stack(int *graph, stack_t *stack, tour_t *best_tour, freed_tours_t *freed_tours, int home_city) {
    assert (graph != NULL);
    assert (stack != NULL);
    assert(best_tour != NULL);
    assert(freed_tours != NULL);
    assert(home_city >= 0);
    tour_t* tour = pop(stack);
    if (tour->size == n_cities) {
        if (is_best_tour(tour, best_tour, graph, home_city)) {
            // If best tour add home city and make new best tour
            add_city(graph, tour, home_city);
            copy_tour(tour, best_tour);
        }
    } else {
        for (int neighbor = n_cities - 1; neighbor >= 0; neighbor--) {
            if (is_neighbor(graph, tour->cities[tour->size - 1], neighbor)) {
                if (!is_visited(tour, neighbor)) {  // not in books code
                    add_city(graph, tour, neighbor);
                    push_copy(stack, tour, freed_tours);
                    remove_city(graph, tour);
                }

            }
        }
    }
    push_freed_tour(freed_tours, tour);
}


int main(int argc, char *argv[]) {

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ****************** User Input ******************** //
    // *************************************************** //
    if (argc != 2) {
        cout << "Error Error" << endl;
        cout << "Please provide: Number of Threads" << endl;
        exit(1);
    }
    int thread_count_request = get_number(argv[1]);
    assert(thread_count_request < n_cities - 1);
    assert(n_cities > thread_count_request);   //   <------- ????????????????????????????
    omp_set_dynamic(0);  // Turn off dynamic threads

    int *graph = build_graph(n_cities);
    freed_tours_t *freed_tours = new_freed_tour();

    int home_city = 0;
    tour_t *best_tour = new_tour();
    best_tour->cost = INT_MAX;
    vector<stack_t *> *local_stack = NULL;


    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ****************** Parallel  ******************** //
    // *************************************************** //

    int z;
# pragma omp parallel  num_threads(thread_count_request) default(none) private(z) shared(graph, local_stack, best_tour, home_city, freed_tours)
    {
        /**
         * Create local variables for each thread
         * Private variables denoted: ts_   ==> thread safe
         */
//        tour_t *ts_tour = NULL;  // <-- not needed.  kept in stack now
        stack_t *ts_stack = NULL;
        tour_t *ts_best_tour = new_tour();
        ts_best_tour->cost = INT_MAX;


        int ts_my_rank = 0;
        int ts_thread_count = 1;
        get_rank_thread_count(&ts_my_rank, &ts_thread_count);
#pragma omp single
        {
            /**
             * Single thread with implied break statement
             * Creates a stack that will be copied to a global stack and distributed amongst threads
             */

            tour_t* ts_tour = new_tour();
            ts_tour->cities[0] = home_city;
            ts_tour->size = 1;
            ts_stack = new_stack();
            push_copy(ts_stack, ts_tour, freed_tours);

            while (ts_stack->size < ts_thread_count) {
                process_stack(graph, ts_stack, ts_best_tour, freed_tours, home_city);

            }
            local_stack = new vector<stack_t *>(ts_stack->size, NULL);
            for (int j = 0; j < ts_stack->size; j++) {
                local_stack->at(j) = new_stack();
                local_stack->at(j)->list[0] = ts_stack->list[j];
                local_stack->at(j)->size = 1;
            }
            delete ts_stack;
        }


        /**
         * Each thread takes the stack and processes their section
         */
#pragma omp for schedule(static, local_stack->size() / ts_thread_count )
        for (int z = 0; z < local_stack->size(); z++) {
            ts_stack = local_stack->at(z);


            while (ts_stack->size > 0) {
                process_stack(graph, ts_stack, ts_best_tour, freed_tours, home_city);
            }

#pragma omp critical
            {
                if (ts_best_tour->cost > 0 && ts_best_tour->cost < best_tour->cost) {
                    copy_tour(ts_best_tour, best_tour); // Reduce: best tour
                }
            }
//            delete ts_best_tour;   <--- throwing memory error when delete
            delete ts_stack;

        }
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

    std::cout << "\nTime taken : " << time_span.count() << " seconds.";
    std::cout << std::endl;

    print_tour(best_tour);
    print_graph(graph);

//    for (int i = 0; i < local_stack->size(); i++) delete (local_stack->at(i));  // <--- error on delete
    delete local_stack;
    delete[] graph;
    delete freed_tours;
    delete best_tour;


    return 0;
}
