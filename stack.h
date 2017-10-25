#include <vector>
#include <iostream>
#include <assert.h>

using namespace std;

int const n_cities = 4;


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** DATA STRUCTURES **************** //
// *************************************************** //

typedef struct {
    int cities[n_cities + 1];  // graph
    int size;  // number of cities
    int cost;  // cost of cities
   //~tour_t();  // Deconstrutor

} tour_t;


// Conflicting typedef name with some xcode stuff
typedef struct {
    // todo check if stack is n * n ?
    tour_t* list[n_cities * n_cities];
    int size;
    //~strack_t() // Deconstrutor
} strack_t;


typedef struct {
    vector<tour_t*> *list;
    int size;
    int limit;
    /* ~freed_tours_t(){
        for(int i= 0; i< size; i++)
            delete(list->at(i));
        delete list;
    }
    */ // Deconstrutor
} freed_tours_t;


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

void copy_tour(tour_t* original, tour_t* target){
    assert(original != NULL);
    target->size = original->size;
    target->cost = original->cost;
    for(int i=0; i < original->size; i++)
        target->cities[i] = original->cities[i];
}

tour_t* new_tour(){
    tour_t* tour = new tour_t;
    tour->size = 0;
    tour->cost = 0;
    for(int i=0; i< n_cities; i++)
        tour->cities[i] = -1;
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

strack_t* new_stack(){
    strack_t* stack = new strack_t;
    stack->size = 0;
    for(int i=0; i < n_cities + 1; i++)
        stack->list[i] = NULL;
    return stack;
}

tour_t* pop(strack_t* stack){
    assert(stack != NULL);
    assert(stack->size >0);

    //todo error handling if size is 0 or neg.
    return stack->list[--stack->size];
}

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

int get_cost(int* graph, int row, int col){
    assert(graph != NULL);
    return graph[row * n_cities + col];
}
int get_current_city(tour_t* tour){
    assert(tour != NULL);
    assert(tour->size >0);

    //todo write error handling for size
    return tour->cities[tour->size - 1 ];
}


void add_city(int* graph, tour_t* tour,  int dest){
    assert(tour != NULL);
    assert(graph != NULL);
    tour->cost += get_cost(graph, get_current_city(tour), dest);
    tour->cities[tour->size] = dest;
    tour->size++;

}
void remove_city(int* graph, tour_t* tour){
    assert(graph != NULL);
    assert(tour != NULL);
    assert(tour->size > 1);

    // todo Why are we removing it????
    // todo Can it be coded better then book???
    int cost = get_cost(graph, tour->cities[tour->size - 2], tour->cities[tour->size - 1]);
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
    if(cost > 0  & cost + current->cost < best->cost){  // is cost to home better then current best
        flag = true;
    }

    return flag;
}

bool is_neighbor(int* graph, int current_city, int neighbor){
    assert(graph != NULL);
    bool flag = false;
    int cost = get_cost(graph, current_city, neighbor);
    if(cost > 0)        //This should cover -1?
        flag = true;
    return  flag;
}

bool is_visited(tour_t* tour, int city){
    assert(tour != NULL);
    assert(city >= 0 );
    assert(city < n_cities);

    // TODO  Time consuming.  See if we have already visited this city

    /*
        Maybe this for checking visited city?

    if (stack->size > tour->size)
        return flag;
    */

    bool  flag = false;
    for(int i =0; i< tour->size; i++){
        if(tour->cities[i] == city)
            flag = true;
    }
    return flag;
}