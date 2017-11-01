#include <vector>
#include <iostream>
#include <assert.h>
#include "stack.h"
using namespace std;

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** TOUR FUNCTIONS ***************** //
// *************************************************** //


// Input  : Instance of type tour_t.
// Output : Prints the cost and size of the instance of the tour.
// Prints the cost and size of the optimal solution of the the graph.
void print_tour(tour_t* tour){
    assert(tour != NULL);

    cout << "Cities: ";
    for(int i =  0; i < tour->size; i++)
        cout << tour->cities[i] << ", " ;
    cout << endl;
    printf("Cost: %d\n", tour->cost);
    printf("Size: %d\n", tour->size);
}

// Input  : The problem graph of travelling salesman.
// Output : Prints the entire graph.
// Prints the problem graph of travelling salesman.
void print_graph(int* graph){
    printf("Printing Graph...\n");
    for(int i = 0; i< n_cities * n_cities; i++){
        if(i % n_cities == 0)
            printf("\n");
        printf("%3d, ", graph[i]);
    }
    printf("\n");
}

// Input  : Takes two instances of tour_t.
// Output : Copies one instance onto another.
// Copies the contents of tour instance onto another.
void copy_tour(tour_t* original, tour_t* target){
    assert(original != NULL);
    target->size = original->size;
    target->cost = original->cost;
    for(int i=0; i < original->size; i++)
    {
        target->cities[i] = original->cities[i];
        target->visited[i] = original->visited[i];
    }
}


// Input  : Doesn't take any inputs.
// Output : Creates a new instance of tour_t.
// Creates a new instance of a tour.
tour_t* new_tour(){
    auto * tour = new tour_t;
    tour->size = 0;
    tour->cost = 0;
    for(int i=0; i< n_cities; i++) {
        tour->cities[i] = -1;
    }
    return tour;
}

