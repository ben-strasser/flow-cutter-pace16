#!/bin/sh
g++ -Wall -std=c++11 -O3 -DNDEBUG src/*.cpp -o flow_cutter_pace16
g++ -Wall -std=c++11 -O3 -DNDEBUG -DPARALLELIZE -fopenmp src/*.cpp -o flow_cutter_parallel_pace16
 
