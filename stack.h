
#include <vector>

using namespace std;

// use control values 4,6,15
const int n_cities = 10;


// +++++++++++++++++++++++++++++++++++++++++++++++++++ //
// ****************** DATA STRUCTURES **************** //
// *************************************************** //

typedef struct tour_t{
    int cities[n_cities + 1];  // graph
    int size;  // number of cities
    int cost;  // cost of cities
   ~tour_t(){};  // Deconstrutor

} tour_t;


// Conflicting typedef name with some xcode stuff
typedef struct strack_t{
    // todo check if stack is n * n ?
    tour_t* list[n_cities * n_cities];
    int size;
    ~strack_t(){} // Deconstrutor
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
     // Deconstrutor
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


