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
 * arg 1 : number of cities in graph.
 * Example : ./main_program 4
 *
 * Description:
 * This program uses C++ for serial program. It takes the cities from the graph and calculates
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
 * */

#include <iostream>
#include <assert.h>
#include <climits>
#include <vector>
//For calculating Time
#include <chrono>
#include <ctime>
#include "stack.h"
#include "graphs.h"
#include <mpi.h>
#ifdef _OPENMP

#include <omp.h>
#include <deque>
#include <cmath>

#endif

using namespace std;
using namespace std::chrono;


void get_rank_thread_count(int *my_rank, int *thread_count) {
#ifdef _OPENMP
    *my_rank = omp_get_thread_num();
    *thread_count = omp_get_num_threads();
#else
    *my_rank = 0;
    *thread_count = 1;
#endif
}




int get_number(char *s) {
    char *temp;
    int number = strtol(s, &temp, 10);
    if (!*s || *temp) {
        cerr << "Invalid number " << *s << '\n';
        exit(1);
    }
    return number;
}



void process_stack(int *graph, stack_t1 *stack, int* best_tour_cost, tour_t *best_tour, freed_tours_t *freed_tours, int home_city) {
    assert (graph != nullptr);
    assert (stack != nullptr);
    assert(best_tour != nullptr);
    assert(freed_tours != nullptr);
    assert(home_city >= 0);
    tour_t* tour = pop(stack);
    if (tour->size == n_cities) {
        if (is_best_tour(tour, best_tour_cost, graph, home_city)) {
            // If best tour add home city and make new best tour
            add_city(graph, tour, home_city);
#pragma omp critical
            {
                *best_tour_cost = tour->cost;

            }

            copy_tour(tour, best_tour);
        }
    } else {
        for (int neighbor = n_cities - 1; neighbor >= 0; neighbor--) {
            if (is_neighbor(graph, tour->cities[tour->size - 1], neighbor)) {
                if (!is_visited(tour, neighbor)) {  // not in books code
                    add_city(graph, tour, neighbor);
                    push_copy(stack, tour, freed_tours);
                    remove_city(graph, tour);
                }

            }
        }
    }
    push_freed_tour(freed_tours, tour);
}

void build_mpi_data_type(int *data_1, int *data_2, int root) {

    MPI_Datatype custom_type = NULL;

    MPI_Aint data_1_addr, data_2_addr;
    MPI_Get_address(data_1, &data_1_addr);
    MPI_Get_address(data_2, &data_2_addr);

    int array_of_blocklengths[2] = {1, 1};
    MPI_Datatype array_of_types[2] = {MPI_INT, MPI_INT};
    MPI_Aint array_of_displacements[2] = {0, data_2_addr - data_1_addr};
    MPI_Type_create_struct(2, array_of_blocklengths, array_of_displacements, array_of_types, &custom_type);
    MPI_Type_commit(&custom_type);

    MPI_Bcast(data_1, 1, custom_type, root, MPI_COMM_WORLD);
    MPI_Type_free(&custom_type);
}

//vector<stack_t1 *>* list_of_stacks(stack_t1* stack){
//    assert(stack != NULL);
//    assert(stack->size > 0);
//    vector<stack_t1 *> *new_stack = new vector<stack_t1 *>(stack->size, NULL);
//    for (int j = 0; j < stack->size; j++) {
//        new_stack->at(j) = new_stack();
//        new_stack->at(j)->list[0] = stack->list[j];
//        new_stack->at(j)->size = 1;
//    }
//    return new_stack;
//}

//void build_mpi_data_type(stack_t1* stack, int root) {
//
//    assert(stack != NULL);
//    assert(root >= 0);
//    MPI_Datatype custom_type = NULL;
//
//    MPI_Aint size_addr = stack->size;
//    MPI_Aint cost_addr = stack->cost;
//    MPI_Get_address((int) stack->size, &size_addr);
//    MPI_Get_address(stack->cost, &cost_addr);
//
//    int array_of_blocklengths[2] = {1, 1};
//    MPI_Datatype array_of_types[2] = {MPI_INT, MPI_INT};
//    MPI_Aint array_of_displacements[2] = {0, cost_addr - size_addr};
//    MPI_Type_create_struct(2, array_of_blocklengths, array_of_displacements, array_of_types, &custom_type);
//    MPI_Type_commit(&custom_type);
//
//    MPI_Bcast(data_1, 1, custom_type, root, MPI_COMM_WORLD);
//    MPI_Type_free(&custom_type);
//}

