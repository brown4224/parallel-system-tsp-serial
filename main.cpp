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
#include <mpi.h>
#include "stack.h"
#include "graphs.h"
#include "mpi_tsp.h"
#ifdef _OPENMP

#include <omp.h>
#include <deque>
//#include <cmath>
#include <math.h>
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



int main(int argc, char *argv[]) {
    printf("Starting Program\n");

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
    best_tour->cost = best_tour_cost;
    vector<stack_t1 *> *local_stack = NULL;

    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ******************     MPI     ******************** //
    // *************************************************** //
    //////// MPI  INIT //////////////
    printf("Init: MPI\n");

    mpi_data_t mpi_data;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_data.comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_data.my_rank);
    mpi_data.keep_alive = true;
    mpi_data.alive = true;
    mpi_data.root = 0;
    mpi_data.NEW_COST_TAG = 1;
    mpi_data.bcast_buffer_size = mpi_calculate_buffer_size_integer(mpi_data.comm_sz);  //Used by MPI_Bsend


    stack_t1* stack = scatter_tsp( &mpi_data, graph, best_tour_cost,  best_tour, freed_tours, home_city);
    io_error_handling(&mpi_data);  // ALL Nodes check for error

    if(mpi_data.root == mpi_data.root){
        time_t  timev;
        printf("MPI FINISHED SENDING DATA Time: %ld\n",  time(&timev));
    }



    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ****************** Parallel  ******************** //
    // *************************************************** //

    int z;
# pragma omp parallel  num_threads(thread_count_request) default(none) private(z) shared( mpi_data, graph, stack, local_stack, best_tour, best_tour_cost, home_city, freed_tours)
    {
        /**
         * Create local variables for each thread
         * Private variables denoted: ts_   ==> thread safe
         */
        stack_t1 *ts_stack = NULL;
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

            ts_stack = stack;
            while (ts_stack->size < ts_thread_count) {

                breadth_first_process_stack( graph, ts_stack, &best_tour_cost, ts_best_tour, freed_tours, home_city, &mpi_data);

            }

            io_error_handling(&mpi_data);  // ALL Nodes check for error

            local_stack = new vector<stack_t1 *>(ts_stack->size, NULL);
            for (int j = 0; j < ts_stack->size; j++) {
                local_stack->at(j) = new_stack();
                local_stack->at(j)->list[0] = ts_stack->list[j];
                local_stack->at(j)->size = 1;

            }
            time_t  timev;
            printf("Thread: %d, SYNC Best Cost Time: %ld\n", mpi_data.my_rank, time(&timev));
            mpi_tsp_sync_best_cost(&best_tour_cost, &mpi_data);

            delete ts_stack;



//            time_t  timev;
            printf("Thread: %d, OMP Split data Time: %ld\n", mpi_data.my_rank, time(&timev));
        }



        /**
         * Each thread takes the stack and processes their section
         */
int local_size = local_stack->size();
#pragma omp for schedule(static, local_size / ts_thread_count )
        for (z = 0; z < local_size; z++){
            ts_stack = local_stack->at(z);


            while (ts_stack->size > 0) {
                process_stack(depth_first, graph, ts_stack, &best_tour_cost, ts_best_tour, freed_tours, home_city, &mpi_data);
            }

//            delete ts_stack;
//            time_t  ts_timev;
//            printf("Thread: %d, Process: %d, Getting Next Job at Time %ld\n", mpi_data.my_rank, ts_my_rank, time(&ts_timev) );


        } // End For Loop
        time_t  ts_timev2;
        printf("Thread: %d, Process: %d, Out of work at Time %ld\n", mpi_data.my_rank, ts_my_rank, time(&ts_timev2) );



#pragma omp critical
        {
            if (ts_best_tour->cost > 0 && ts_best_tour->cost < best_tour->cost) {
                copy_tour(ts_best_tour, best_tour, &mpi_data); // Reduce: best tour
            }
        }
                    delete ts_best_tour;  // <--- Fixed

    }


    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ******************     MPI Results****************** //
    // *************************************************** //
    time_t  timev;
    printf("Thread: %d Finished at Time: %ld\n", mpi_data.my_rank, time(&timev));
    io_error_handling(&mpi_data);
    mpi_tsp_print_best_tour( &mpi_data,  best_tour);


    if(mpi_data.my_rank == mpi_data.root){
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

        std::cout << "\nTime taken : " << time_span.count() << " seconds.";
        std::cout << std::endl;

        print_graph(graph);
    }

    mpi_tsp_async_recieve(&mpi_data, &best_tour_cost);  //Recieve old messages
    delete local_stack;
    delete[] graph;
    delete freed_tours;
    delete best_tour;
    MPI_Finalize();
    return 0;
}
