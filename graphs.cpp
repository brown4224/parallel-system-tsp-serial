#include "graphs.h"
#include <iostream>
#include <assert.h>


int * build_graph(const int  n_cities){
    assert(n_cities > 0);
    int* graph;

    if(n_cities == 4){
        graph = graph_1();
        //        graph = graph_2();
    }

    else if(n_cities == 6){
        graph = graph_3();

    }
    else if(n_cities == 15){
        graph = graph_4();

    } else{
        graph = graph_random(n_cities);
    }

    return graph;

}

/*
  Time taken : 0.00011 seconds
*/



int* graph_random(int const n_cities){

    const int size = n_cities * n_cities;
    int* graph = new int[size];

//    graph[0] = 0;

    srand (time(NULL));

    for (int j = 1; j < size; ++j) {
        int temp =  rand() %50 + 1;

        if(temp%9 == 0)
        {
//            temp = -temp;
            temp = -1;
        }

//        if( ((n_cities ) % j) == 0 )  // <-- logic error?
//        {
//            temp = 0;
//        }
        graph[j] = temp;


    }

    for (int j = 0; j < size; j += n_cities + 1 ) {
        graph[j] = 0;
    }

    return graph;


/*
OR

int* lists = new int[graph];
Make a array with different graphs, use a randomizer to choose a graph.
*/


}

int* graph_1() {
    return new (int[16])
            {
                    0, 1,  3,  8,
                    5, 0,  2,  6,
                    1, 18, 0, 10,
                    7, 4,  12, 0
            };
}

int* graph_2() {
    //2nd test case : Symmetric tsp. Time taken : 6.44 x 10^-5 seconds
    return new int[16]
            {
                    0, 29, 82, 46,
                    29, 0, 55, 46,
                    82, 55, 0, 68,
                    46, 46, 68, 0,
            };
}

int* graph_3() {
    //3rd case : Assymetric tsp with 6 cities. Time taken : 0.00013401
    return new int[36]
            {
                    0, 42, 23, 31, 77, 37,
                    42, 0, 33, 15, 37, 33,
                    23, 33, 0, 29, 62, 46,
                    31, 15, 29, 0, 51, 21,
                    77, 37, 62, 51, 0, 65,
                    37, 33, 46, 21, 65, 0,
            };
}

int* graph_4() {
    // 4th case : Either a. Taking too long or b. Not working for n_cities = 15
    return new int[255]
            {
                    0, 29, 82, 46, 68, 52, 72, 42, 51, 55, 29, 74, 23, 72, 46,
                    29, 0, 55, 46, 42, 43, 43, 23, 23, 31, 41, 51, 11, 52, 21,
                    82, 55, 0, 68, 46, 55, 23, 43, 41, 29, 79, 21, 64, 31, 51,
                    46, 46, 68, 0, 82, 15, 72, 31, 62, 42, 21, 51, 51, 43, 64,
                    68, 42, 46, 82, 0, 74, 23, 52, 21, 46, 82, 58, 46, 65, 23,
                    52, 43, 55, 15, 74, 0, 61, 23, 55, 31, 33, 37, 51, 29, 59,
                    72, 43, 23, 72, 23, 61, 0, 42, 23, 31, 77, 37, 51, 46, 33,
                    42, 23, 43, 31, 52, 23, 42, 0, 33, 15, 37, 33, 33, 31, 37,
                    51, 23, 41, 62, 21, 55, 23, 33, 0, 29, 62, 46, 29, 51, 11,
                    55, 31, 29, 42, 46, 31, 31, 15, 29, 0, 51, 21, 41, 23, 37,
                    29, 41, 79, 21, 82, 33, 77, 37, 62, 51, 0, 65, 42, 59, 61,
                    74, 51, 21, 51, 58, 37, 37, 33, 46, 21, 65, 0, 61, 11, 55,
                    23, 11, 64, 51, 46, 51, 51, 33, 29, 41, 42, 61, 0, 62, 23,
                    72, 52, 31, 43, 65, 29, 46, 31, 51, 23, 59, 11, 62, 0, 59,
                    46, 21, 51, 64, 23, 59, 33, 37, 11, 37, 61, 55, 23, 59, 0,
            };
}