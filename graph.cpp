#include <iostream>
#include <algorithm>
#include <vector>
#include <tuple>
#include <math.h>
#include <stdlib.h>
#include <limits>
#include <random>
#include <iomanip>

using namespace std;

class Node;
typedef tuple<double, Node> infoNode;
typedef vector<Node*> Graph;

class AdyNode {
    public:
        int index;
        double dist_node;
        infoNode *info_node;
        AdyNode *next_ady_node;
};

class Node {
    public:
        int index;
        AdyNode *next_ady_node;
};

Graph *createGraph(int n_vertices, int n_edges, int seed) {
    Graph *ptr_graph = new Graph;
    Graph graph(n_vertices);
    for (int i = 0; i < n_vertices; i++) {
        Node *ptr_node_i = new Node;
        Node node_i = {i, NULL};
        *ptr_node_i = node_i;
        graph[i] = ptr_node_i;
    }
    default_random_engine gen;
    uniform_real_distribution<> urd(0.0 + numeric_limits<double>::epsilon(), 1.0);
    srand(seed);
    // make a covertor tree with the nodes
    for (int i = 1; i < n_vertices; i++) {
        // add a new adjacent node to the node i
        int rand_index = rand() % i;
        double rand_weight = urd(gen);
        AdyNode *ptr_new_ady_node = new AdyNode;
        AdyNode new_ady_node = {rand_index, rand_weight, NULL, NULL};
        *ptr_new_ady_node = new_ady_node; 
        graph[i]->next_ady_node = ptr_new_ady_node;
        // add the initial vertice i to the adjacents of new_ady_node
        AdyNode *ptr_node_like_ady = new AdyNode;
        AdyNode node_like_ady = {i, rand_weight, NULL, NULL};
        *ptr_node_like_ady = node_like_ady;
        AdyNode *last_ady_node = graph[rand_index]->next_ady_node;
        if (last_ady_node != NULL) {
            while(last_ady_node->next_ady_node != NULL)
                last_ady_node = last_ady_node->next_ady_node;
            last_ady_node->next_ady_node = ptr_node_like_ady;
        } else {
            graph[rand_index]->next_ady_node = ptr_node_like_ady;
        }
    }
    // create the random n_edges - (n_vertices - 1)
    int m_edges = n_edges - (n_vertices - 1);
    for (int i = 0; i < m_edges; i++) {
        int is_repeated = 0;
        int node1;
        int node2;
        // iterate until find two nodes that have not been connected
        do {
            node1 = rand() % n_vertices;
            do node2 = rand() % n_vertices;    
            while (node2 == node1);
            AdyNode *ady_node = graph[node1]->next_ady_node;
            while (ady_node != NULL) {
                if (ady_node->index == node2) {
                    is_repeated = 1;
                    break;
                } else {
                    is_repeated = 0;
                }
                ady_node = ady_node->next_ady_node;
            }
        } while (is_repeated);
        // create the two nodes for the adjacent list of node1 and node2
        double rand_weight = urd(gen);
        AdyNode *ptr_ady_node1 = new AdyNode();
        AdyNode *ptr_ady_node2 = new AdyNode();
        AdyNode ady_node1 = {node1, rand_weight, NULL, NULL};
        AdyNode ady_node2 = {node2, rand_weight, NULL, NULL};
        *ptr_ady_node1 = ady_node1;
        *ptr_ady_node2 = ady_node2; 
        AdyNode *actual_ady_node;
        actual_ady_node = graph[node1]->next_ady_node;
        while (actual_ady_node->next_ady_node!= NULL)
            actual_ady_node = actual_ady_node->next_ady_node;
        // this is the last adjacent node of the node1
        actual_ady_node->next_ady_node = ptr_ady_node2;
        actual_ady_node = graph[node2]->next_ady_node;
        while (actual_ady_node->next_ady_node!= NULL)
            actual_ady_node = actual_ady_node->next_ady_node;
        // this is the last adjacent node of the node2
        actual_ady_node->next_ady_node = ptr_ady_node1;
    }
    *ptr_graph = graph; 
    return ptr_graph;
}

void printGraph(Graph graph) {
    cout << setprecision(2);
    for (int i = 0; i < graph.size(); i++) {
        cout << graph[i]->index << "--> ";
        AdyNode *actual_ady_node = graph[i]->next_ady_node;
        while (actual_ady_node != NULL) {
            cout << "(" << actual_ady_node->index << ", " << actual_ady_node->dist_node <<  ") --> ";
            actual_ady_node = actual_ady_node->next_ady_node;
        }
        cout << "NULL" << endl;
    }
}

int main() {
    Graph *test = createGraph(20, 45, 9);
    printGraph(*test);
}
