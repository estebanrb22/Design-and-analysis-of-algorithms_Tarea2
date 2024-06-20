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
        int n_root;
        vector<FibNode*> *v_nodes;
};

FibHeap *makeFibHeap() {
    FibHeap *ptr_fib_heap = new FibHeap();
    vector<FibNode*> *ptr_vector = new vector<FibNode*>;
    FibHeap fib_heap = {NULL, 0, 0, ptr_vector};
    *ptr_fib_heap = fib_heap;
    return ptr_fib_heap;
}

void insertRootFibHeapNode( FibHeap *heap, FibNode *new_node) {
    // just change the pointers 
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
        if (new_node->value->dist < heap->min->value->dist)
            heap->min = new_node;
    }
    heap->n_root++;
}

void insertFibHeap(FibHeap *heap, infoNode *new_element) {
    FibNode *new_node = new FibNode();
    FibNode init_node = {new_element, NULL, NULL, NULL, NULL, 0, 0};
    *new_node = init_node;
    // the insersion of a node has to be in order, this is for v_nodes[i] <= FibNode->value->node->index = i
    heap->v_nodes->push_back(new_node);
    insertRootFibHeapNode(heap, new_node);
    heap->n_nodes++;
}

void removeRootFibHeapNode(FibHeap *heap, FibNode *to_delete) {
    // don't change the pointers of to_delete node and maintains the n_nodes
    to_delete->left->right = to_delete->right;
    to_delete->right->left = to_delete->left;
    heap->n_root--;
}

void removeChildFromParent(FibNode *child) {
    //don't change the pointers of the child
    FibNode *parent = child->parent;
    if (parent->degree == 1)
        parent->childs = NULL;
    else {
        child->left->right = child->right;
        child->right->left = child->left;
        if (parent->childs == child)
            parent->childs = child->right;
    }
    parent->degree--;
}

infoNode *findMin(FibHeap *heap) {
    return heap->min->value;
} 

void fibHeapLink(FibHeap *heap, FibNode *child, FibNode *parent) {
    removeRootFibHeapNode(heap, child);
    if (parent->childs == NULL) {
        parent->childs = child;
        child->parent = parent;
        child->right = child;
        child->left = child;
    } else {
        child->parent = parent;
        parent->childs->left->right = child;
        child->left = parent->childs->left;
        parent->childs->left = child;
        child->right = parent->childs;
    }
    parent->degree++;
    child->mark = 0;
}

void consolidateFiboHeap(FibHeap *heap) {
    int degree = log(heap->n_nodes) / log(2) + 1;
    FibNode *aux[degree];
    for (int i = 0; i < degree; i++)
        aux[i] = NULL;
    int n = heap->n_root;
    FibNode *p_node = heap->min;
    for (int i = 0; i < n; i++) {
        FibNode *next_node = p_node->right;
        int act_d = p_node->degree;
        while (aux[act_d] != NULL) {
            FibNode *c_node = aux[act_d];
            if (p_node->value->dist > c_node->value->dist) {
                FibNode *swap = p_node;
                p_node = c_node;
                c_node = swap;
            }
            fibHeapLink(heap, c_node, p_node);
            aux[act_d] = NULL;
            act_d++;
        }
        aux[act_d] = p_node;
        p_node = next_node;
    }
    heap->min = NULL;
    heap->n_root = 0;
    for (int i = 0; i < degree; i++) {
        if (aux[i] != NULL)
            insertRootFibHeapNode(heap, aux[i]);
    }
}

infoNode *extractMin(FibHeap *heap) {
    FibNode *min = heap->min;
    if (min != NULL) {
        FibNode *act_child = min->childs;
        for (int i = 0; i < min->degree; i++) {
            FibNode *next_child = act_child->right;
            insertRootFibHeapNode(heap, act_child);
            act_child->parent = NULL;
            act_child = next_child;
        }
        removeRootFibHeapNode(heap, min);
        heap->n_nodes--;
        if (min == min->right)
            heap->min == NULL;
        else {
            heap->min = min->right;
            consolidateFiboHeap(heap);
        }
    }
    return min->value;
}

void cutNodeFibHeap(FibHeap *heap, FibNode *node) {
    removeChildFromParent(node);
    insertRootFibHeapNode(heap, node);
    node->parent = NULL;
    node->mark = 0;
}

void cascadingCutFibHeap(FibHeap *heap, FibNode *parent) {
    FibNode *grnd_parent = parent->parent;
    if (grnd_parent != NULL) {
        if (!parent->mark) 
            parent->mark = 1;
        else {
            cutNodeFibHeap(heap, parent);
            cascadingCutFibHeap(heap, grnd_parent);
        }
    }
}

void decreaseKeyFibHeap(FibHeap *heap, FibNode *node, double new_pri) {
    node->value->dist = new_pri;
    FibNode *parent = node->parent;
    if (parent != NULL && node->value->dist < parent->value->dist) {
        cutNodeFibHeap(heap, node);
        cascadingCutFibHeap(heap, parent);
    }
    if (node->value->dist < heap->min->value->dist) 
        heap->min = node;
}

FibHeap *heapifyFibHeap(Graph *graph) {
    FibHeap *heap = makeFibHeap();
    for (int i = 0; i < graph->size(); i++) {
        infoNode *ptr_info = new infoNode();
        infoNode info = {0.1 * i, (*graph)[i]};
        *ptr_info = info;
        insertFibHeap(heap, ptr_info);
    }
    return heap;
}

void printInfoNode(infoNode *info) {
    cout << setprecision(2);
    cout << "(" << info->node->index << ", " << info->dist <<  ")";
}

void printFibHeap(FibHeap *heap) {
    cout << setprecision(2);
    if (heap->min == NULL) {
        cout << "Cola vacía" << endl;
    } else {
        cout << "Root list:" << endl;
        FibNode *last = heap->min->left;
        for (FibNode *i = heap->min; i != last; i = i->right) {
            printInfoNode(i->value);
            cout << " <---> ";
        }
        printInfoNode(last->value);
        cout << "    ||    Min = ";
        printInfoNode(heap->min->value);
        cout << endl << endl;
    }
}

void printFibNode(FibNode *fibNode) {
    cout << "node " << fibNode->value->node->index << " -> ";
    cout << "pri: " << fibNode->value->dist << " - ";
    if (fibNode->parent != NULL)
        cout << "p_node: " << fibNode->parent->value->node->index << " - ";
    if (fibNode->childs != NULL) 
        cout << "c_node: " << fibNode->childs->value->node->index << " - ";
    cout << "l_node: " << fibNode->left->value->node->index << " - ";
    cout << "r_node: " << fibNode->right->value->node->index << " - ";
    cout << "d: " << fibNode->degree << " - ";
    cout << "mark: " << fibNode->mark << endl;
}

void printNodesFibHeap(FibHeap *heap) {
    vector<FibNode*> *nodes = heap->v_nodes;
    for (int i = 0; i < heap->v_nodes->size(); i++)
        printFibNode((*nodes)[i]);
    cout << endl;
}

/**
int main() {
    Graph *graph = createGraph(8, 9, 0);

    printGraph(*graph);

    FibHeap *heap = heapifyFibHeap(graph);
    printFibHeap(heap);

    extractMin(heap);

    printFibHeap(heap);

    printNodesFibHeap(heap);

    decreaseKeyFibHeap(heap, (*heap->v_nodes)[4], 0.05);

    printFibHeap(heap);

    printNodesFibHeap(heap);
}
*/

