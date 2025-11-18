Distance Vector and Link State Routing Simulator
================================================

This C++ program simulates two routing algorithms:
1. Distance Vector Routing (DVR)
2. Link State Routing (LSR)

It reads a graph from a file (representing a network of nodes and link costs) and displays the routing tables for each node using both DVR and LSR techniques.

///////
How to Compile and Run:

Enter the command: make
Or directly compile using: g++ -std=c++11 -o routing_sim routing_sim.cpp
to create the execuatable file 'routing_sim'.

To run the program enter the command:

./routing_sim input.txt

where, input.txt is the input in grid format.

///////////
Output:


The program prints two sections:
1. Distance Vector Routing Simulation:
   - For each node, it shows the routing table: destination, cost, and next hop.

2. Link State Routing Simulation:
   - For each node, it computes shortest paths using Dijkstra’s algorithm and displays the routing table similarly.

////////////
Code Explanation:

Functions:
readGraphFromFile: Reads the graph structure from the input file.
simulateDVR: Runs the Bellman-Ford-based distance vector routing algorithm.
simulateLSR: Runs Dijkstra’s algorithm from each node for link state routing.
printRoutingTableDVR / printRoutingTableLSR**: Print the formatted routing tables.
main: Takes in input, calls the simulation functions, and prints the output.

Note: 'INF' represents an unreachable path (with value = 9999).

