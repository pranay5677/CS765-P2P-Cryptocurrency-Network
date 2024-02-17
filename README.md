# Running Simulation Script

This script automates process of a discrete event simulator. Certain statistics related and grahical representative programs are commented out.<br>
Uncomment them as per your use case scenario. These may require following libraries:
```
1) networkx
2) matplotlib
3) pygraphviz
4) numpy
```
If you exprience any crashes/freezes comment out the content written to 'logfile.txt' in 'events.h' to ease the runtime.

## Script Contents

```bash
#!/bin/bash

# Set simulation parameters
N_VAL=40
Z0_VAL=40
Z1_VAL=40
T_TXT_VAL=100
I_VAL=1000

# Clean previous outputs
rm -f graph_program main_program
rm -f logfile.txt edge_trees.txt adjacency_list.txt vectors.txt Graph_peers.png
rm -f *.png

# Compile and run graph program
g++ graph.cpp -o graph_program
./graph_program $N_VAL

# Uncomment below to visualize the peer network (optional)
# python3 graph_visualize.py

# Compile and run main program with specified arguments
g++ -g main.cpp -o main_program
./main_program --n=$N_VAL --z0=$Z0_VAL --z1=$Z1_VAL --T_tx=$T_TXT_VAL --I=$I_VAL

# Uncomment below to get trees visualization (optional)
# python3 tree_visualize.py

# Uncomment below to get statistics (optional)
# python3 statistics.py
```

## Commands to run the discrete event simulator
```
chmod +x run.sh
bash run.sh
```

