#!bin/bash
g++ -g -Wall   main.cpp stack.cpp graphs.cpp -o tsp_serial_code -std=c++0x
#g++ -g -Wall  main.o stack.o graphs.o -o tsp_serial_code -std=c++0x
./tsp_serial_code
