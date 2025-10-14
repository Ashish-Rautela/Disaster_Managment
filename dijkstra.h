// --- FILE: dijkstra.h ---
#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

// Min-heap node
typedef struct MinHeapNode {
    int vertex;
    int distance;
} MinHeapNode;

// Min-heap
typedef struct MinHeap {
    int size;
    int capacity;
    int* pos;               // Vertex positions in heap
    MinHeapNode** array;
} MinHeap;

// Min-heap functions
MinHeap* createMinHeap(int capacity);
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b);
void minHeapify(MinHeap* heap, int idx);
int isEmpty(MinHeap* heap);
MinHeapNode* extractMin(MinHeap* heap);
void decreaseKey(MinHeap* heap, int vertex, int distance);
void freeMinHeap(MinHeap* heap);

// Dijkstra functions
void dijkstra(Graph* g, int source, int dist[], int parent[]);
void printShortestPath(Graph* g, int src, int dest, int parent[]);
int getShortestDistance(Graph* g, int src, int dest);

#endif // DIJKSTRA_H
