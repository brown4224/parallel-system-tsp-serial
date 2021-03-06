#include "graphs.h"
#include <iostream>
#include <assert.h>


int *build_graph(const int n_cities) {
    assert(n_cities > 0);
    int *graph;

    if (n_cities == 4) {
        graph = graph_4();
    } else if (n_cities == 6) {
        graph = graph_6();

    } else if (n_cities == 12) {
        graph = graph_12();
    } else if (n_cities == 15) {
        graph = graph_15();

    } else {
        graph = graph_random(n_cities);
    }

    return graph;

}


int *graph_random(int const n_cities) {

    const int size = n_cities * n_cities;
    int *graph = new int[size];


    srand(time(NULL));

    for (int j = 1; j < size; ++j) {
        int temp = rand() % 50 + 1;

        if (temp % 9 == 0) {
            temp = -1;
        }


        graph[j] = temp;


    }

    for (int j = 0; j < size; j += n_cities + 1) {
        graph[j] = 0;
    }

    return graph;


}

int *graph_4() {
    /**
     * Results From part 1:
     * Cities: 0, 3, 1, 2, 0,
     * Cost: 15
     * Size: 5
     */

    return new (int[16])
            {
                    0, 1, 3, 8,
                    5, 0, 2, 6,
                    1, 18, 0, 10,
                    7, 4, 12, 0
            };
}

int *graph_2() {
    //2nd test case : Symmetric tsp. Time taken : 6.44 x 10^-5 seconds
    return new int[16]
            {
                    0, 29, 82, 46,
                    29, 0, 55, 46,
                    82, 55, 0, 68,
                    46, 46, 68, 0,
            };
}

int *graph_6() {
    //3rd case : Assymetric tsp with 6 cities. Time taken : 0.00013401
    /**
     * Results From part 1:
     * Cities:  0, 2, 4, 1, 3, 5, 0,
     * Cost: 195
     * Size: 7
     */
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

int *graph_12() {
    /**
     * Results From part 1:
     * Cities: 0, 5, 7, 9, 6, 3, 8, 2, 11, 4, 1, 10, 0,
     * Cost: 103
     * Size: 13
     */

    return new int[144]
            {
                    0, 47, -1, 23, 19, 5, 41, 28, 16, 25, 39, 5,
                    24, 0, 50, 41, 13, -1, -1, 49, 30, 21, 2, -1,
                    -1, 13, 0, 20, 14, 37, 33, 28, -1, 11, 3, 4,
                    17, -1, 31, 0, 19, 19, 39, -1, 13, 40, 37, 25,
                    -1, 6, 26, 38, 0, -1, 15, -1, 39, 17, 14, 4,
                    4, 48, 33, 39, -1, 0, 44, 25, 30, 24, -1, 50,
                    44, 49, 46, 6, 38, 32, 0, 48, 37, -1, 38, 17,
                    35, 4, 11, 25, 20, -1, 28, 0, 26, 11, 15, 34,
                    47, 8, 11, 28, 33, 21, 30, 26, 0, 25, 33, -1,
                    7, 17, -1, -1, 25, 46, 11, 10, 49, 0, -1, -1,
                    2, 16, 43, -1, 28, -1, 13, 24, 17, 25, 0, 1,
                    47, 33, -1, -1, 7, 11, -1, 15, -1, 37, 12, 0,
            };
}

int *graph_15() {
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