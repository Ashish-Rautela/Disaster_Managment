// --- FILE: graph.c ---
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new graph with n cities
Graph* createGraph(int n) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) {
        fprintf(stderr, "Memory allocation failed for graph\n");
        exit(1);
    }
    g->numCities = 0;
    for (int i = 0; i < MAX_CITIES; i++) {
        g->adjList[i] = NULL;
    }
    return g;
}

// Add a city to the graph
void addCity(Graph* g, int id, const char* name, int population, 
             int damageLevel, int resources, double lat, double lon) {
    if (g->numCities >= MAX_CITIES) {
        fprintf(stderr, "Maximum city limit reached\n");
        return;
    }
    
    City* city = &g->cities[g->numCities];
    city->id = id;
    strncpy(city->name, name, MAX_NAME_LEN - 1);
    city->name[MAX_NAME_LEN - 1] = '\0';
    city->population = population;
    city->damageLevel = damageLevel;
    city->availableResources = resources;
    city->latitude = lat;
    city->longitude = lon;
    
    g->numCities++;
}

// Add a bidirectional edge between two cities
void addEdge(Graph* g, int src, int dest, int distance) {
    if (src >= g->numCities || dest >= g->numCities || src < 0 || dest < 0) {
        fprintf(stderr, "Invalid city indices\n");
        return;
    }
    
    // Add edge from src to dest
    EdgeNode* newNode = (EdgeNode*)malloc(sizeof(EdgeNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed for edge\n");
        return;
    }
    newNode->destCity = dest;
    newNode->distance = distance;
    newNode->next = g->adjList[src];
    g->adjList[src] = newNode;
    
    // Add edge from dest to src (bidirectional)
    newNode = (EdgeNode*)malloc(sizeof(EdgeNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed for edge\n");
        return;
    }
    newNode->destCity = src;
    newNode->distance = distance;
    newNode->next = g->adjList[dest];
    g->adjList[dest] = newNode;
}

// Display the entire graph network
void displayGraph(Graph* g) {
    printf("\n---------------------------------------------------------------------\n");
    printf("              DISASTER RELIEF CITY NETWORK                         \n");
    printf("---------------------------------------------------------------------\n\n");
    
    for (int i = 0; i < g->numCities; i++) {
        City* city = &g->cities[i];
        printf("  City: %-15s [ID: %d]\n", city->name, city->id);
        printf("   Population: %d | Damage Level: %d/10 | Resources: %d units\n",
               city->population, city->damageLevel, city->availableResources);
        printf("   Coordinates: (%.2f, %.2f)\n", city->latitude, city->longitude);
        printf("   Connected to: ");
        
        EdgeNode* edge = g->adjList[i];
        if (!edge) {
            printf("None\n");
        } else {
            while (edge) {
                printf("%s (%d km)", g->cities[edge->destCity].name, edge->distance);
                edge = edge->next;
                if (edge) printf(", ");
            }
            printf("\n");
        }
        printf("   ---------------------------------------------------------------------\n");
    }
}

// Find city index by name
int findCityByName(Graph* g, const char* name) {
    for (int i = 0; i < g->numCities; i++) {
        if (strcmp(g->cities[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Free all graph memory
void freeGraph(Graph* g) {
    for (int i = 0; i < g->numCities; i++) {
        EdgeNode* current = g->adjList[i];
        while (current) {
            EdgeNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(g);
}