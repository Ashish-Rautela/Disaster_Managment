// --- FILE: dijkstra.c ---
#include "dijkstra.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Create a min-heap with given capacity
MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->pos = (int*)malloc(capacity * sizeof(int));
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = (MinHeapNode**)malloc(capacity * sizeof(MinHeapNode*));
    return heap;
}

// Swap two min-heap nodes
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* temp = *a;
    *a = *b;
    *b = temp;
}

// Heapify at given index
void minHeapify(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    
    if (left < heap->size && 
        heap->array[left]->distance < heap->array[smallest]->distance)
        smallest = left;
    
    if (right < heap->size && 
        heap->array[right]->distance < heap->array[smallest]->distance)
        smallest = right;
    
    if (smallest != idx) {
        MinHeapNode* smallestNode = heap->array[smallest];
        MinHeapNode* idxNode = heap->array[idx];
        
        // Swap positions
        heap->pos[smallestNode->vertex] = idx;
        heap->pos[idxNode->vertex] = smallest;
        
        // Swap nodes
        swapMinHeapNode(&heap->array[smallest], &heap->array[idx]);
        minHeapify(heap, smallest);
    }
}

// Check if heap is empty
int isEmpty(MinHeap* heap) {
    return heap->size == 0;
}

// Extract minimum node from heap
MinHeapNode* extractMin(MinHeap* heap) {
    if (isEmpty(heap))
        return NULL;
    
    MinHeapNode* root = heap->array[0];
    MinHeapNode* lastNode = heap->array[heap->size - 1];
    heap->array[0] = lastNode;
    
    heap->pos[root->vertex] = heap->size - 1;
    heap->pos[lastNode->vertex] = 0;
    
    --heap->size;
    minHeapify(heap, 0);
    
    return root;
}

// Decrease key value of a given vertex
void decreaseKey(MinHeap* heap, int vertex, int distance) {
    int i = heap->pos[vertex];
    heap->array[i]->distance = distance;
    
    while (i && heap->array[i]->distance < heap->array[(i - 1) / 2]->distance) {
        heap->pos[heap->array[i]->vertex] = (i - 1) / 2;
        heap->pos[heap->array[(i - 1) / 2]->vertex] = i;
        swapMinHeapNode(&heap->array[i], &heap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Dijkstra's algorithm implementation
void dijkstra(Graph* g, int source, int dist[], int parent[]) {
    int V = g->numCities;
    MinHeap* heap = createMinHeap(V);
    
    // Initialize distances and parent array
    for (int v = 0; v < V; v++) {
        dist[v] = INT_MAX;
        parent[v] = -1;
        heap->array[v] = (MinHeapNode*)malloc(sizeof(MinHeapNode));
        heap->array[v]->vertex = v;
        heap->array[v]->distance = dist[v];
        heap->pos[v] = v;
    }
    
    // Set source distance to 0
    heap->array[source]->distance = dist[source] = 0;
    decreaseKey(heap, source, dist[source]);
    heap->size = V;
    
    // Process all vertices
    while (!isEmpty(heap)) {
        MinHeapNode* minNode = extractMin(heap);
        int u = minNode->vertex;
        free(minNode);
        
        // Traverse all adjacent vertices
        EdgeNode* edge = g->adjList[u];
        while (edge != NULL) {
            int v = edge->destCity;
            
            // If shorter path found
            if (dist[u] != INT_MAX && 
                edge->distance + dist[u] < dist[v]) {
                dist[v] = dist[u] + edge->distance;
                parent[v] = u;
                decreaseKey(heap, v, dist[v]);
            }
            edge = edge->next;
        }
    }
    
    freeMinHeap(heap);
}

// Recursive helper to print path
void printPathRecursive(Graph* g, int parent[], int j) {
    if (parent[j] == -1)
        return;
    
    printPathRecursive(g, parent, parent[j]);
    printf(" → %s", g->cities[j].name);
}

// Print shortest path from src to dest
void printShortestPath(Graph* g, int src, int dest, int parent[]) {
    if (parent[dest] == -1 && src != dest) {
        printf("No path exists from %s to %s\n", 
               g->cities[src].name, g->cities[dest].name);
        return;
    }
    
    printf(" Route: %s", g->cities[src].name);
    printPathRecursive(g, parent, dest);
    printf("\n");
}

// Get shortest distance between two cities
int getShortestDistance(Graph* g, int src, int dest) {
    int dist[MAX_CITIES];
    int parent[MAX_CITIES];
    dijkstra(g, src, dist, parent);
    return dist[dest];
}

// Free min-heap memory
void freeMinHeap(MinHeap* heap) {
    free(heap->pos);
    free(heap->array);
    free(heap);
}