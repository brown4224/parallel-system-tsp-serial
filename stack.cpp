#include <vector>
#include <iostream>
//#include <assert.h>
#include "stack.h"
#include "mpi_tsp.h"
using namespace std;

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** TOUR FUNCTIONS ***************** //
// *************************************************** //


void print_tour(tour_t *tour) {
//    assert(tour != NULL);

    cout << "Cities: ";
    for (int i = 0; i < tour->size; i++)
        cout << tour->cities[i] << ", ";
    cout << endl;
    printf("Cost: %d\n", tour->cost);
    printf("Size: %d\n", tour->size);
}

void print_graph(int *graph) {
    printf("Printing Graph...\n");
    for (int i = 0; i < n_cities * n_cities; i++) {
        if (i % n_cities == 0)
            printf("\n");
        printf("%3d, ", graph[i]);
    }
    printf("\n");
}

void copy_tour(tour_t *original, tour_t *target, mpi_data_t* mpi_data) {
    if(original == NULL){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }
//    assert(original != NULL);
    target->size = original->size;
    target->cost = original->cost;
    std::copy(original->cities, original->cities + original->size, target->cities);

}

tour_t *new_tour() {
    tour_t *tour = new tour_t;
    tour->size = 0;
    tour->cost = 0;
    for (int i = 0; i < n_cities; i++)
        tour->cities[i] = -1;
    return tour;
}

