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

const int n_cities = 10;


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** DATA STRUCTURES **************** //
// *************************************************** //

typedef struct tour_t{
    int cities[n_cities + 1];
    int size;
    int cost;
    bool visited[n_cities];
   ~tour_t(){};

} tour_t;



typedef struct strack_t{
    // todo check if stack is n * n ?
    tour_t* list[n_cities * n_cities];
    int size;
    ~strack_t(){}
} strack_t;


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

void print_tour(tour_t* tour);
void print_graph(int* graph);
void copy_tour(tour_t* original, tour_t* target);
tour_t* new_tour();
void free_cities(tour_t* tour);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** FREED TOURS ******************** //
// *************************************************** //

freed_tours_t* new_freed_tour();
void resize_freed_tour(freed_tours_t* freed_tour, int new_size);
void push_freed_tour(freed_tours_t* freed_tours ,tour_t* tour);
tour_t* pop_freed_tour(freed_tours_t* freed_tours);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Stack       ******************** //
// *************************************************** //

strack_t* new_stack();
tour_t* pop(strack_t* stack);
void push_copy(strack_t* stack, tour_t* tour, freed_tours_t* freed_tours);
int get_cost(int* graph, int row, int col);

// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Cities       ******************** //
// *************************************************** //
int get_current_city(tour_t* tour);
void add_city(int* graph, tour_t* tour,  int dest);
void remove_city(int* graph, tour_t* tour);


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** Bool Functions ******************** //
// *************************************************** //
bool is_best_tour(tour_t *current, tour_t *best, int *graph, int home_city);
bool is_neighbor(int* graph, int current_city, int neighbor);
bool is_visited(tour_t* tour, int city);


