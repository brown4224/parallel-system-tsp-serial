/*
 *
 * Sean Mcglincy and Kshitij Bantupalli
 * Parallel and Distributed Systems
 * Project (Serial Version)
 *
 * Dependencies : We are using CLION as an IDE Dependencies: I am using CLION as an IDE which uses CMAKE 3.8, and GCC, C+11
 * gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-11)
 * Running on Cento 7 and MacOSX 10.23
 *
 * Program:
 * Running the file:
 * Run the script "compile_serial_code.sh" to ensure we are running the same commands.
 * arg 1 : number of cities in graph.
 * Example : ./main_program 4
 *
 * Description:
 * This program uses C++ for serial program. It takes the cities from the graph and calculates
 * partial tours. When the number of cities in stack reaches n_cities, it compares it to best_tour.
 * Subsequent comparision lead to the best_tour to be most optimal.
 *
 * The Algorithm
 * 1: The graph is generated and home_city is set as 0 and size is 1.
 *
 * 2: Partial tours are generated.
 *
 * 3: best_tour is updated.
 *
 * Error Handling : Used assert() in functions to ensure correct data is being passed.
 * */

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

void process_stack(int *graph, stack_t1 *stack, tour_t *best_tour, freed_tours_t *freed_tours, int home_city) {
    assert (graph != nullptr);
    assert (stack != nullptr);
    assert(best_tour != nullptr);
    assert(freed_tours != nullptr);
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
    assert(n_cities > thread_count_request);  // Was mentioned in the book, to ensure equal load distribution?
    omp_set_dynamic(0);  // Turn off dynamic threads

    int *graph = build_graph(n_cities);
    freed_tours_t *freed_tours = new_freed_tour();

    int home_city = 0;
    tour_t *best_tour = new_tour();
    best_tour->cost = INT_MAX;
    vector<stack_t1 *> *local_stack = nullptr;


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
        stack_t1 *ts_stack = nullptr;
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
            local_stack = new vector<stack_t1 *>(ts_stack->size, NULL);
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
