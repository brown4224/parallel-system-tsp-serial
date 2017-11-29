#include <mpi.h>
#include "mpi_tsp.h"
using namespace std;

#ifdef _OPENMP
#include <omp.h>

#endif

void io_error_occur(mpi_data_t* mpi_data){
#pragma omp critical(keep_alive)
    {
        mpi_data->keep_alive = false;

    }

    printf("Abort process Started by MPI Node %d !!!!!\n", mpi_data->my_rank);

}

// Input:  takes a void pointer
// Checks to see if a any threads have had errors.  If error exist, shut down cluster and clear files and memory
void io_error_handling(mpi_data_t* mpi_data){
    MPI_Allreduce(&mpi_data->keep_alive, &mpi_data->alive, 1,MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD );
    if(!(mpi_data->alive)){
        printf("Abort process called.  MPI Node: %d shutting down!\n", mpi_data->my_rank);
        int temp = 0;
        mpi_tsp_async_recieve(mpi_data, &temp);  // Empty received messages
        MPI_Finalize();
        exit(1);
    }
}


// Input: 2 pointers to integer files and cluster root
// Groups values together and broadcast
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


stack_t1* scatter_tsp(mpi_data_t* mpi_data, int* graph, int& best_tour_cost, tour_t* best_tour, freed_tours_t* freed_tours, int home_city){

    // Sending
    int num_sent = 0;
    int stack_size = 0;
    int* stack_sent = NULL;


    if(mpi_data->my_rank == mpi_data->root){
        // Build the list...
        tour_t* ts_tour = new_tour();
        ts_tour->cities[0] = home_city;
        ts_tour->size = 1;
        stack_t1  *stack = new_stack();
        push_copy(stack, ts_tour, freed_tours, mpi_data);


        while (stack->size < mpi_data->comm_sz) {



            breadth_first_process_stack(graph, stack, &best_tour_cost, best_tour, freed_tours, home_city, mpi_data);
        }



        for(int s = 0; s < stack->size; s++){
            if(stack_size < stack->list[s]->size)
                stack_size = stack->list[s]->size;  // Find max stack size
        }


        stack_size += 2;  // offset for cost and size

        // Calculations
        num_sent = (stack->size + mpi_data->comm_sz - 1) / mpi_data->comm_sz;  // Per node
        int arr_size =  stack_size *  num_sent;
        int total_size = arr_size * mpi_data->comm_sz;
        stack_sent = new int [total_size];  // need contigous memory
        for(int j = 0; j< total_size ; j++) {
            stack_sent[j] = 0;  // Zero Array
        }


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

//        int node = 0; // todo do we off set and evenly distribute array????
//        int offset  = 0;
//        for(int s = 0; s < stack->size; s++){
//            int counter = (node * arr_size )+ ( offset * stack_size);
//
//
//            stack_sent[counter] = stack->list[s]->size;
//            counter++;
//
//            stack_sent[counter] = stack->list[s]->cost;
//            counter++;
//            std::copy(stack->list[s]->cities, stack->list[s]->cities + stack_size - 2, stack_sent + counter);
//
//            node++;
//            if(node % mpi_data->comm_sz == 0) {
//                node = 0;
//                offset++;
//            }
//
//        }


        int node = mpi_data->comm_sz - 1; // todo do we off set and evenly distribute array????
        int offset  = 0;
        for(int s = 0; s < stack->size; s++){
            int counter = (node * arr_size )+ ( offset * stack_size);


            stack_sent[counter] = stack->list[s]->size;
            counter++;

            stack_sent[counter] = stack->list[s]->cost;
            counter++;
            std::copy(stack->list[s]->cities, stack->list[s]->cities + stack_size - 2, stack_sent + counter);

            node--;
            if(node % mpi_data->comm_sz == -1) {
                node = mpi_data->comm_sz - 1;
                offset++;
            }

        }

        delete stack;
    }



    build_mpi_data_type(&num_sent, &stack_size, mpi_data->root);
    int list_size = num_sent * stack_size;
    int stack_received [list_size]; // allocate memory
    if (mpi_data->my_rank != mpi_data->root)
        stack_sent = new int[list_size];




    //*  SCATTER  *//

    MPI_Scatter(stack_sent, list_size, MPI_INT, &stack_received, list_size, MPI_INT, mpi_data->root, MPI_COMM_WORLD);
    MPI_Bcast(&best_tour_cost, 1, MPI_INT, mpi_data->root, MPI_COMM_WORLD);


    if(mpi_data->my_rank == mpi_data->root){
        delete[] stack_sent;
    }

    stack_t1* stack = new_stack();
    for(int j = 0; j< num_sent; j++){
        int pos = j * stack_size;
        if(stack_received[pos] > 0){
            stack->size++;
            stack->list[j] = new_tour();
            stack->list[j]->size = stack_received[pos];
            pos++;
            stack->list[j]->cost = stack_received[pos];
            pos++;
            std::copy(stack_received + pos, stack_received + pos + stack_size,  stack->list[j]->cities);

            // Mark Cities as visited
            for(int counter = 0; counter < *stack_received; counter++){

                if(stack->list[j]->cities[counter] >= 0){
                    int temp = stack->list[j]->cities[counter];
                    stack->list[j]->visited[temp] = true;
                }

            }


        } else if(stack_received[pos] > 0){
            printf("An Error occurd Recieving the TSP stack");
            io_error_occur(mpi_data);
            io_error_handling(mpi_data);
        }
        else{
            break;
        }
    }

    return  stack;
}



