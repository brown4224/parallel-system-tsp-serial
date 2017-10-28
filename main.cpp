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
process_stack(int *graph, stack_t *stack, tour_t *tour, tour_t *best_tour, freed_tours_t *freed_tours, int home_city) {
    tour = pop(stack);
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
    omp_set_dynamic(0);  // Turn off dynamic threads


    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ****************** Serial ******************** //
    // *************************************************** //


    int *graph = build_graph(n_cities);
    freed_tours_t *freed_tours = new_freed_tour();

    int home_city = 0;
    tour_t *best_tour = new_tour();
    best_tour->cost = INT_MAX;
    vector<stack_t *> *city_list = NULL;

    int z;
# pragma omp parallel  num_threads(thread_count_request) default(none) private(z) shared(graph, city_list, best_tour, home_city, freed_tours)
    {
        tour_t *tour = NULL;
        stack_t *stack = NULL;
        tour_t *ts_best_tour = new_tour();
        ts_best_tour->cost = INT_MAX;


        int my_rank = 0;
        int thread_count = 1;
        get_rank_thread_count(&my_rank, &thread_count);
#pragma omp single
        {

            tour = new_tour();
            tour->cities[0] = home_city;
            tour->size = 1;
            stack = new_stack();
            push_copy(stack, tour, freed_tours);

            while (stack->size < thread_count) {
//                process_stack(graph, stack, tour, best_tour, freed_tours, home_city);

                tour = pop(stack);
                if (tour->size == n_cities) {
                    if (is_best_tour(tour, ts_best_tour, graph, home_city)) {
                        // If best tour add home city and make new best tour
                        add_city(graph, tour, home_city);
                        copy_tour(tour, ts_best_tour);
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
            city_list = new vector<stack_t *>(stack->size);
            for (int j = 0; j < stack->size; j++) {
                for (int i = 0; i < stack->list[j]->size; i++)
                    city_list->at(j) = new_stack();
                city_list->at(j)->list[0] = stack->list[j];
                city_list->at(j)->size = 1;
            }
        }


#pragma omp for
        for (int z = 0; z < city_list->size(); z++) {
            stack = city_list->at(z);


            while (stack->size > 0) {
//                process_stack(graph, stack, tour, ts_best_tour, freed_tours, home_city);

                tour = pop(stack);
                if (tour->size == n_cities) {
                    if (is_best_tour(tour, ts_best_tour, graph, home_city)) {
                        // If best tour add home city and make new best tour
                        add_city(graph, tour, home_city);
                        copy_tour(tour, ts_best_tour);
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
        }

#pragma omp critical
        {
            if (ts_best_tour->cost > 0 &&  ts_best_tour->cost < best_tour->cost) {
                copy_tour(ts_best_tour, best_tour); // Reduce: best tour
            }
        }
        delete ts_best_tour;
        delete tour;
        delete stack;

    }


//                tour = pop(stack);
//                if(tour->size == n_cities){
//#pragma omp critical
//                {
//                    if(is_best_tour(tour, best_tour, graph, home_city)) {
//
//                        // If best tour add home city and make new best tour
//                        add_city(graph, tour, home_city);
//                        copy_tour(tour, best_tour);
//                    }
//                }
//
//
//                } else{
//                    for(int neighbor = n_cities -1; neighbor >= 0; neighbor--){
//                        if(is_neighbor(graph, tour->cities[tour->size - 1], neighbor)){
//                            if(!is_visited(tour, neighbor)){  // not in books code
//                                add_city(graph, tour, neighbor );
//                                push_copy(stack, tour, freed_tours);
//                                remove_city(graph, tour);
//                            }
//
//                        }
//                    }
//                }
//                push_freed_tour(freed_tours, tour);
//            }
//
//
//
//        }
//        delete freed_tours;

//        delete tour;
//        delete stack;










    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

    std::cout << "\nTime taken : " << time_span.count() << " seconds.";
    std::cout << std::endl;

    print_tour(best_tour);
    print_graph(graph);

    for (int i = 0; i < city_list->size(); i++) delete (city_list->at(i));
    delete city_list;
    delete[] graph;
    delete freed_tours;
    delete best_tour;


    return 0;
}
