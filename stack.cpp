#include <vector>
#include <iostream>
#include <assert.h>
#include "stack.h"
using namespace std;

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** TOUR FUNCTIONS ***************** //
// *************************************************** //
void print_tour(tour_t* tour){
    assert(tour != NULL);
    cout << "Cities: ";
    for(int i =  0; i < tour->size; i++)
        cout << tour->cities[i] << ", " ;
    cout << endl;
    printf("Cost: %d\n", tour->cost);
    printf("Size: %d\n", tour->size);
}

void print_graph(int* graph){
    printf("Printing Graph...\n");
    for(int i = 0; i< n_cities * n_cities; i++){
        if(i % n_cities == 0)
            printf("\n");
        printf("%3d, ", graph[i]);
    }
    printf("\n");
}

void copy_tour(tour_t* original, tour_t* target){
    assert(original != NULL);
    target->size = original->size;
    target->cost = original->cost;
    copy(original->cities, original->cities + original->size, target->cities);
    copy(original->visited, original->visited + n_cities + 1, target->visited);

}

tour_t* new_tour(){
    auto * tour = new tour_t;
    tour->size = 0;
    tour->cost = 0;
    for(int i=0; i< n_cities + 1; i++) {
        tour->cities[i] = -1;
        tour->visited[i] = false;
    }
    return tour;
}

void free_cities(tour_t* tour){
    tour->size = 0;
    tour->cost = 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** FREED TOURS ******************** //
// *************************************************** //
freed_tours_t* new_freed_tour(){
    freed_tours_t* freed_tour = new freed_tours_t;
    freed_tour->size = 0;
    freed_tour->limit = n_cities;
    freed_tour->list = new vector<tour_t*>(freed_tour->limit, NULL);
    return  freed_tour;
}

void resize_freed_tour(freed_tours_t* freed_tour, int new_size){
    assert(freed_tour != NULL);
    freed_tour->list->resize(new_size, NULL);
    freed_tour->limit = new_size;
}

/**
 * Note:
 * We are probably getting rid of the free tour functions because
 * and replacing them with delete and allocate.
 *
 * It doesn't make a big diffence on the serial version but resulted in
 * blocking code for OMP and slow run times
 *
 * See Commented out code for what we were using....
 */
void push_freed_tour(freed_tours_t* freed_tours ,tour_t* tour){
    assert(freed_tours != NULL);
    assert(tour != NULL);
    delete(tour);

//    // Make freed tour larger
//    if(freed_tours->size >= freed_tours->limit)
//        resize_freed_tour(freed_tours, freed_tours->limit + n_cities);
//
//    free_cities(tour);
//    freed_tours->list->push_back(tour);
//    freed_tours->size++;
}

/**
 * Note:
 * We are probably getting rid of the free tour functions because
 * and replacing them with delete and allocate.
 *
 * It doesn't make a big diffence on the serial version but resulted in
 * blocking code for OMP and slow run times
 *
 * See Commented out code for what we were using....
 */
tour_t* pop_freed_tour(freed_tours_t* freed_tours){
//    assert(freed_tours != NULL);
//    tour_t* tour = NULL;
//    if(freed_tours->size > 0){
//        tour = freed_tours->list->back();
//        freed_tours->list->pop_back();
//        freed_tours->size--;
//    } else{
//        tour = new_tour();
//    }
//
//    return tour;

    assert(freed_tours != NULL);
    tour_t* tour = new_tour();
    return tour;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Stack       ******************** //
// *************************************************** //
stack_t* new_stack(){
    stack_t* stack = new stack_t;
    stack->size = 0;
    for(int i=0; i < n_cities + 1; i++)
        stack->list[i] = NULL;
    return stack;
}

tour_t* pop(stack_t* stack){
    assert(stack != NULL);
    assert(stack->size >0);
    return stack->list[--stack->size];
}

void push_copy(stack_t* stack, tour_t* tour, freed_tours_t* freed_tours){
    assert(stack != NULL);
    assert(tour != NULL);
    assert(stack->size < n_cities * n_cities);
    int i = stack->size;
    tour_t* temp = pop_freed_tour(freed_tours);  // Reuse tours or create new one
    copy_tour(tour, temp);
    stack->list[i] = temp;
    stack->size++;
}

int get_cost(int* graph, int row, int col){
    assert(graph != NULL);
    return graph[row * n_cities + col];
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Cities       ******************** //
// *************************************************** //
int get_current_city(tour_t* tour){
    assert(tour != NULL);
    assert(tour->size >0);
    return tour->cities[tour->size - 1 ];
}

void add_city(int* graph, tour_t* tour,  int dest){
    assert(tour != NULL);
    assert(graph != NULL);
    tour->cost += get_cost(graph, get_current_city(tour), dest);
    tour->cities[tour->size] = dest;
    tour->visited[dest] = true;
    tour->size++;
}

void remove_city(int* graph, tour_t* tour){
    assert(graph != NULL);
    assert(tour != NULL);
    assert(tour->size > 1);
    int city = tour->cities[tour->size - 1];
    int cost = get_cost(graph, tour->cities[tour->size - 2], city);
    tour->visited[city] = false;
    tour->cost -= cost;
    tour->size--;
}

bool is_best_tour(tour_t *current, tour_t *best, int *graph, int home_city){
    assert(best != NULL);
    assert(current != NULL);
    assert(graph != NULL);
    bool flag = false;
    int current_city = current->cities[current->size - 1];
    int cost = get_cost(graph, current_city, home_city);
    if((cost > 0)  && (cost + current->cost < best->cost)){
        flag = true;
    }
    return flag;
}

bool is_neighbor(int* graph, int current_city, int neighbor){
    assert(graph != NULL);
    bool flag = false;
    int cost = get_cost(graph, current_city, neighbor);
    if(cost > 0)
        flag = true;
    return  flag;
}

bool is_visited(tour_t* tour, int city){
    assert(tour != NULL);
    assert(city >= 0 );
    assert(city < n_cities);

    bool flag =  false;
    for(int i =0; i< tour->size; i++){
        if(tour->cities[i] == city)
           flag = true;
    }
    return flag;
}