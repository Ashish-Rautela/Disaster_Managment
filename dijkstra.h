// --- FILE: dijkstra.h ---
#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

// Min-heap node for priority queue
typedef struct MinHeapNode {
    int vertex;
    int distance;
} MinHeapNode;

// Min-heap structure
typedef struct MinHeap {
    int size;
    int capacity;
    int* pos;               // Position of vertices in heap
    MinHeapNode** array;
} MinHeap;

// Function declarations
MinHeap* createMinHeap(int capacity);
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b);
void minHeapify(MinHeap* heap, int idx);
int isEmpty(MinHeap* heap);
MinHeapNode* extractMin(MinHeap* heap);
void decreaseKey(MinHeap* heap, int vertex, int distance);
void dijkstra(Graph* g, int source, int dist[], int parent[]);
void printShortestPath(Graph* g, int src, int dest, int parent[]);
int getShortestDistance(Graph* g, int src, int dest);
void freeMinHeap(MinHeap* heap);

#endif // DIJKSTRA_H