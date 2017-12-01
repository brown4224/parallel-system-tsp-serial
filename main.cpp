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
 * Run the script "compile_mpi_code.sh" to ensure we are running the same commands.
 * arg 1 : number of mpi nodes
 * arg 2 : number of omp threads.
 * Example : mpiCC -g -Wall -fopenmp main.cpp stack.cpp graphs.cpp mpi_tsp.cpp -o tsp_mpi_code -std=c++0x
            mpiexec -n 10  tsp_mpi_code  4
     OR
            mpirun --hostfile host_file -np 10 tsp_mpi_code 4
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
 * Error Handling : Used functions in the 'mpi_tsp' file.
 *
 *
 * PART 2:  MPI
 *
 * The program takes a single tour in a stack and does a breadth first traversal to generate enough tours for the cluster
 * The tours are then copied to an array which is padded to make delivery manageable,  this is in part due to the nested
 * tour structure data structure.  The Visited City list is rebuilt by the client and the current best tour is distributed to
 * all to the cluster nodes.
 *
 * Throught the algorithm, I am trying to minimize the number of calls to the best tour function.  This is done by
 * distributing the best tour forcefully to all the nodes at the beginning of the algorithm and then later by using async methods.
 *
 * The second phase of the algorithm is to use OMP and again a breadth first method seeds the tour for the other threads.
 * The list is copied to a list which uses the OMP for loop.
 * The best tour is sent to all the nodes via 'mpi all reduce'
 *
 * The OMP threads then process the stack until all work has been complete.  The next tour is given to a tour which is not working
 *
 * Note:  I am currently working on the dynamic mpi portion of the algorithm but I am running into sig faults from the
 * asyn message passing.  Based on my tests, I felt our algorith would benifit more from load balancing MPI then OMP.
 * This is some what of a time management call...
 *
 * Since the last submission I added one line of code to check if the tour is less then the best tour for every loop trip
 *
 *
 * ************************************************************************************************************
 * ************************************************************************************************************
 * ************************************************************************************************************
 * ************************************************************************************************************
 * This is the unfinished dynamic MPI cluster.  It works but is slower then the other version.
 * The algorithm sends and receives several MPI messages using non blocking async message passing.
 *
 *
 *
 * Presentation:  Problems that we ran into
 * 15 MIN
 * What I looked at
 * table of data or mpi/omp
 *
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

#endif

#include <deque>
//#include <cmath>
#include <math.h>
#include <unistd.h>

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


bool is_done(mpi_data_t *mpi_data) {



    /****** Shutdown    ********/

    int counter = mpi_data->comm_sz;


    for (int i = 0; i < mpi_data->comm_sz; i++) {
        if (mpi_data->mpi_need_work_list[i] == 1) {
            --counter;
        }
    }


    if (counter > 0)
        return false;
    else
        return true;

}

