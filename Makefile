N_VAL=4
Z0_VAL=30
Z1_VAL=30
T_TXT_VAL=0.1
I_VAL=3

all: graph_program main_program

graph_program: graph.cpp
	g++ graph.cpp -o graph_program

main_program: main.cpp
	g++ main.cpp -o main_program

run: all
	./graph_program
	python3 graph_visualize.py
	./main_program --n=$(N_VAL) --z0=$(Z0_VAL) --z1=$(Z1_VAL) --T_tx=$(T_TXT_VAL) --I=$(I_VAL)

clean:
	rm -f graph_program main_program
	rm -f logfile.txt edge_trees.txt adjacency_list.txt Graph_peers.png
