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

int main(){
    cout << "Test Dijkstra:\n";

    int i = 14;
    int j = 22;
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