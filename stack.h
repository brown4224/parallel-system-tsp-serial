#include <vector>

using namespace std;

// use control values 4,6,12,15
#pragma once
    const int n_cities = 12;


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** DATA STRUCTURES **************** //
// *************************************************** //

typedef struct tour_t{
    int cities[n_cities + 1];  // graph
    int size;  // number of cities
    int cost;  // cost of cities
    bool visited[n_cities + 1];
   ~tour_t() = default;  // Deconstrutor

} tour_t;


// Conflicting typedef name with some xcode stuff
typedef struct stack_t1 {
    // todo check if stack is n * n ?
    tour_t* list[n_cities * n_cities];
    int size;
    int start;
//    int end;
    ~stack_t1() = default; // Deconstrutor
} stack_t1;



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


// todo make a struct.h  ???
typedef struct mpi_data_t{
    int comm_sz;  // Number of process
    int my_rank;
    bool keep_alive;
    bool alive;
    int root;
//    int bcast_buffer_size;
//    int bcast_buffer_need_work_size;
//    int bcast_buffer_tour_size;
    int NEW_COST_TAG;
    int NEED_WORK_TAG;
    int RECIEVE_LOAD_BALANCE_TAG;
    int* mpi_need_work_list;
    ~mpi_data_t() = default;  // Deconstrutor

} mpi_data_t;


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** TOUR FUNCTIONS ***************** //
// *************************************************** //

void print_tour(tour_t* tour);
void print_graph(int* graph);
void copy_tour(tour_t* original, tour_t* target, mpi_data_t* mpi_data);
tour_t* new_tour();
void free_cities(tour_t* tour, mpi_data_t* mpi_data);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** FREED TOURS ******************** //
// *************************************************** //

freed_tours_t* new_freed_tour();
void resize_freed_tour(freed_tours_t* freed_tour, int new_size, mpi_data_t* mpi_data);
void push_freed_tour(freed_tours_t* freed_tours ,tour_t* tour, mpi_data_t* mpi_data);
tour_t* pop_freed_tour(freed_tours_t* freed_tours, mpi_data_t* mpi_data);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Stack       ******************** //
// *************************************************** //

stack_t1* new_stack();
tour_t* depth_first(stack_t1* stack, mpi_data_t* mpi_data);
tour_t* breadth_first(stack_t1* stack, mpi_data_t* mpi_data);
void push_copy(stack_t1* stack, tour_t* tour, freed_tours_t* freed_tours, mpi_data_t* mpi_data);
int get_cost(int* graph, int row, int col, mpi_data_t* mpi_data);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Cities       ******************** //
// *************************************************** //
int get_current_city(tour_t* tour, mpi_data_t* mpi_data);
void add_city(int* graph, tour_t* tour,  int dest, mpi_data_t* mpi_data);
void remove_city(int* graph, tour_t* tour,mpi_data_t* mpi_data);


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Bool Functions ******************** //
// *************************************************** //
bool is_best_tour(tour_t *current, int* best_tour_cost, int *graph, int home_city, mpi_data_t* mpi_data);
bool is_neighbor(int* graph, int current_city, int neighbor, mpi_data_t* mpi_data);
bool is_visited(tour_t* tour, int city, mpi_data_t* mpi_data);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Process Stack      ******************** //
// *************************************************** //
void process_stack(tour_t*(&pop)(stack_t1 *stack, mpi_data_t* mpi_data ), int *graph, stack_t1 *stack, int* best_tour_cost, tour_t *best_tour, freed_tours_t *freed_tours, int home_city, mpi_data_t* mpi_data);
void breadth_first_process_stack( int *graph, stack_t1 *stack, int* best_tour_cost, tour_t *best_tour, freed_tours_t *freed_tours, int home_city, mpi_data_t* mpi_data);