void free_cities(tour_t *tour, mpi_data_t* mpi_data) {
    if(tour == NULL){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }
    tour->size = 0;
    tour->cost = 0;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** FREED TOURS ******************** //
// *************************************************** //


freed_tours_t *new_freed_tour() {
    freed_tours_t *freed_tour = new freed_tours_t;
    freed_tour->size = 0;
    freed_tour->limit = n_cities;
    freed_tour->list = new vector<tour_t *>(freed_tour->limit, NULL);

    return freed_tour;
}

void resize_freed_tour(freed_tours_t *freed_tour, int new_size, mpi_data_t* mpi_data) {

    if(freed_tour == NULL){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }
//    assert(freed_tour != NULL);
    freed_tour->list->resize(new_size, NULL);
    freed_tour->limit = new_size;
}

void push_freed_tour(freed_tours_t *freed_tours, tour_t *tour,mpi_data_t* mpi_data) {
    delete (tour);
//    assert(freed_tours != NULL);
//    assert(tour != NULL);
//#pragma omp critical
//    {
//        // Make freed tour larger
//        if(freed_tours->size >= freed_tours->limit)
//            resize_freed_tour(freed_tours, freed_tours->limit + n_cities);
//
//        // zero out tour & push on array
//        free_cities(tour);
//        freed_tours->list->push_back(tour);
//        freed_tours->size++;
//    }

}

tour_t *pop_freed_tour(freed_tours_t *freed_tours, mpi_data_t* mpi_data) {
    if(freed_tours == NULL){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }

//    assert(freed_tours != NULL);
    tour_t *tour = NULL;

//#pragma omp critical
//    {
//        if (freed_tours->size > 0) {
//
//            tour = freed_tours->list->back();
//            freed_tours->list->pop_back();
//            freed_tours->size--;
//
//        } else {
//            tour = new_tour();
//        }
//    }
    tour = new_tour();

    return tour;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Stack       ******************** //
// *************************************************** //

stack_t1 *new_stack() {
    stack_t1 *stack = new stack_t1;
    stack->size = 0;
    for (int i = 0; i < n_cities + 1; i++)
        stack->list[i] = NULL;
    return stack;
}

tour_t *pop(stack_t1 *stack, mpi_data_t* mpi_data) {

    if(stack == NULL || stack->size <= 0){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }
//    assert(stack != NULL);
//    assert(stack->size > 0);

    return stack->list[--stack->size];
}


void push_copy(stack_t1 *stack, tour_t *tour, freed_tours_t *freed_tours, mpi_data_t* mpi_data) {


    if(stack == NULL || tour == NULL || stack->size > n_cities * n_cities){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }


//    assert(stack != NULL);
//    assert(tour != NULL);
//    assert(stack->size < n_cities * n_cities);

    //todo error handling if stack is less then n * n
    int i = stack->size;
    tour_t *temp = pop_freed_tour(freed_tours, mpi_data);  // Reuse tours or create new one
    copy_tour(tour, temp, mpi_data);
    stack->list[i] = temp;
    stack->size++;
}

int get_cost(int *graph, int row, int col, mpi_data_t* mpi_data) {
    if(graph == NULL){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }
//    assert(graph != NULL);
    return graph[row * n_cities + col];
}



// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Cities       ******************** //
// *************************************************** //
int get_current_city(tour_t *tour, mpi_data_t* mpi_data) {

    if(tour == NULL || tour->size <= 0){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }
//    assert(tour != NULL);
//    assert(tour->size > 0);
    return tour->cities[tour->size - 1];
}


void add_city(int *graph, tour_t *tour, int dest, mpi_data_t* mpi_data) {
    if(tour == NULL || graph == NULL){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }
//    assert(tour != NULL);
//    assert(graph != NULL);
    tour->cost += get_cost(graph, get_current_city(tour, mpi_data), dest, mpi_data);
    tour->cities[tour->size] = dest;
    tour->size++;

}

void remove_city(int *graph, tour_t *tour, mpi_data_t* mpi_data) {
    if(graph == NULL || tour == NULL || tour->size < 1){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }


//    assert(graph != NULL);
//    assert(tour != NULL);
//    assert(tour->size > 1);

    int cost = get_cost(graph, tour->cities[tour->size - 2], tour->cities[tour->size - 1], mpi_data);
    tour->cost -= cost;
    tour->size--;
}

bool is_best_tour(tour_t *current, int* best_tour_cost, int *graph, int home_city, mpi_data_t* mpi_data) {
    if(best_tour_cost == NULL || current == NULL || graph == NULL){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }

//    assert(best_tour_cost != NULL);
//    assert(current != NULL);
//    assert(graph != NULL);
    bool flag = false;
    int current_city = current->cities[current->size - 1];
    int cost = get_cost(graph, current_city, home_city, mpi_data);
//    if ((cost > 0) && (cost + current->cost < best->cost)) {  // is cost to home better then current best
    if ((cost > 0) && (cost + current->cost < *best_tour_cost)) {  // is cost to home better then current best
        flag = true;
    }

    return flag;
}

bool is_neighbor(int *graph, int current_city, int neighbor, mpi_data_t* mpi_data) {

    if(graph == NULL){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }

//    assert(graph != NULL);
    bool flag = false;
    int cost = get_cost(graph, current_city, neighbor, mpi_data);
    if (cost > 0)        //This should cover -1?
        flag = true;
    return flag;
}

bool is_visited(tour_t *tour, int city, mpi_data_t* mpi_data) {

    if(tour == NULL || city < 0 || city >= n_cities) {
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }
//    assert(tour != NULL);
//    assert(city >= 0);
//    assert(city < n_cities);

    // TODO  Time consuming.  See if we have already visited this city

    /*
        Maybe this for checking visited city?
        ***THIS WOULD NOT WORK, WE ARE NOT COMPARING THE SIZES IN THE CODE BELOW****

    if (stack->size > tour->size)
        return flag;
    */

    bool flag = false;
    for (int i = 0; i < tour->size; i++) {
        if (tour->cities[i] == city)
            flag = true;
    }
    return flag;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Process Stack      ******************** //
// *************************************************** //

void process_stack(int *graph, stack_t1 *stack, int* best_tour_cost, tour_t *best_tour, freed_tours_t *freed_tours, int home_city, mpi_data_t* mpi_data) {
    if(graph == nullptr ||  stack == nullptr || best_tour == nullptr || freed_tours == nullptr || home_city < 0){
        io_error_occur(mpi_data);
        io_error_handling(mpi_data);
    }

//    assert (graph != nullptr);
//    assert (stack != nullptr);
//    assert(best_tour != nullptr);
//    assert(freed_tours != nullptr);
//    assert(home_city >= 0);
//
    tour_t* tour = pop(stack, mpi_data);
    if ( tour->size == n_cities) {
        if (is_best_tour(tour, best_tour_cost, graph, home_city, mpi_data)) {
            // If best tour add home city and make new best tour
            add_city(graph, tour, home_city, mpi_data);
#pragma omp critical
            {
                *best_tour_cost = tour->cost;

            }

            copy_tour(tour, best_tour, mpi_data);
        }
    } else {
        for (int neighbor = n_cities - 1; neighbor >= 0; neighbor--) {
            if (is_neighbor(graph, tour->cities[tour->size - 1], neighbor, mpi_data)) {
                if (!is_visited(tour, neighbor, mpi_data)) {  // not in books code
                    add_city(graph, tour, neighbor, mpi_data);
                    push_copy(stack, tour, freed_tours, mpi_data);
                    remove_city(graph, tour, mpi_data);
                }

            }
        }
    }
    push_freed_tour(freed_tours, tour, mpi_data);



}