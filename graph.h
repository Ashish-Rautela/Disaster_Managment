// --- FILE: graph.h ---
#ifndef GRAPH_H
#define GRAPH_H
#define INF 999999
#define MAX_CITIES 50
#define MAX_NAME_LEN 50

// City structure with detailed information
typedef struct City {
    int id;
    char name[MAX_NAME_LEN];
    int population;
    int damageLevel;        // 0-10 scale
    int availableResources; // Units of resources
    double latitude;
    double longitude;
} City;

// Edge node for adjacency list
typedef struct EdgeNode {
    int destCity;
    int distance;           // Distance in km
    struct EdgeNode* next;
} EdgeNode;

// Graph structure
typedef struct Graph {
    int numCities;
    City cities[MAX_CITIES];
    EdgeNode* adjList[MAX_CITIES];
} Graph;

// Function declarations
Graph* createGraph(int n);
void addCity(Graph* g, int id, const char* name, int population, 
             int damageLevel, int resources, double lat, double lon);
void addEdge(Graph* g, int src, int dest, int distance);
void displayGraph(Graph* g);
void freeGraph(Graph* g);
int findCityByName(Graph* g, const char* name);

#endif // GRAPH_H