#include <iostream>
#include <assert.h>
#include <climits>
#include <vector>
//For calculating Time
#include <chrono>
#include <ctime>


using namespace std;
using namespace std::chrono;
int const n_cities = 15;


/* I refactored the typedef struct definitions.
  Code works fine right now.
  I'l add the deconstructors after I test for other arrays.
  I'm gonna try to add a clock and check the time increase for different types of arrays.
  Should I use chronos for the clock or regular clock_t works fine?
*/

typedef struct {
    int cities[n_cities + 1];  // graph
    int size;  // number of cities
    int cost;  // cost of cities
   // ~tour_t(){}  // Deconstrutor

} tour_t;


// Conflicting typedef name with some xcode stuff
typedef struct {
    // todo check if stack is n * n ?
    tour_t* list[n_cities * n_cities];
    int size;
    // ~strack_t(){} // Deconstrutor
} strack_t;


typedef struct {
    vector<tour_t*> *list;
    int size;
    int limit;
    /* ~freed_tours_t(){
        for(int i= 0; i< size; i++)
            delete(list->at(i));
        delete list;
    } // Deconstrutor
    */
} freed_tours_t;

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
    if(cost > 0)
        flag = true;
    return  flag;
}

bool is_visited(tour_t* tour, int city){
    assert(tour != NULL);
    assert(city >= 0 );
    assert(city < n_cities);

    // TODO  Time consuming.  See if we have already visited this city
    bool  flag = false;
    for(int i =0; i< tour->size; i++){
        if(tour->cities[i] == city)
            flag = true;
    }
    return flag;
}


int main() {

      high_resolution_clock::time_point t1 = high_resolution_clock::now();
    /*
      Original graph

      I'm thinking of adding a for loop here and feed all the int* graph's as inputs to it, instead of testing
      for each graph individually.

      Time taken : 0.00011 seconds


    int* graph = new int[n_cities * n_cities]
            {
                    0, 1,  3,  8,
                    5, 0,  2,  6,
                    1, 18, 0, 10,
                    7, 4,  12, 0
            };


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
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

    std::cout << "\nTime taken : " << time_span.count() << " seconds.";
    std::cout << std::endl;
    return 0;
}
