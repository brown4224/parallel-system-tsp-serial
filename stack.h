#include <vector>

using namespace std;

// use control values 4,6,12,15
#pragma once
    const int n_cities = 15;

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** DATA STRUCTURES **************** //
// *************************************************** //
// Struct of tours used by the program
typedef struct tour_t{
    int cities[n_cities + 1];  // graph
    int size;  // number of cities
    int cost;  // cost of cities
    bool visited[n_cities + 1];
   ~tour_t() = default;  // Deconstrutor

} tour_t;


// Struct of stack:  collection of tours
typedef struct stack_t1 {
    tour_t* list[n_cities * n_cities];
    int size;
    ~stack_t1() = default; // Deconstrutor
} stack_t1;


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
     // Deconstrutor
} freed_tours_t;


typedef struct mpi_data_t{
    int comm_sz;  // Number of process
    int my_rank;
    bool keep_alive;
    bool alive;
    int root;
    int NEW_COST_TAG;
    int NEED_WORK_TAG;
    int RECIEVE_LOAD_BALANCE_TAG;
    int* mpi_need_work_list;
    ~mpi_data_t() = default;  // Deconstrutor

} mpi_data_t;


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
void copy_tour(tour_t* original, tour_t* target, mpi_data_t* mpi_data);

// Input  : Takes an instance of tour_t
// Output : No output.
// Zero's out the size and cost of the tour instance passed.
tour_t* new_tour();
void free_cities(tour_t* tour, mpi_data_t* mpi_data);

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
void resize_freed_tour(freed_tours_t* freed_tour, int new_size, mpi_data_t* mpi_data);

// Input  : Takes an instance of freed_tours and tour_t.
// Output : Doesn't have an output.
// Stores partial tours onto a stack.
// NOTE: The book asks us to discard partial tours after checking them for best tour.
void push_freed_tour(freed_tours_t* freed_tours ,tour_t* tour, mpi_data_t* mpi_data);

// Input  : Takes an instance of freed_tours.
// Output : Returns an instance of freed_tours.
// Pop the freed_tour from stack.
tour_t* pop_freed_tour(freed_tours_t* freed_tours, mpi_data_t* mpi_data);


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Stack       ******************** //
// *************************************************** //

// Input  : Doesn't take any inputs.
// Output : Returns a new instance of stack_t.
// Creates new instance of tour stack.
stack_t1* new_stack();

// Input  : Takes an instance of stack_t.
// Output : Returns an instance of tour_t.
// Pops the last tour from stack.
tour_t* depth_first(stack_t1* stack, mpi_data_t* mpi_data);

// Input  : Takes an instance of stack_t.
// Output : Returns an instance of tour_t.
// Pops the First tour from stack.
tour_t* breadth_first(stack_t1* stack, mpi_data_t* mpi_data);


// Input  : Takes an instance of strack, tour_t and freed_tours_t.
// Output : Doesn't return anything.
// Pop's a freed_tour and assigns it to a new instance of tour and copies it to stack.
void push_copy(stack_t1* stack, tour_t* tour, freed_tours_t* freed_tours, mpi_data_t* mpi_data);

// Input  : Takes problem graph, and two integers as input.
// Output : Returns an integer.
// Get cost of travelling.
int get_cost(int* graph, int row, int col, mpi_data_t* mpi_data);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Cities       ******************** //
// *************************************************** //
// Input  : Takes an instance of tour_t.
// Output : Returns an integer.
// Returns the current city on the tour.
int get_current_city(tour_t* tour, mpi_data_t* mpi_data);

// Input  : Takes problem graph, instance of tour_t and an int.
// Output : Doesn't return anything.
// Add a neighbouring city to the stack.
void add_city(int* graph, tour_t* tour,  int dest, mpi_data_t* mpi_data);

// Input  : Takes a graph and an instance of tour_t.
// Output : Doesn't return anything.
// Removes a city from from the partial tour stack.
void remove_city(int* graph, tour_t* tour,mpi_data_t* mpi_data);


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Bool Functions ******************** //
// *************************************************** //


// Input  : Takes two instances of tour_t and two integers.
// Output : Returns a boolean value.
// Checks if the current tour is the best tour.
bool is_best_tour(tour_t *current, int* best_tour_cost, int *graph, int home_city, mpi_data_t* mpi_data);

// Input  : Takes a graph, an two integers.
// Output : Returns a boolean value.
// Checks if there is a path from current city to destination city.
bool is_neighbor(int* graph, int current_city, int neighbor, mpi_data_t* mpi_data);

// Input  : Takes an instance of tour_t and an integer.
// Output : Returns a boolean value.
// Checks if a city has already been added to the stack.
// Note : Needs to be optimized.
bool is_visited(tour_t* tour, int city, mpi_data_t* mpi_data);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Process Stack      ******************** //
// *************************************************** //

// Input:  Takes in all of the data.  Process Stack can take a depth or breadth first function
// Process the stack and runs the algorithm
void process_stack(tour_t*(&pop)(stack_t1 *stack, mpi_data_t* mpi_data ), int *graph, stack_t1 *stack, int* best_tour_cost, tour_t *best_tour, freed_tours_t *freed_tours, int home_city, mpi_data_t* mpi_data);

// Input:  Takes in all of the data.
// Process the stack and runs the algorithm
// Does a breadth first traversal and does not support multi threading
void breadth_first_process_stack( int *graph, stack_t1 *stack, int* best_tour_cost, tour_t *best_tour, freed_tours_t *freed_tours, int home_city, mpi_data_t* mpi_data);
