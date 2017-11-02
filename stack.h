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
 *
 * Description:
 * This program uses C++ for serial program. n_cities is set to 10. It takes the cities from the graph and calculates
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
 *
*/

#include <vector>

using namespace std;


#pragma once
    const int n_cities = 12; // use control values 4, 6, 12, 15


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** DATA STRUCTURES **************** //
// *************************************************** //


// Struct of tours used by the program
typedef struct tour_t{
    int cities[n_cities + 1];
    int size;
    int cost;
    bool visited[n_cities + 1];
   ~tour_t(){};

} tour_t;


// Struct of stack:  collection of tours
typedef struct stack_t{
    tour_t* list[n_cities * n_cities];
    int size;
    ~stack_t(){}
} stack_t;


// Collection of freed memory
// Note: We are probably getting rid of this
// It slowed things down in OMP
typedef struct freed_tours_t{
    vector<tour_t*> *list;
    int size;
    int limit;
     ~freed_tours_t(){
        for(int i= 0; i< size; i++)
            delete(list->at(i));
        delete list;
    }

} freed_tours_t;

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** TOUR FUNCTIONS ***************** //
// *************************************************** //

// Input  : Instance of type tour_t.
// Output : Prints the cost and size of the instance of the tour.
// Prints the cost and size of the optimal solution of the the graph.
void print_tour(tour_t* tour);

// Input  : The problem graph of travelling salesman.
// Output : Prints the entire graph.
// Prints the problem graph of travelling salesman.
void print_graph(int* graph);

// Input  : Takes two instances of tour_t.
// Output : Copies one instance onto another.
// Copies the contents of tour instance onto another.
void copy_tour(tour_t* original, tour_t* target);

// Input  : Doesn't take any inputs.
// Output : Creates a new instance of tour_t.
// Creates a new instance of a tour.
tour_t* new_tour();

// Input  : Takes an instance of tour_t
// Output : No output.
// Zero's out the size and cost of the tour instance passed.
void free_cities(tour_t* tour);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** FREED TOURS ******************** //
// *************************************************** //

// Input  : No input.
// Output : Return's a new instance of freed_tour.
// Generates a freed_tour instance.
freed_tours_t* new_freed_tour();

// Input  : Takes an instance of freed_tour and an integer size.
// Output : Doesn't have an output.
// Resizes the freed_tour to increase freed_tour.
void resize_freed_tour(freed_tours_t* freed_tour, int new_size);

// Input  : Takes an instance of freed_tours and tour_t.
// Output : Doesn't have an output.
// Stores partial tours onto a stack.
// NOTE: The book asks us to discard partial tours after checking them for best tour.
void push_freed_tour(freed_tours_t* freed_tours ,tour_t* tour);

// Input  : Takes an instance of freed_tours.
// Output : Returns an instance of freed_tours.
// Pop the freed_tour from stack.
tour_t* pop_freed_tour(freed_tours_t* freed_tours);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Stack       ******************** //
// *************************************************** //

// Input  : Doesn't take any inputs.
// Output : Returns a new instance of stack_t.
// Creates new instance of tour stack.
stack_t* new_stack();

// Input  : Takes an instance of stack_t.
// Output : Returns an instance of tour_t.
// Pops the last tour from stack.
tour_t* pop(stack_t* stack);

// Input  : Takes an instance of strack, tour_t and freed_tours_t.
// Output : Doesn't return anything.
// Pop's a freed_tour and assigns it to a new instance of tour and copies it to stack.
void push_copy(stack_t* stack, tour_t* tour, freed_tours_t* freed_tours);

// Input  : Takes problem graph, and two integers as input.
// Output : Returns an integer.
// Get cost of travelling.
int get_cost(int* graph, int row, int col);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Cities       ******************** //
// *************************************************** //

// Input  : Takes an instance of tour_t.
// Output : Returns an integer.
// Returns the current city on the tour.
int get_current_city(tour_t* tour);

// Input  : Takes problem graph, instance of tour_t and an int.
// Output : Doesn't return anything.
// Add a neighbouring city to the stack.
void add_city(int* graph, tour_t* tour,  int dest);

// Input  : Takes a graph and an instance of tour_t.
// Output : Doesn't return anything.
// Removes a city from from the partial tour stack.
void remove_city(int* graph, tour_t* tour);


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Bool Functions ******************** //
// *************************************************** //

// Input  : Takes two instances of tour_t and two integers.
// Output : Returns a boolean value.
// Checks if the current tour is the best tour.
bool is_best_tour(tour_t *current, tour_t *best, int *graph, int home_city);

// Input  : Takes a graph, an two integers.
// Output : Returns a boolean value.
// Checks if there is a path from current city to destination city.
bool is_neighbor(int* graph, int current_city, int neighbor);

// Input  : Takes an instance of tour_t and an integer.
// Output : Returns a boolean value.
// Checks if a city has already been added to the stack.
// Note : Needs to be optimized.
bool is_visited(tour_t* tour, int city);