void mpi_tsp_sync_best_cost(int* best_tour_cost, mpi_data_t* mpi_data){


    int send_best_cost = *best_tour_cost;
    MPI_Allreduce(&send_best_cost, best_tour_cost, 1, MPI_INT, MPI_MIN , MPI_COMM_WORLD);

}
void gather_best_tours(int* results, int size, tour_t* best_tour, int root){
    int local_results[size];
    local_results[0] = best_tour->cost;
    std::copy(best_tour->cities, best_tour->cities + n_cities + 1, local_results + 1);
    MPI_Gather(&local_results, size, MPI_INT, results, size, MPI_INT, root,  MPI_COMM_WORLD);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
int mpi_calculate_buffer_size_integer(int const mpi_comm_size ) {
    int data_size;
    int message_size;


    int size = n_cities + 5;

    MPI_Pack_size(size, MPI_INT, MPI_COMM_WORLD, &data_size); // sets data size
    message_size = data_size + (MPI_BSEND_OVERHEAD * 3);
    return ((mpi_comm_size - 1) * message_size);
}

void attach_buffer(){
    int size = 1024 * 1024* 1024;
    char* buffer= new char [size];
    MPI_Buffer_attach(buffer, size);
}

void mpi_tsp_async_send(mpi_data_t*  mpi_data, int* best_tour_cost){


    int message = *best_tour_cost;


    for(int i=0; i < mpi_data->comm_sz; i++){
        if(mpi_data->my_rank != i){
            MPI_Bsend(&message, 1, MPI_INT, i, mpi_data->NEW_COST_TAG, MPI_COMM_WORLD);
        }

    }


}

void mpi_tsp_async_recieve(mpi_data_t*  mpi_data, int* best_tour_cost){
    int msg_arrive;
    int msg_cost;
    MPI_Status status;

    MPI_Iprobe(MPI_ANY_SOURCE, mpi_data->NEW_COST_TAG, MPI_COMM_WORLD, &msg_arrive, &status);
    while(msg_arrive){
        MPI_Recv(&msg_cost, 1, MPI_INT, status.MPI_SOURCE, mpi_data->NEW_COST_TAG, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        if(msg_cost < *best_tour_cost){
            *best_tour_cost = msg_cost;
        }


        MPI_Iprobe(MPI_ANY_SOURCE, mpi_data->NEW_COST_TAG, MPI_COMM_WORLD, &msg_arrive, &status);
    }
}

void mpi_tsp_print_best_tour(mpi_data_t*  mpi_data,  tour_t *best_tour) {
    int size = n_cities + 2;
    int results[size * mpi_data->comm_sz];
    gather_best_tours(results, size, best_tour, mpi_data->root);


    if (mpi_data->my_rank == mpi_data->root) {

        int min = results[0];
        for (int i = 1; i < mpi_data->comm_sz; i++) {
            int val = i * size;

            if (results[val] < min)
                min = results[val];
        }

        printf("Cost: %d\n", min);
        printf("Size: %d\n", n_cities + 1);


        for (int i = 0; i < mpi_data->comm_sz; i++) {
            int pos = i * size;
            if (results[pos] <= min) {
                pos++;
                printf("Path: ");
                for (int j = 0; j < n_cities + 1; j++)
                    printf("%d, ", results[pos + j]);
            }
        }
    }


}
//////////////////////////////////////////////////////////////////



void mpi_tsp_need_work_async_send(mpi_data_t*  mpi_data, int node, int flag){



    char buf;
    int bsize;
    int* msg = new int [2];
    msg[0]= node;
    msg[1] = flag;


    for(int i=0; i < mpi_data->comm_sz; i++){
        if(mpi_data->my_rank != i){
#pragma omp critical
            {
                MPI_Bsend(msg, 2, MPI_INT, i, mpi_data->NEED_WORK_TAG, MPI_COMM_WORLD);
            }

        }

    }
    delete(msg);


}

void mpi_tsp_need_work_async_recieve(mpi_data_t*  mpi_data){
//    printf("Requesting recieve work message, Node: %d\n", mpi_data->my_rank);
    int msg_arrive;
    int* msg = new int[2];
    msg[0] = 0;
    msg[1] = 0;
    MPI_Request request;
    MPI_Status status;




#pragma omp critical
        {
            MPI_Iprobe(MPI_ANY_SOURCE, mpi_data->NEED_WORK_TAG, MPI_COMM_WORLD, &msg_arrive, &status);
            while(msg_arrive){
            MPI_Recv(msg, 2, MPI_INT, status.MPI_SOURCE, mpi_data->NEED_WORK_TAG, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            int node = msg[0];
            int flag = msg[1];
//            printf("Thread: %d, From: %d, Flag: %d\n", mpi_data->my_rank, node, flag);

            mpi_data->mpi_need_work_list[node] = flag;
//            printf("Thread: %d, NOW From: %d, Flag: %d\n", mpi_data->my_rank, node, mpi_data->mpi_need_work_list[node]);
            MPI_Iprobe(MPI_ANY_SOURCE, mpi_data->NEED_WORK_TAG, MPI_COMM_WORLD, &msg_arrive, &status);

        }

    }
    delete(msg);
}

void mpi_tsp_load_balance_async_send(mpi_data_t*  mpi_data, int* dest, tour_t* tour){
    int size = n_cities + 3;
    int* payload = new int [size];
    for(int i=0; i< size; i++) payload[i] = -1;
    payload[0] = tour->size;
    payload[1] = tour->cost;
    std::copy(tour->cities, tour->cities + size, payload + 2);
//    printf("Node: %d Sending Load Balance from Node: %d\n", mpi_data->my_rank, *dest);

#pragma omp critical
    {
        MPI_Bsend(payload, size, MPI_INT, *dest, mpi_data->RECIEVE_LOAD_BALANCE_TAG, MPI_COMM_WORLD);
    }
    delete(payload);
}

void mpi_tsp_load_balance_async_recieve(mpi_data_t*  mpi_data, stack_t1* stack){

    int msg_arrive;
    MPI_Status status;
    int msg_size = n_cities + 3;
    int* payload = new int[msg_size];
    for(int i=0; i< msg_size; i++) payload[i] = -1;

    MPI_Iprobe(MPI_ANY_SOURCE, mpi_data->RECIEVE_LOAD_BALANCE_TAG , MPI_COMM_WORLD, &msg_arrive, &status);
    while(msg_arrive){
        MPI_Recv(payload, msg_size, MPI_INT, status.MPI_SOURCE, mpi_data->RECIEVE_LOAD_BALANCE_TAG, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

//        printf("Node: %d Receiving Load Balance from Node: %d\n", mpi_data->my_rank, status.MPI_SOURCE);
#pragma omp critical
        {
            tour_t* tour = new_tour();
            tour->size = payload[0];
            tour->cost = payload[1];
            std::copy(payload + 2, payload + 2 + tour->size, tour->cities);
            for(int i=0; i< tour->size; i++){
                int city = tour->cities[i];
                tour->visited[city] = true;
            }
            stack->list[stack->size] = tour;
            stack->size++;
        }



        MPI_Iprobe(MPI_ANY_SOURCE, mpi_data->RECIEVE_LOAD_BALANCE_TAG , MPI_COMM_WORLD, &msg_arrive, &status);
    }
    delete(payload);
}

