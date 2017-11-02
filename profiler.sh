#!bin/bash
#  sudo yum install -y install valgrind kcachegrind graphviz
#  https://baptiste-wicht.com/posts/2011/09/profile-c-application-with-callgrind-kcachegrind.html

g++ -g -Wall   main.cpp stack.cpp graphs.cpp -o tsp_serial_code -std=c++0x 
 valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes   ./tsp_serial_code
 kcachegrind
