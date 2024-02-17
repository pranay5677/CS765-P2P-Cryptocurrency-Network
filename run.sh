#!/bin/bash

# Values specified at the top of the file
N_VAL=40
Z0_VAL=40
Z1_VAL=40
T_TXT_VAL=100
I_VAL=200

rm -f graph_program main_program
rm -f logfile.txt edge_trees.txt adjacency_list.txt vectors.txt Graph_peers.png
rm -f *.png
# Compile and run graph.cpp
g++ graph.cpp -o graph_program
./graph_program $N_VAL

# Run Python script
# python3 graph_visualize.py

# Compile and run main.cpp with specified arguments
g++ -g main.cpp -o main_program
./main_program --n=$N_VAL --z0=$Z0_VAL --z1=$Z1_VAL --T_tx=$T_TXT_VAL --I=$I_VAL
# python3 tree_visualize.py
# python3 statistics.py

