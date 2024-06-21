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


class HeapNode {
    public:
        infoNode value;
        int pos; // Posición dentro de la cola
};

typedef vector<HeapNode*> Heap;
typedef vector<HeapNode*> NodePair;

class MinHeap {
    public:
        Heap priqueue;
        NodePair nodepair;
        int size;

    MinHeap(int heapSize){
        size = heapSize;
        priqueue.reserve(heapSize);
        nodepair.resize(heapSize);
    }
};

int parent(int i){
    return (i-1) / 2;
}

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
    if(l < A.size && A.priqueue[l]->value.dist < A.priqueue[i]->value.dist){
        smallest = l;
    } else {
        smallest = i;
    }
    if(r < A.size && A.priqueue[r]->value.dist < A.priqueue[smallest]->value.dist){
        smallest = r;
    }

    if(smallest != i){
        swap(A.priqueue[i]->pos, A.priqueue[smallest]->pos);
        swap(A.priqueue[i], A.priqueue[smallest]);
        minHeapify(A, smallest);
    }
}

MinHeap heapify(vector<HeapNode*>& A){ // O(n)
    MinHeap H(A.size());
    H.priqueue = A;

    for(int i = (H.size/2) - 1; i >= 0; i--){ // O(n)
        minHeapify(H, i);
    }

    int posicion = 0; // Para asegurarnos que cada HeapNode no tenga posicion nula
    /*
    for(HeapNode &n : H.priqueue){ // O(n)
        int index = n.value.node->index;
        H.nodepair[index] = &n;
        n.pos = posicion;
        posicion++;
    }
    */

    for(int i = 0; i < H.size; i++){
        int index = H.priqueue[i]->value.node->index;
        H.nodepair[index] = H.priqueue[i];
        H.priqueue[i]->pos = posicion;
        posicion++;
    }

    return H;
}

HeapNode* min(MinHeap& A){ // O(1)
    return A.priqueue[0];
}

HeapNode* extractMin(MinHeap& A){ // O(log n)
    if(A.size < 0){
        throw out_of_range("La cola no tiene elementos");
    }
    HeapNode* min = A.priqueue[0];
    A.priqueue[0] = A.priqueue[A.size-1];
    A.priqueue[0]->pos = 0;
    A.size -= 1;
    minHeapify(A, 0);
    return min;
}

void decreaseKey(MinHeap& A, int index, double key){
    if(key > A.nodepair[index]->value.dist){
        throw invalid_argument("Nueva key es mayor a la actual");
    }
    A.nodepair[index]->value.dist = key;
    int pos = A.nodepair[index]->pos;
    while(pos > 0 && A.priqueue[parent(pos)]->value.dist > A.priqueue[pos]->value.dist){
        swap(A.priqueue[pos]->pos, A.priqueue[parent(pos)]->pos);
        swap(A.priqueue[pos], A.priqueue[parent(pos)]);
        pos = parent(pos);
    }
}

int main() {
    default_random_engine gen;
    uniform_real_distribution<> urd(0.0 + numeric_limits<double>::epsilon(), 1.0);

    srand(45);
    cout << setprecision(3);

    vector<HeapNode*> A1;
    vector<HeapNode*> A2;

    Graph* g = createGraph(7, 2, 4);

    //printGraph(*g);

    cout << "\nTest minHeapify, mantener propiedad de minheap:\n";

    for(int i = 0; i < 2; i++){
        //double r = urd(gen);
        double d = 0.99;
        infoNode inf1 = {d, (*g)[i]};
        HeapNode* ptr = new HeapNode;
        HeapNode hn1 = {inf1, i}; 
        *ptr = hn1;
        A1.push_back(ptr);
    }

    infoNode inf = {0.1, (*g)[g->size()-1]};
    HeapNode* ptr1 = new HeapNode;
    HeapNode hn1 = {inf, 3};
    *ptr1 = hn1; 
    A1.push_back(ptr1);

    MinHeap H(A1.size());

    H.priqueue = A1;
    
    for(HeapNode* n : H.priqueue){
        cout << "(" << n->value.dist << ", " << n->value.node->index << ") ";
    }

    cout << "\n";
    
    minHeapify(H, 0);

    for(HeapNode* n : H.priqueue){
        cout << "(" << n->value.dist << ", " << n->value.node->index << ") ";
    }


    cout << "\n\nTest heapify: \n";

    for(int i = 0; i < g->size(); i++){
        double r = urd(gen);
        double d = 0.99;
        infoNode inf2 = {r, (*g)[i]};
        HeapNode* ptr2 = new HeapNode;
        HeapNode hn2 = {inf2, i};
        *ptr2 = hn2;
        A2.push_back(ptr2);
    }
    
    for(HeapNode* n : A2){
        cout << "(" << n->value.dist << ", " << n->value.node->index << ") posicion: " << n->pos << "\n";
    }
    
    cout << "\n";
    MinHeap H2 = heapify(A2);

    
    for(HeapNode* n : H2.priqueue){
        cout << "(" << n->value.dist << ", " << n->value.node->index << ") posicion: " << n->pos << "\n";
    }

    cout << "\n\nTest referencia de nodo a par que lo representa:\n";
    
    for(Node* n : *g){
        int node_index = n->index;
        cout << "Indice de nodo: " << node_index;
        HeapNode* par = H2.nodepair[node_index];
        cout << " Par que lo representa: " << "(" << par->value.dist << ", " << par->value.node->index << ") \n";
    }
    cout << "\n";
    for(HeapNode* n : H2.priqueue){
        int node_index = n->value.node->index;
        cout << "Indice de nodo: " << node_index;
        HeapNode* par = H2.nodepair[node_index];
        cout << " Par que lo representa: " << "(" << par->value.dist << ", " << par->value.node->index << ") \n";
    }
    

    cout << "\n\nTest min y extractMin:\n";

    cout << "Par con dist. minima: " << "(" << min(H2)->value.dist << ", " << min(H2)->value.node->index << ") \n";

    HeapNode* min_pair = extractMin(H2);

    cout << "Par con dist. minima extraido: " << "(" << min_pair->value.dist << ", " << min_pair->value.node->index << ") \n";

    cout << "Heap luego de extraer minimo:\n";
    for(int i = 0; i < H2.size; i++){
        cout << "(" << H2.priqueue[i]->value.dist << ", " << H2.priqueue[i]->value.node->index << ") posicion: " << H2.priqueue[i]->pos << "\n";
    }

    cout << "\nTest decreaseKey:\n";

    decreaseKey(H2, 4, 0.01);

    cout << "Heap luego de disminuir clave de nodo 4 a 0.01:\n";
    for(int i = 0; i < H2.size; i++){
        cout << "(" << H2.priqueue[i]->value.dist << ", " << H2.priqueue[i]->value.node->index << ") posicion: " << H2.priqueue[i]->pos << "\n";
    }

    cout << "\nTest referencia de nodo a par que lo representa:\n";

    for(int i = 0; i < H2.size; i++){
        int node_index = H2.priqueue[i]->value.node->index;
        cout << "Indice de nodo: " << node_index;
        HeapNode* par = H2.nodepair[node_index];
        cout << " Par que lo representa: " << "(" << par->value.dist << ", " << par->value.node->index << ") \n";
    }

}