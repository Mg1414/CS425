// ---------------------------------------------------------------------------
// routing_sim.cpp — Distance Vector and Link State (ELI5 version)
// ---------------------------------------------------------------------------
// This program reads a small graph from input*.txt, then prints routing tables
// as if each node was running:
//   • Distance Vector (Bellman‑Ford style updates / next hops)
//   • Link State    (Dijkstra from each node / next hops)
// The comments explain what each stage does and why it’s valid.
// ---------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <limits>
#include <queue>
#include <fstream>
#include <sstream>

using namespace std;

const int INF = 9999;  


void printRoutingTable(int node, const vector<vector<int>> &costTable, const vector<vector<int>> &nextHopTable) {
    cout << "Routing Table for Node " << node << ":\n";
    cout << "Destination\tCost\tNext Hop\n";
    for (int dest = 0; dest < costTable.size(); ++dest) {
        cout << dest << "\t\t" << costTable[node][dest] << "\t";
        // Print the next hop or "-" if no valid next hop
        if (dest == node || nextHopTable[node][dest] == -1)
            cout << "-";
        else
            cout << nextHopTable[node][dest];
        cout << endl;
    }
    cout << endl;
}


void simulateDistanceVectorRouting(const vector<vector<int>> &graph) {
    int numNodes = graph.size();
    vector<vector<int>> distanceTable = graph;  
    vector<vector<int>> nextHopTable(numNodes, vector<int>(numNodes, -1));

    // Initialize distance and next hop
    for (int node = 0; node < numNodes; ++node) {
        for (int dest = 0; dest < numNodes; ++dest) {
            if (node == dest) {
                distanceTable[node][dest] = 0;  // Distance to itself is 0
                nextHopTable[node][dest] = dest;  // Next hop to itself is itself
            } else if (graph[node][dest] != INF) {
                nextHopTable[node][dest] = dest;  // Direct neighbors point to themselves as the next hop
            }
        }
    }

    bool updated;
    do {
        updated = false;
        
        for (int node = 0; node < numNodes; ++node) {
            for (int dest = 0; dest < numNodes; ++dest) {
                for (int intermediate = 0; intermediate < numNodes; ++intermediate) {
                    // Check if there's a better route
                    if (distanceTable[node][intermediate] != INF && distanceTable[intermediate][dest] != INF && 
                        distanceTable[node][dest] > distanceTable[node][intermediate] + distanceTable[intermediate][dest]) {
                        distanceTable[node][dest] = distanceTable[node][intermediate] + distanceTable[intermediate][dest];
                        nextHopTable[node][dest] = nextHopTable[node][intermediate];
                        updated = true;  // Mark that an update has occurred
                    }
                }
            }
        }
    } while (updated);  // stop if no more updates possible

    cout << "--- Final Distance Vector Routing Tables ---\n";
    for (int node = 0; node < numNodes; ++node) {
        printRoutingTable(node, distanceTable, nextHopTable);  
    }
}


void printLinkStateRoutingTable(int src, const vector<int> &distances, const vector<int> &previousNodes) {
    cout << "Routing Table for Node " << src << ":\n";
    cout << "Destination\tCost\tNext Hop\n";
    for (int dest = 0; dest < distances.size(); ++dest) {
        if (dest == src)
            continue;  
        cout << dest << "\t\t" << distances[dest] << "\t";
        
        
        int hop = dest;
        while (previousNodes[hop] != src && previousNodes[hop] != -1)
            hop = previousNodes[hop];
        
        
        cout << (previousNodes[hop] == -1 ? -1 : hop) << endl;
    }
    cout << endl;
}


void simulateLinkStateRouting(const vector<vector<int>> &graph) {
    int numNodes = graph.size();
    for (int src = 0; src < numNodes; ++src) {
        vector<int> distances(numNodes, INF);  // Initialize distance array with infinity
        vector<int> previousNodes(numNodes, -1);  // Previous node array, initialized with -1
        vector<bool> visited(numNodes, false);  // Visited array to track visited nodes
        distances[src] = 0;  // Distance to itself is 0

        // priority queue
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push(make_pair(0, src));  // Push the src node with distance 0

        while (!pq.empty()) {
            pair<int, int> currentNode = pq.top();  // Get the node with the smallest distance
            pq.pop();
            int currentDist = currentNode.first, node = currentNode.second;
            if (visited[node])
                continue;  // Skip if the node has already been visited
            visited[node] = true;

            
            for (int neighbor = 0; neighbor < numNodes; ++neighbor) {
                if (graph[node][neighbor] != INF && graph[node][neighbor] > 0) {
                    int alternativeDist = distances[node] + graph[node][neighbor];
                    if (alternativeDist < distances[neighbor]) {
                        distances[neighbor] = alternativeDist;  // Update the distance to the neighbor
                        previousNodes[neighbor] = node;  // Set the previous node as the current node
                        pq.push(make_pair(alternativeDist, neighbor));  // Push the updated neighbor into the priority queue
                    }
                }
            }
        }
        
        printLinkStateRoutingTable(src, distances, previousNodes);
    }
}


vector<vector<int>> readGraphFromFile(const string &filename) {
    ifstream file(filename); 
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);  
    }

    int numNodes;
    file >> numNodes;  
    vector<vector<int>> graph(numNodes, vector<int>(numNodes)); 
    for (int i = 0; i < numNodes; ++i)
        for (int j = 0; j < numNodes; ++j)
            file >> graph[i][j]; 

    return graph;
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    string filename = argv[1];  
    vector<vector<int>> graph = readGraphFromFile(filename);  

    
    cout << "\n--- Distance Vector Routing Simulation ---\n";
    simulateDistanceVectorRouting(graph);

    
    cout << "\n--- Link State Routing Simulation ---\n";
    simulateLinkStateRouting(graph);

    return 0;
}
