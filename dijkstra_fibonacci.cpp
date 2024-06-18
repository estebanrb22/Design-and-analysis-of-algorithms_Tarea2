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

class FibNode {
    public:
        infoNode *value;
        FibNode *parent;
        FibNode *childs;
        FibNode *left;
        FibNode *right;
        int mark;
        int degree;
};

class FibHeap {
    public:
        FibNode *min;
        int n_nodes;
};

FibHeap *makeFibHeap() {
    FibHeap *ptr_fib_heap = new FibHeap();
    FibHeap fib_heap = {NULL, 0};
    *ptr_fib_heap = fib_heap;
    return ptr_fib_heap;
}

void insertFibHeap(FibHeap *heap, infoNode *new_element) {
    FibNode *new_node = new FibNode();
    FibNode init_node = {new_element, NULL, NULL, NULL, NULL, 0, 0};
    *new_node = init_node;
    insertFibHeapNode(heap, new_node);
    heap->n_nodes++;
}

void insertFibHeapNode( FibHeap *heap, FibNode *new_node) {
    if (heap->min == NULL) {
        new_node->left = new_node;
        new_node->right = new_node;
        heap->min = new_node;   
    } else {
        FibNode *left_min = heap->min->left;
        left_min->right = new_node;
        heap->min->left = new_node;
        new_node->left = left_min;
        new_node->right = heap->min;
    }
}

infoNode *findMin(FibHeap *heap) {
    return heap->min->value;
} 

void consolidateFiboHeap(FibHeap *heap) {
    ;
}

infoNode *extractMin(FibHeap *heap) {
    FibNode *min = heap->min;
    if (min != NULL) {
        FibNode *act_child = min->childs;
        for (int i = 0; i < min->degree; i++) {
            FibNode *next_child = act_child->right;
            insertFibHeapNode(heap, act_child);
            act_child->parent = NULL;
            act_child = next_child;
        }
        min->left->right = min->right;
        min->right->left = min->left;
        if (min == min->right)
            heap->min == NULL;
        else {
            heap->min = min->right;
            consolidateFiboHeap(heap);
        }
        heap->n_nodes--;
    }
    return min->value;
}



void printFibHeap(FibHeap *heap) {
    cout << setprecision(2);
    if (heap->min == NULL) {
        cout << "Cola vacía" << endl;
    } else {
        FibNode *min = heap->min;
        FibNode *act = min;
        cout << "min dist: " << heap->min->value->dist << endl;
        cout << "min node: " << heap->min->value->node->index << endl << endl;
        do {
            cout << "(" << min->value->dist << ", " << min->value->node->index << ") <--> ";
            act = act->right;
        } while (act != min);
        cout << endl;
    }
}

int main() {
    FibHeap *test = makeFibHeap();
    printFibHeap(test);
}