bool is_active(mpi_data_t *mpi_data) {



    /****** Shutdown    ********/

    int counter = (int) ceil(mpi_data->comm_sz / (long) 2.0);


    for (int i = 0; i < mpi_data->comm_sz; i++) {
        if (mpi_data->mpi_need_work_list[i] == 1) {
            --counter;
        }
    }


    if (counter > 0)
        return true;
    else
        return false;

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

void process_data(int *graph, int *best_tour_cost, tour_t *best_tour, freed_tours_t *freed_tours, int home_city,
                  mpi_data_t *mpi_data, vector<stack_t1 *> *local_stack, stack_t1 *stack, int thread_count_request,
                  bool flag) {

    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ****************** Parallel  ******************** //
    // *************************************************** //

    int z;
# pragma omp parallel  num_threads(thread_count_request) default(none) private(z) shared( mpi_data, graph, stack, local_stack, best_tour, best_tour_cost, home_city, freed_tours, flag)
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
        int to_send[mpi_data->comm_sz];
        for (int i = 0; i < mpi_data->comm_sz; i++) to_send[i] = 0;
        int to_send_size = 0;
        int load_balance_counter = 0;
        int threshold = 10000;  // only check so many iterations

#pragma omp single
        {

            /**
             * Single thread with implied break statement
             * Creates a stack that will be copied to a global stack and distributed amongst threads
             */

            ts_stack = stack;
            while (ts_stack->size < ts_thread_count && ts_stack->size > 0) {

                breadth_first_process_stack(graph, ts_stack, best_tour_cost, ts_best_tour, freed_tours, home_city,
                                            mpi_data);

            }


            local_stack = new vector<stack_t1 *>(ts_stack->size, NULL);
            for (int j = 0; j < ts_stack->size; j++) {
                local_stack->at(j) = new_stack();
                local_stack->at(j)->list[0] = ts_stack->list[j];
                local_stack->at(j)->size = 1;

            }
            if (flag) {
                mpi_tsp_sync_best_cost(best_tour_cost, mpi_data);
            }


            delete ts_stack;

        }



        /**
         * Each thread takes the stack and processes their section
         */
        int local_size = local_stack->size();
#pragma omp for schedule(static, local_size / ts_thread_count )
        for (z = 0; z < local_size; z++) {
            ts_stack = local_stack->at(z);


            while (ts_stack->size > 0) {


                /* Check for messages and send work */
                if (ts_my_rank == 0) {
                    load_balance_counter++;


                    if (load_balance_counter % threshold == 0) {
                        mpi_tsp_async_recieve(mpi_data, best_tour_cost);
                        mpi_tsp_need_work_async_recieve(mpi_data);
//                        if (is_active(mpi_data)) {

                        //  Group messages in an array
                        to_send_size = 0;
                        for (int i = 0; i < mpi_data->comm_sz; i++) {
                            if (mpi_data->mpi_need_work_list[i] == 1 && mpi_data->my_rank != i) {
                                to_send[to_send_size] = i;
                                to_send_size++;
                            }
                        }


                        // Process array and send a tour if there is work
                        for (int i = 0; i < to_send_size; i++) {
                            if (ts_stack->size > 2) {
                                int dest = to_send[i];
                                // Send tour
                                tour_t *send_tour = breadth_first(ts_stack, mpi_data);
                                if (send_tour->cost < *best_tour_cost) {
                                    mpi_tsp_load_balance_async_send(mpi_data, (int) dest, send_tour);
                                    mpi_tsp_need_work_async_send(mpi_data, (int) dest, 0);
                                    mpi_data->mpi_need_work_list[dest] = 0;

                                }
                                push_freed_tour(freed_tours, send_tour, mpi_data);

                            }
                        }


//                        }
                    }
                }


                //  Continue processing
                if (ts_stack->size > 0) {
                    process_stack(depth_first, graph, ts_stack, best_tour_cost, ts_best_tour, freed_tours,
                                  home_city,
                                  mpi_data);

                }
            }
        } // End For Loop


#pragma omp critical
        {
            if (ts_best_tour->cost > 0 && ts_best_tour->cost < best_tour->cost) {
                copy_tour(ts_best_tour, best_tour, mpi_data); // Reduce: best tour
            }
        }
        delete ts_best_tour;  // <--- Fixed
    }
}


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
    best_tour->cost = best_tour_cost;
    vector<stack_t1 *> *local_stack = NULL;

    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ******************     MPI     ******************** //
    // *************************************************** //
    //////// MPI  INIT //////////////
    mpi_data_t mpi_data;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_data.comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_data.my_rank);
    mpi_data.keep_alive = true;
    mpi_data.alive = true;
    mpi_data.root = 0;
    mpi_data.NEW_COST_TAG = 1;
    mpi_data.NEED_WORK_TAG = 2;
    mpi_data.RECIEVE_LOAD_BALANCE_TAG = 3;
    mpi_data.mpi_need_work_list = new int[mpi_data.comm_sz];
    for (int i = 0; i < mpi_data.comm_sz; i++) mpi_data.mpi_need_work_list[i] = 0;
    attach_buffer(mpi_data.comm_sz);


    stack_t1 *stack = scatter_tsp(&mpi_data, graph, best_tour_cost, best_tour, freed_tours, home_city);
    io_error_handling(&mpi_data);  // ALL Nodes check for error

    // *************************************************** //
    bool done = false;
    bool flag = true;
    while (!done) {
        if (stack->size > 0) {
            //  Process data and check for messages
            process_data(graph, &best_tour_cost, best_tour, freed_tours, home_city, &mpi_data, local_stack, stack,
                         thread_count_request, flag);
            flag = false;


            //MPI NODE Out of work, Notify other nodes
            stack = new_stack();
            mpi_tsp_need_work_async_recieve(&mpi_data);
            mpi_data.mpi_need_work_list[mpi_data.my_rank] = true;
            mpi_tsp_need_work_async_send(&mpi_data, (int) mpi_data.my_rank, 1);  // <-- needs the cast



        } else {
            //  Continue checking for messages and data
            mpi_tsp_need_work_async_recieve(&mpi_data);
            mpi_tsp_load_balance_async_recieve(&mpi_data, stack);

            // Check if cluster is done
            if (stack->size < 1) {
                done = is_done(&mpi_data);

            }
        }
    }

    printf("Node: %d Exiting...\n", mpi_data.my_rank);
    mpi_tsp_need_work_async_send(&mpi_data, (int) mpi_data.my_rank, 1);  // <-- needs the cast
    MPI_Barrier(MPI_COMM_WORLD);

    // +++++++++++++++++++++++++++++++++++++++++++++++++++ //
    // ******************     MPI Results****************** //
    // *************************************************** //

    io_error_handling(&mpi_data);
    mpi_tsp_print_best_tour(&mpi_data, best_tour);


    if (mpi_data.my_rank == mpi_data.root) {
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

        std::cout << "\nTime taken : " << time_span.count() << " seconds.";
        std::cout << std::endl;

        print_graph(graph);
    }


    /* Shutdown gracefully */
    char buf;
    int bsize;
    //Recieve old messages
    mpi_tsp_need_work_async_recieve(&mpi_data);
    mpi_tsp_load_balance_async_recieve(&mpi_data, stack);
    mpi_tsp_async_recieve(&mpi_data, &best_tour_cost);
    MPI_Buffer_detach(&buf, &bsize);
    delete local_stack;
    delete[] graph;
    delete freed_tours;
    delete best_tour;
    MPI_Finalize();
    return 0;
}
