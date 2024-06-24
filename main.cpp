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

#include "dijkstra_heap.cpp"
#include "dijkstra_fibonacci.cpp"

using namespace std;
using namespace std::chrono;

void main_v1() {
    cout << "Test Dijkstra:\n";

    int i = 10;
    int j = 16 ;
    double iterations = 50;

    double time_graph = 0;
    double time_heap = 0;
    double time_fib = 0;
    auto start = high_resolution_clock::now();
    for(int w = 0; w < iterations; w++){
        auto seed = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        auto start1 = high_resolution_clock::now();
        Graph* grafo = createGraph(1 << i, 1 << j, seed);
        auto stop1 = high_resolution_clock::now();
        chrono::duration<double, milli> duration1 = stop1 - start1;
        //cout << "Tiempo creacion grafo: " << duration1.count() / 1000 << endl;
        time_graph += duration1.count()/ 1000;
        //cout << "Grafo:\n";

        //printGraph(*grafo);

        int index_raiz = 0;
        auto start2 = high_resolution_clock::now();
        Dist_Prev camino_heap = dijkstraHeap(index_raiz, grafo);
        auto stop2 = high_resolution_clock::now();

        chrono::duration<double, milli> duration2 = stop2 - start2;
        //cout << "Tiempo dijkstra Heap: " << duration2.count() / 1000 << endl;
        time_heap += duration2.count()/ 1000;


        auto start3 = high_resolution_clock::now();
        info_dijkstra* camino_fib = dijkstra_fibonacci(grafo, index_raiz);
        auto stop3 = high_resolution_clock::now();

        chrono::duration<double, milli> duration3 = stop3 - start3;
        //cout << "Tiempo dijkstra Fibonacci: " << duration3.count() / 1000 << endl;
        time_fib += duration3.count()/ 1000;

        //cout << "\nVerificando que los resultados sean iguales:\n";

        //bool same_dist = equal(camino_heap.first.begin(), camino_heap.first.end(), (*camino_fib->dist).begin());
        //cout << "Arreglo Distancias iguales: "<<same_dist << endl; 

        //bool same_prev = equal(camino_heap.second.begin(), camino_heap.second.end(), (*camino_fib->prev).begin());
        //cout << "Arreglo Previos iguales: "<< same_prev << endl;

        //if(!same_dist || !same_prev){
        //    throw logic_error("Resultados de los algoritmos no son iguales");
        //    return 1;
        //}
    }
    auto stop = high_resolution_clock::now();
    chrono::duration<double, milli> duration = stop - start;

    double time_graph_prom = time_graph/iterations;
    double time_heap_prom = time_heap/iterations;
    double time_fib_prom = time_fib/iterations;
    cout << "Tiempo de ejecucion total con i = " << i << ", j = " << j << ": " << duration.count() / 1000 << " segundos.\n";
    cout << "Tiempo promedio de creacion de grafo: " << time_graph_prom << " segundos.\n";
    cout << "Tiempo promedio de dijkstra con heap: " << time_heap_prom << " segundos.\n";
    cout << "Tiempo promedio de dijkstra con fibonacci: " << time_fib_prom << " segundos.\n";
    /*
    for(int i = 0; i < (*grafo).size(); i++){
        cout << "Previo del nodo " << i << " en heap : " << camino_heap.second[i] << " y en fib : " << (*camino_fib->prev)[i] << endl;
    }
    */
}

void calculateTimeDijkstraHeapAndFib() {
    int i;
    int j;
    cout << "Ingrese el valor de i (2^i) para la cantidad de nodos: ";
    cin >> i;
    cout << "Ingrese el valor de j (2^j) para la cantidad de aristas: ";
    cin >> j;
    int n_nodes = 1 << i;
    int n_edges = 1 << j;
    cout << "Algoritmo de Dijkstra para " << n_nodes << " (2^" << i << ") nodos y " << n_edges << " (2^" << j << ") aristas:" << endl << endl;
    int iterations = 50;
    double time_graph = 0;
    double time_fibonacci = 0;
    double time_heap = 0;
    int max_edges = (n_nodes*(n_nodes - 1)) / 2;
    if (n_edges <= max_edges) {
        auto start_total = high_resolution_clock::now();
        for (int i = 0; i < iterations; i++) {
            // create the seed           
            auto random_seed = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

            // calculate the time to create a graph.
            auto start_g = chrono::high_resolution_clock::now();
            Graph *graph_i = createGraph(n_nodes, n_edges, random_seed);
            auto end_g = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> time_g = end_g - start_g;

            // set a random root node to make its minimum tree.
            int root_index = rand() % n_nodes; 

            //calculate the time of Djikstra with Fibonacci Heap in graph_i with root_index its root node.
            auto start_f = chrono::high_resolution_clock::now();
            info_dijkstra *info_f = dijkstra_fibonacci(graph_i, root_index);
            auto end_f = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> time_f = end_f - start_f;

            //calculate the time of Djikstra with Heap in graph_i with root_index its root node.
            auto start_h = chrono::high_resolution_clock::now();
            Dist_Prev info_h = dijkstraHeap(root_index, graph_i);
            auto end_h = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> time_h = end_h - start_h;

            // free the memory from the graph and the results from the Dijkstra algorithm.
            deleteGraph(graph_i);
            deleteDistPrev(info_f);

            time_graph += time_g.count();
            time_fibonacci += time_f.count();
            time_heap += time_h.count();

            cout << fixed << setprecision(2) << "Iteration " << i + 1 << ": Create-Graph= "  << time_g.count() <<"ms Dijkstra_Heap= " << time_h.count() << "ms Dijkstra_Fib= " << time_f.count() << "ms" <<  endl;
        }
        auto stop_total = high_resolution_clock::now();
        chrono::duration<double, milli> duration_total = stop_total - start_total;
        time_graph = time_graph / iterations;
        time_heap = time_heap / iterations;
        time_fibonacci = time_fibonacci / iterations;
        cout << fixed << setprecision(5);
        cout << "\nTiempo de ejecucion total con (2^" << i << ") nodos y (2^" << j << ") aristas = " << duration_total.count() / 1000 << " segundos.\n";
        cout << "Tiempo promedio de creacion del grafo: " << time_graph / 1000 << " segundos.\n";
        cout << "Tiempo promedio de dijkstra con heap: " << time_heap / 1000 << " segundos.\n";
        cout << "Tiempo promedio de dijkstra con fibonacci: " << time_fibonacci / 1000 << " segundos.\n";
    } 
    else {
        // The n_edges value is not compatible, mean_d and mean_g are 0.
        cout << "La cantidad de aristas (2^" << j << ") supera la cantidad de aristas posibles para un grafo no dirigido con (2^" << i << ") nodos" << endl << endl;
    }
}

int main(){
    calculateTimeDijkstraHeapAndFib();
}