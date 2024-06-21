#include <iostream>
#include <algorithm>
#include <vector>
#include <tuple>
#include <math.h>
#include <stdlib.h>
#include <limits>
#include <random>
#include <iomanip>

#include "graph.cpp"

using namespace std;

// Implementación de MinHeap basada en el libro
// “Introduction to Algorithms Third Edition”
// de Cormen et. al. 

typedef vector<infoNode> Heap;
typedef vector<infoNode*> NodePair;

class MinHeap {
    public:
        Heap priqueue;
        NodePair nodepair;
        int size;

    MinHeap(int heapSize){
        size = heapSize;
        priqueue.reserve(heapSize);
        nodepair.reserve(heapSize);
    }
};

// Retornar hijo izquierdo del nodo i
int left(int i){
    return 2*i + 1;
}

// Retornar hijo derecho del nodo i
int right(int i){
    return 2*i + 2;
}

// Mantiene la propiedad del heap para el Heap A en su nodo i
void minHeapify(MinHeap& A, int i){ // O(log n)
    int l = left(i);
    int r = right(i);
    int smallest;
    if(l < A.size && A.priqueue[l].dist < A.priqueue[i].dist){
        smallest = l;
    } else {
        smallest = i;
    }
    if(r < A.size && A.priqueue[r].dist < A.priqueue[smallest].dist){
        smallest = r;
    }

    if(smallest != i){
        swap(A.priqueue[i], A.priqueue[smallest]);
        minHeapify(A, smallest);
    }
}

MinHeap heapify(vector<infoNode>& A){ // O(n)
    MinHeap H(A.size());
    H.priqueue = A;

    for(int i = (H.size/2) - 1; i >= 0; i--){ // O(n)
        minHeapify(H, i);
    }

    for(infoNode &n : H.priqueue){ // O(n)
        int index = n.node->index;
        H.nodepair[index] = &n;
    }

    return H;
}

int main() {
    default_random_engine gen;
    uniform_real_distribution<> urd(0.0 + numeric_limits<double>::epsilon(), 1.0);

    srand(45);
    cout << setprecision(3);
    // Test minHeapify, mantener propiedad de minheap

    vector<infoNode> A1;
    vector<infoNode> A2;

    Graph* g = createGraph(7, 2, 4);

    //printGraph(*g);

    cout << "\nTest minHeapify, mantener propiedad de minheap\n\n";

    for(int i = 0; i < 2; i++){
        //double r = urd(gen);
        double d = 0.99;
        infoNode inf1 = {d, (*g)[i]};
        A1.push_back(inf1);
    }

    infoNode inf = {0.1, (*g)[g->size()-1]};
    A1.push_back(inf);

    MinHeap H(A1.size());

    H.priqueue = A1;

    for(infoNode n : H.priqueue){
        cout << "(" << n.dist << ", " << n.node->index << ") ";
    }
    
    cout << "\n";
    minHeapify(H, 0);

    for(infoNode n : H.priqueue){
        cout << "(" << n.dist << ", " << n.node->index << ") ";
    }


    cout << "\n\nTest heapify \n\n";

    for(int i = 0; i < g->size(); i++){
        double r = urd(gen);
        double d = 0.99;
        infoNode inf2 = {r, (*g)[i]};
        A2.push_back(inf2);
    }

    for(infoNode n : A2){
        cout << "(" << n.dist << ", " << n.node->index << ") ";
    }
    
    cout << "\n";
    MinHeap H2 = heapify(A2);


    for(infoNode n : H2.priqueue){
        cout << "(" << n.dist << ", " << n.node->index << ") ";
    }

    cout << "\n\nTest referencia de nodo a par que lo representa\n";

    for(Node* n : *g){
        int node_index = n->index;
        cout << "Indice de nodo: " << node_index;
        infoNode* par = H2.nodepair[node_index];
        cout << " Par que lo representa: " << "(" << par->dist << ", " << par->node->index << ") \n";
    }
    cout << "\n";
    for(infoNode n : H2.priqueue){
        int node_index = n.node->index;
        cout << "Indice de nodo: " << node_index;
        infoNode* par = H2.nodepair[node_index];
        cout << " Par que lo representa: " << "(" << par->dist << ", " << par->node->index << ") \n";
    }
}