// Input  : Takes an instance of tour_t
// Output : No output.
// Zero's out the size and cost of the tour instance passed.
void free_cities(tour_t* tour){
    tour->size = 0;
    tour->cost = 0;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** FREED TOURS ******************** //
// *************************************************** //

// Input  : No input.
// Output : Return's a new instance of freed_tour.
// Generates a freed_tour instance.
freed_tours_t* new_freed_tour(){
    freed_tours_t* freed_tour = new freed_tours_t;
    freed_tour->size = 0;
    freed_tour->limit = n_cities;
    freed_tour->list = new vector<tour_t*>(freed_tour->limit, NULL);

    return  freed_tour;
}

// Input  : Takes an instance of freed_tour and an integer size.
// Output : Doesn't have an output.
// Resizes the freed_tour to increase freed_tour.
void resize_freed_tour(freed_tours_t* freed_tour, int new_size){
    assert(freed_tour != NULL);
    freed_tour->list->resize(new_size, NULL);
    freed_tour->limit = new_size;
}

// Input  : Takes an instance of freed_tours and tour_t.
// Output : Doesn't have an output.
// Stores partial tours onto a stack.
// NOTE: The book asks us to discard partial tours after checking them for best tour.
void push_freed_tour(freed_tours_t* freed_tours ,tour_t* tour){
    assert(freed_tours != NULL);
    assert(tour != NULL);

    // Make freed tour larger
    if(freed_tours->size >= freed_tours->limit)
        resize_freed_tour(freed_tours, freed_tours->limit + n_cities);

    // zero out tour & push on array
    free_cities(tour);
    freed_tours->list->push_back(tour);
    freed_tours->size++;
}

// Input  : Takes an instance of freed_tours.
// Output : Returns an instance of freed_tours.
// Pop the freed_tour from stack.
tour_t* pop_freed_tour(freed_tours_t* freed_tours){
    assert(freed_tours != NULL);
    tour_t* tour = NULL;
    if(freed_tours->size > 0){
        tour = freed_tours->list->back();
        freed_tours->list->pop_back();
        freed_tours->size--;
    } else{
        tour = new_tour();
    }

    return tour;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Stack       ******************** //
// *************************************************** //

// Input  : Doesn't take any inputs.
// Output : Returns a new instance of strack_t.
// Creates new instance of tour stack.
strack_t* new_stack(){
    strack_t* stack = new strack_t;
    stack->size = 0;
    for(int i=0; i < n_cities + 1; i++)
        stack->list[i] = NULL;
    return stack;
}

// Input  : Takes an instance of strack_t.
// Output : Returns an instance of tour_t.
// Pops the last tour from stack.
tour_t* pop(strack_t* stack){
    assert(stack != NULL);
    assert(stack->size >0);

    return stack->list[--stack->size];
}

// Input  : Takes an instance of strack, tour_t and freed_tours_t.
// Output : Doesn't return anything.
// Pop's a freed_tour and assigns it to a new instance of tour and copies it to stack.
void push_copy(strack_t* stack, tour_t* tour, freed_tours_t* freed_tours){
    assert(stack != NULL);
    assert(tour != NULL);
    assert(stack->size < n_cities * n_cities);

    //todo error handling if stack is less then n * n
    int i = stack->size;
    tour_t* temp = pop_freed_tour(freed_tours);  // Reuse tours or create new one
    copy_tour(tour, temp);
    stack->list[i] = temp;
    stack->size++;
}

// Input  : Takes problem graph, and two integers as input.
// Output : Returns an integer.
// Get cost of travelling.
int get_cost(int* graph, int row, int col){
    assert(graph != NULL);
    return graph[row * n_cities + col];
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Cities       ******************** //
// *************************************************** //

// Input  : Takes an instance of tour_t.
// Output : Returns an integer.
// Returns the current city on the tour.
int get_current_city(tour_t* tour){
    assert(tour != NULL);
    assert(tour->size >0);
    return tour->cities[tour->size - 1 ];
}

// Input  : Takes problem graph, instance of tour_t and an int.
// Output : Doesn't return anything.
// Add a neighbouring city to the stack.
void add_city(int* graph, tour_t* tour,  int dest){
    assert(tour != NULL);
    assert(graph != NULL);
    tour->cost += get_cost(graph, get_current_city(tour), dest);
    tour->cities[tour->size] = dest;
    tour->size++;
    //int temp = get_current_city(tour);
    //tour->visited[tour->size] = true;
}

// Input  : Takes a graph and an instance of tour_t.
// Output : Doesn't return anything.
// Removes a city from from the partial tour stack.
void remove_city(int* graph, tour_t* tour){
    assert(graph != NULL);
    assert(tour != NULL);
    assert(tour->size > 1);

    int cost = get_cost(graph, tour->cities[tour->size - 2], tour->cities[tour->size - 1]);
    tour->cost -= cost;
    tour->size--;
}

// Input  : Takes two instances of tour_t and two integers.
// Output : Returns a boolean value.
// Checks if the current tour is the best tour.
bool is_best_tour(tour_t *current, tour_t *best, int *graph, int home_city){
    assert(best != NULL);
    assert(current != NULL);
    assert(graph != NULL);
    bool flag = false;
    int current_city = current->cities[current->size - 1];
    int cost = get_cost(graph, current_city, home_city);
    if((cost > 0)  && (cost + current->cost < best->cost)){  // is cost to home better then current best
        flag = true;
    }

    return flag;
}

// Input  : Takes a graph, an two integers.
// Output : Returns a boolean value.
// Checks if there is a path from current city to destination city.
bool is_neighbor(int* graph, int current_city, int neighbor){
    assert(graph != NULL);
    bool flag = false;
    int cost = get_cost(graph, current_city, neighbor);
    if(cost > 0)        //This should cover -1?
        flag = true;
    return  flag;
}

// Input  : Takes an instance of tour_t and an integer.
// Output : Returns a boolean value.
// Checks if a city has already been added to the stack.
// Note : Needs to be optimized.
bool is_visited(tour_t* tour, int city){
    assert(tour != NULL);
    assert(city >= 0 );
    assert(city < n_cities);

    // TODO  Time consuming.  See if we have already visited this city

    /*
        Maybe this for checking visited city?
        ***THIS WOULD NOT WORK, WE ARE NOT COMPARING THE SIZES IN THE CODE BELOW****

    if (stack->size > tour->size)
        return flag;
    */

    bool flag =  false;

//    if(tour->visited[city])
  //      flag = true;
    //bool  flag = false;
    for(int i =0; i< tour->size; i++){
        if(tour->cities[i] == city)
           flag = true;
    }
    return flag;
}