int main(int argc, char *argv[]) {

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ****************** User Input ******************** //
    // *************************************************** //
    if (argc != 2) {
        cout << "Error Error" << endl;
        cout << "Please provide: Number of Threads" << endl;
        exit(1);
    }
    int thread_count_request = get_number(argv[1]);
    assert(thread_count_request < n_cities - 1);
    assert(n_cities > thread_count_request);  // Was mentioned in the book, to ensure equal load distribution?
    omp_set_dynamic(0);  // Turn off dynamic threads

    int *graph = build_graph(n_cities);
    freed_tours_t *freed_tours = new_freed_tour();

    int home_city = 0;
    int best_tour_cost = INT_MAX;


    tour_t *best_tour = new_tour();
    best_tour->cost = INT_MAX;
    vector<stack_t1 *> *local_stack = nullptr;

    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ******************     MPI     ******************** //
    // *************************************************** //
    //////// MPI  INIT //////////////

    int comm_sz;  // Number of process
    int my_rank;
    bool keep_alive = true;
    bool alive = true;
    int root = 0;


    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Sending
    int num_sent = 0;
    int stack_size = 0;
    int* stack_sent = nullptr;

    // Recieving
    int* stack_received = nullptr;
//    int num_stack = 0;
//    int stack_size = 0;

    if(my_rank == root){
        // Build the list...
        tour_t* ts_tour = new_tour();
        ts_tour->cities[0] = home_city;
        ts_tour->size = 1;
        stack_t1  *stack = new_stack();
        push_copy(stack, ts_tour, freed_tours);

        while (stack->size < comm_sz) {

            process_stack(graph, stack, &best_tour_cost, best_tour, freed_tours, home_city);

        }
//        int mpi_counter = 0;
//        stack_size = 0;
        for(int s = 0; s < stack->size; s++){
            if(stack_size < stack->list[s]->size)
                stack_size = stack->list[s]->size;  // Find max stack size
        }

        stack_size += 2;  // offset for cost and size

        // Calculations
        num_sent = std::ceil(stack->size / comm_sz);
//        int arr_size =  (2 * num_sent) + (stack_size *  num_sent);
        int arr_size =  stack_size *  num_sent;
        stack_sent = new int [arr_size];  // need contigous memory
        for(int j = 0; j< arr_size ; j++)
            stack_sent[j] = 0;  // Zero Array


//        int mpi_stack_map [stack->size];  // need contigous memory

        /**
         * Build an array to send to all clusters.
         * Make the array continues memory
         * Make the array evenly split between all node.
         *
         * Stack mapping:
         * 1st element:  tour size
         * 2nd element:  tour cost
         * 3rd element  --> (stack_size + 2):  tour cities
         * Tour data structure, init to -1 for cities by default.
         *
         * Note: If tour size is zero, there is no data and end of message.
         */
        int counter = 0;
        int node = 0; // todo do we off set and evenly distribute array????
        for(int s = 0; s < stack->size; s++){

            stack_sent[counter] = stack->list[s]->size;
            counter++;


            stack_sent[counter] = stack->list[s]->cost;
            counter++;

            std::copy(stack->list[s]->cities, stack->list[s]->cities + stack_size, stack_sent + counter);
            counter += stack_size;


//            for(int c = 0; c< stack_size; c++){
//
//                if(c < stack->list[s]->size)
//                    stack_sent[counter] = stack->list[s]->cities[c];
//                counter++;
//            }
        }
        delete stack;
    }
    build_mpi_data_type(&num_sent, &stack_size, root);
    int list_size = num_sent * stack_size;
    stack_received  = new int[list_size]; // allocate memory
    if (my_rank != root)
        stack_sent = new int[list_size];
    MPI_Scatter(stack_sent, list_size, MPI_INT, stack_received, list_size, MPI_INT, root, MPI_COMM_WORLD);
    if(my_rank == root){
        delete[] stack_sent;
    }

    stack_t1* stack = new_stack();
//    tour_t* tour = nullptr;
    for(int j = 0; j< num_sent; j++){
        int pos = j * (stack_size +2);
        if(stack_received[pos] > 0){
            stack->size++;
            stack->list[j] = new_tour();
            stack->list[j]->size = stack_received[pos];
            pos++;
            stack->list[j]->cost = stack_received[pos];
            pos++;
//            std::copy(stack_received[pos], stack_received[pos + stack_size],  stack->list[j]->cities[0]);
            std::copy(stack_received + pos, stack_received + pos + stack_size,  stack->list[j]->cities);
            stack->list[j]->cities;
        } else{
            break;
        }
    }
    delete[] stack_received;

//    for(int j = 0; j< list_size; j++){
//        if(j % stack_size == 0)
//            tour_t* tour = new_tour();
//
//        if(stack_received[j] > -1){
//            tour     stack_received[j%stack_size]
//        }
//
//
//    }

//    delete[] stack_received;


//    stack_t1* stack = new_stack();
//    for(int j = )



//    MPI_Datatype custom_type = NULL;




    // Build data type...
    // ScatterV





    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ****************** Parallel  ******************** //
    // *************************************************** //

    int z;
# pragma omp parallel  num_threads(thread_count_request) default(none) private(z) shared(graph, stack, local_stack, best_tour, best_tour_cost, home_city, freed_tours)
    {
        /**
         * Create local variables for each thread
         * Private variables denoted: ts_   ==> thread safe
         */
//        tour_t *ts_tour = NULL;  // <-- not needed.  kept in stack now
        stack_t1 *ts_stack = nullptr;
        tour_t *ts_best_tour = new_tour();
        ts_best_tour->cost = INT_MAX;




        int ts_my_rank = 0;
        int ts_thread_count = 1;
        get_rank_thread_count(&ts_my_rank, &ts_thread_count);
#pragma omp single
        {
            /**
             * Single thread with implied break statement
             * Creates a stack that will be copied to a global stack and distributed amongst threads
             */

//            tour_t* ts_tour = new_tour();
//            ts_tour->cities[0] = home_city;
//            ts_tour->size = 1;
//            ts_stack = new_stack();
//            push_copy(ts_stack, ts_tour, freed_tours);

            ts_stack = stack;
            while (ts_stack->size < ts_thread_count) {

                process_stack(graph, ts_stack, &best_tour_cost, ts_best_tour, freed_tours, home_city);

            }
//            local_stack = list_of_stacks(ts_stack);
            local_stack = new vector<stack_t1 *>(ts_stack->size, NULL);
            for (int j = 0; j < ts_stack->size; j++) {
                int temp_size = ts_stack->list[j]->size;
                printf("List size: %d\n", ts_stack->list[j]->size);

                for(int i = 0 ; i < temp_size; i++){
                    printf("J: %d,  City: %d\n",j, ts_stack->list[j]->cities[i]);
                }
                local_stack->at(j) = new_stack();
                local_stack->at(j)->list[0] = ts_stack->list[j];
                local_stack->at(j)->size = 1;
            }

            delete ts_stack;
        }


        /**
         * Each thread takes the stack and processes their section
         */
#pragma omp for schedule(static, local_stack->size() / ts_thread_count )
        for (int z = 0; z < local_stack->size(); z++) {
            ts_stack = local_stack->at(z);


            while (ts_stack->size > 0) {
                process_stack(graph, ts_stack, &best_tour_cost, ts_best_tour, freed_tours, home_city);
            }

            delete ts_stack;

        } // End For Loop


        // This is where thread needs to ask for jobs

#pragma omp critical
        {
            if (ts_best_tour->cost > 0 && ts_best_tour->cost < best_tour->cost) {
                copy_tour(ts_best_tour, best_tour); // Reduce: best tour
            }
        }
                    delete ts_best_tour;  // <--- Fixed

    }

    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ******************     MPI     ******************** //
    // *************************************************** //
    int size = n_cities + 2;
    int results [size * comm_sz];
    int local_results[size];
    local_results[0] = best_tour->cost;
    std::copy(best_tour->cities, best_tour->cities + n_cities + 1, local_results + 1);
    MPI_Gather(&local_results, size, MPI_INT,results,size, MPI_INT, root,  MPI_COMM_WORLD);

    if(my_rank == root){
        int min = results[0];
        for(int i = 1; i< comm_sz; i++){
            if(results[i * (n_cities + 2)] < min)
                min = results[i];
        }

        printf("Cost: %d\n", min);
        printf("Size: %d\n", n_cities +1);
        for(int i = 1; i< comm_sz; i++){
            int pos = i * (n_cities + 2);
            if(results[ pos ] == min){
                //print
                pos++;
                printf("\n");
                for(int j = 0; j< n_cities + 1; j++)
                    printf("%d, ", results[pos + j]);
            }
        }
    }








    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

    std::cout << "\nTime taken : " << time_span.count() << " seconds.";
    std::cout << std::endl;

    print_tour(best_tour);
    print_graph(graph);

//    for (int i = 0; i < local_stack->size(); i++) delete (local_stack->at(i));  // <--- error on delete
    delete local_stack;
    delete[] graph;
    delete freed_tours;
    delete best_tour;
    MPI_Finalize();


    return 0;
}
