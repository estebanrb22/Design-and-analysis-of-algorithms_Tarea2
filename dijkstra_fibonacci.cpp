#include <iostream>
#include <algorithm>
#include <vector>
#include <tuple>
#include <math.h>
#include <stdlib.h>
#include <limits>
#include <random>
#include <iomanip>
#include <chrono>

#include "graph.cpp"

using namespace std;

#define INFINITE numeric_limits<double>::infinity(); 

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

class info_djikstra {
    public: 
        vector<double> *dist;
        vector<int> *prev;
};

FibHeap *makeFibHeap() {
    FibHeap *ptr_fib_heap = new FibHeap();
    FibHeap fib_heap = {NULL, 0, 0, NULL};
    *ptr_fib_heap = fib_heap;
    return ptr_fib_heap;
}

void deleteFibHeap(FibHeap *heap) {
    vector<FibNode*> vec_nodes = *heap->v_nodes; 
    for (int i = 0; i < vec_nodes.size(); i++) {
        delete vec_nodes[i]->value;
        delete vec_nodes[i];
    }
    delete heap;
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
    int degree = log(heap->n_nodes) / log(2) + 2;
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

FibHeap *heapifyFibHeap(Graph *graph, vector<double> *dist) {
    FibHeap *heap = makeFibHeap();
    vector<FibNode*> *ptr_nodes = new vector<FibNode*>();
    heap->v_nodes = ptr_nodes;
    for (int i = 0; i < graph->size(); i++) {
        infoNode *ptr_info = new infoNode();
        infoNode info = {(*dist)[i], (*graph)[i]};
        *ptr_info = info;
        insertFibHeap(heap, ptr_info);
    }
    return heap;
}

void decreaseKeyFibHeapByIndex(FibHeap *heap, int index_node, double new_pri) {
    FibNode *node = (*heap->v_nodes)[index_node];
    decreaseKeyFibHeap(heap, node, new_pri);
}

info_djikstra *createDistPrev(Graph *graph, int root_index) {
    /**create the array of distances where dist[root_index] is 0 and all the
       other elements are infinity. */
    int n_nodes = graph->size();
    vector<double> *dist = new vector<double>(n_nodes);
    vector<double> aux_d(n_nodes);
    vector<int> *prev = new vector<int>(n_nodes);
    for (int i = 0; i < root_index; i++)
        (aux_d)[i] = INFINITE;

    (aux_d)[root_index] = 0;

    for (int i = root_index + 1; i < n_nodes; i++)
        (aux_d)[i] = INFINITE;
    *dist = aux_d;
    info_djikstra *info = new info_djikstra();
    info->dist = dist;
    info->prev = prev;
    return info;
}

void deleteDistPrev(info_djikstra *info) {
    delete info->dist;
    delete info->prev;
    delete info;
}

void printInfoNode(infoNode *info) {
    cout << "(" << info->node->index << ", " << info->dist <<  ")";
}

void printFibHeap(FibHeap *heap) {
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
    for (int i = 0; i < heap->v_nodes->size(); i++)
        printFibNode((*heap->v_nodes)[i]);
    cout << endl;
}

void printInfoDjikstra(info_djikstra *info, int root_index) {
    cout << "Arbol de mínimas distancias nodo " << root_index <<" (node, distance, prev):" << endl << endl;
    int n_nodes = info->dist->size();
    vector<double> distances = *info->dist;
    vector<int> prevs = *info->prev;
    for (int i = 0; i < n_nodes; i++)
        cout << "(" << i << ", " << distances[i] << ", " << prevs[i] << ") ";
}

void test(int n_nodes, int n_edges) {
    Graph *graph = createGraph(n_nodes, n_edges, 0);
    printGraph(*graph);

    info_djikstra *info = createDistPrev(graph, n_nodes-1);

    FibHeap *heap = heapifyFibHeap(graph, info->dist);
    printFibHeap(heap);

    extractMin(heap);

    printFibHeap(heap);
    printNodesFibHeap(heap);

    decreaseKeyFibHeap(heap, (*heap->v_nodes)[4], 0.05);

    printFibHeap(heap);
    printNodesFibHeap(heap);
}

info_djikstra *dijkstra_fibonacci(Graph *graph, int root_index) {
    // create the arrays of distances and prevs, info_dijkstra has all this information.
    info_djikstra *min_tree = createDistPrev(graph, root_index);

    // create the fibonacci heap with the graph nodes and their distances.
    FibHeap *fibonacciHeap = heapifyFibHeap(graph, min_tree->dist);

    while (fibonacciHeap->n_nodes > 0) {
        infoNode *min_node = extractMin(fibonacciHeap);
        AdyNode *act_ady_node = min_node->node->next_ady_node;
        while (act_ady_node != NULL) {
            /* if the distance of the min node + distance of the ady node is lower 
            than the actual ady node distance to the root in the heap we need to decrease its key. */
            double new_root_dist = act_ady_node->dist_node + min_node->dist;
            double act_root_dist = (*fibonacciHeap->v_nodes)[act_ady_node->index]->value->dist;
            if (new_root_dist < act_root_dist) {
                decreaseKeyFibHeapByIndex(fibonacciHeap, act_ady_node->index, new_root_dist);
                (*min_tree->dist)[act_ady_node->index] = new_root_dist;
                (*min_tree->prev)[act_ady_node->index] = min_node->node->index;
            }
            act_ady_node = act_ady_node->next_ady_node;
        }
    }
    deleteFibHeap(fibonacciHeap);
    return min_tree;
}

double calculateMean(vector<double> time) {
    double sum = accumulate(time.begin(), time.end(), 0.0);
    double mean = sum / time.size();
    return mean;
}

string makeLegibleNumber(long long num) {
    string legible_number;
    string snum = to_string(num);
    int len = snum.length();
    int count = 0;
    for (int i = snum.size() - 1; i >= 0; i--) {
        if (count % 3 == 0 && count != 0)
            legible_number.insert(0, 1, '.');
        legible_number.insert(0, 1, snum[i]);
        count++;
    }
    return legible_number;
}

vector<double> *calculateNanoMeanDurationDjikstraFibonacci(int pow_nodes, int pow_edges, int seed_graphs) {
    /* This function executes dijkstra algorithm with a graph with 2^pow_nodes nodes and 2^pow_edges edges, 
       it makes 50 iterations and calculates the time of creation of the graph and the time of 
       Djikstra algorithm with a random root node, this with a Fibonacci Heap. Return the mean of dijkstra algorithm 
       and the mean of creation graph in a vector of lenth equal 2.*/
    int n_nodes = pow(2, pow_nodes);
    int n_edges = pow(2, pow_edges);
    cout << "Djikstra Fibonacci mean time for graph with " << n_nodes << " (2^" << pow_nodes << ") nodes and " << n_edges << " (2^" << pow_edges << ") edges:" << endl << endl;
    srand(seed_graphs);
    int iterations = 50;
    vector<double> time_dijkstra(iterations);
    vector<double> time_graph(iterations);
    int max_edges = (n_nodes*(n_nodes - 1)) / 2;
    double mean_d = 0; 
    double mean_g = 0;
    if (n_edges <= max_edges) {
        for (int i = 0; i < iterations; i++) {

            // calculate the time to create a graph.
            auto start_g = chrono::high_resolution_clock::now();
            Graph *graph_i = createGraph(n_nodes, n_edges, rand());
            auto end_g = chrono::high_resolution_clock::now();
            auto time_i_g = chrono::duration_cast<chrono::nanoseconds>(end_g - start_g);
            double time_i_g_d = static_cast<double>(time_i_g.count());

            // set a random root node to make its minimum tree.
            int root_index = rand() % n_nodes; 

            //calculate the time of Djikstra with Fibonacci Heap in graph_i with root_index its root node.
            auto start = chrono::high_resolution_clock::now();
            info_djikstra *info = dijkstra_fibonacci(graph_i, root_index);
            auto end = chrono::high_resolution_clock::now();
            auto time_i = chrono::duration_cast<chrono::nanoseconds>(end - start);
            double time_i_d = static_cast<double>(time_i.count());

            // free the memory from the graph and the results from the Dijkstra algorithm.
            deleteGraph(graph_i);
            deleteDistPrev(info);

            time_graph[i] = time_i_g_d;
            time_dijkstra[i] = time_i_d;
            cout << fixed << setprecision(0) << "time iteration " << i + 1 << ": Fib-Dijkstra= " << makeLegibleNumber(time_i_d) << "ns Create-Graph= " << makeLegibleNumber(time_i_g_d) << "ns" << endl;
        }
        mean_d = calculateMean(time_dijkstra);
        mean_g = calculateMean(time_graph);

        cout << endl << fixed << setprecision(0) << "MEAN TIME (2^" << pow_nodes << ") nodes - (2^" << pow_edges << ") edges: Fib-Dijkstra= "<< makeLegibleNumber(mean_d) << "ns Create-Graph= " << makeLegibleNumber(mean_g) << "ns" << endl << endl;
    } 
    else {
        // The n_edges value is not compatible, mean_d and mean_g are 0.
        cout << "MEAN TIME (2^" << pow_nodes << ") nodes - (2^" << pow_edges << ") edges = NOT_POSSIBLE" << endl << endl;
    }
    vector<double> *info = new vector<double>(2);
    (*info)[0] = mean_d;
    (*info)[1] = mean_g;
    return info;
}

vector<vector<vector<double>*>*> *calculateAllTimes(int seed_experiments) {
    srand(seed_experiments);
    int n_samples = 3;
    int e_samples = 7;
    int n_nodes[n_samples] = {10, 12, 14};
    int n_edges[e_samples] = {16, 17, 18, 19, 20, 21, 22};
    vector<vector<vector<double>*>*> *results = new vector<vector<vector<double>*>*>(n_samples);
    // create the vectors
    for (int i = 0; i < n_samples; i++) {
        vector<vector<double>*> *info_i_nodes = new vector<vector<double>*>(e_samples);
        (*results)[i] = info_i_nodes;
    }
    // for every combination of n_nodes and n_edges calculate the mean time of Dijstra's algorithm with Fibonacci Heap.
    for (int i = 0; i < n_samples; i++) {
        for (int j = 0; j < e_samples; j++) {        
            vector<double> *mean_ij = calculateNanoMeanDurationDjikstraFibonacci(n_nodes[i], n_edges[j], rand());
            (*(*results)[i])[j] = mean_ij;
        }
    }
    cout << "------------------ ALL MEAN TIMES ------------------" << endl << endl;
    for (int i = 0; i < results->size(); i++) {
        for (int j = 0; j < (*results)[i]->size(); j++)
            cout << fixed << setprecision(0) << "(2^" << n_nodes[i] << ") nodes - (2^" << n_edges[j] << ") edges: Fib-Dijkstra= "<< makeLegibleNumber((*(*(*results)[i])[j])[0]) << "ns Create-Graph= " << makeLegibleNumber((*(*(*results)[i])[j])[1]) << "ns" << endl;
        cout << endl;
    }
    cout << endl;
    return results;
}

void deleteResultsExperiment(vector<vector<vector<double>*>*> *data) {
    for (int i = 0; i < data->size(); i++) {
        for (int j = 0; j < (*data)[i]->size(); j++)
            delete (*(*data)[i])[j];
        delete (*data)[i];
    }
    delete data;
}

void executeExperiments(int seed_experiments) {
    srand(seed_experiments);
    vector<vector<vector<double>*>*> *results = calculateAllTimes(rand());
    deleteResultsExperiment(results);
}

int main() {
    int random_seed = 10;
    executeExperiments(random_seed);
    /* This executes all the combinations of i, j values. If you want to evaluate just one combitation 
       you can use calculateNanoMeanDurationDjikstraFibonacci(i, j, rand_seed), you have to liberate the memory after that. 
       Example: 
        vector<double> *means = calculateNanoMeanDurationDjikstraFibonacci(i, j, rand_seed);
       */
}