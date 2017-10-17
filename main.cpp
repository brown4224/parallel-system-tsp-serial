#include <iostream>
#include <assert.h>
#include <climits>


using namespace std;
int const n_cities = 4;
typedef struct tour_t {
    int cities[n_cities + 1];  // graph
    int size;  // number of cities
    int cost;  // cost of cities
    ~tour_t(){}  // Deconstrutor

};
typedef struct stack_t {
    // todo check if stack is n * n ?
    tour_t* list[n_cities * n_cities];
    int size;
    ~stack_t(){} // Deconstrutor

};

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
}
void free_cities(tour_t* tour){
    tour->size = 0;
    tour->cost = 0;
}


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

    //todo error handling if size is 0 or neg.
    return stack->list[--stack->size];
}

void push_copy(stack_t* stack, tour_t* tour){
    assert(stack != NULL);
    assert(tour != NULL);
    assert(stack->size < n_cities * n_cities);

    //todo error handling if stack is less then n * n
    int i = stack->size;
    tour_t* temp = new_tour();
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

//int get_city(tour_t* tour, int i){
//    assert(tour != NULL);
//    return tour->cities[i];
//}
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
    int* graph = new int[n_cities * n_cities]
            {
                    0, 1,  3,  8,
                    5, 0,  2,  6,
                    1, 18, 0, 10,
                    7, 4,  12, 0
            };

    tour_t* best_tour = new_tour();
    tour_t* tour = new_tour();
    stack_t* stack = new_stack();
    int home_city = 0;
    tour->cities[0] = home_city;
    tour->size = 1;
    best_tour->cost = INT_MAX;

    push_copy(stack, tour);
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
                        push_copy(stack, tour);
                        remove_city(graph, tour);
                    }

                }
            }
        }
        delete tour;
//        free_cities(tour);  // todo make it dynamic
    }
    print_tour(best_tour);


/*  CODE FROM BOOK */
//    Push copy(stack, cities); // Tour that visits only the hometown
//    while (!Empty(stack)) {
//        curr cities = Pop(stack);
//        if (City count(curr cities) == n) {
//            if (Best cities(curr cities))
//                Update best cities(curr cities);
//        } else {
//            for (nbr = n − 1; nbr >= 1; nbr −− )
//            if (Feasible(curr cities, nbr)) {
//                Add city(curr cities, nbr);
//                Push copy(stack, curr cities);
//                Remove last city(curr cities);
//            }
//        }
//        Free cities(curr cities);
//    }


    delete[] graph;
    delete stack;
    return 0;
}