// --- FILE: main.c ---
#include "graph.h"
#include "dijkstra.h"
#include "resources.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialize sample disaster relief network
void initializeSampleNetwork(Graph* g) {
    // Add cities with realistic data
    addCity(g, 0, "Mumbai", 12500000, 8, 500, 19.0760, 72.8777);
    addCity(g, 1, "Pune", 3200000, 2, 1200, 18.5204, 73.8567);
    addCity(g, 2, "Nashik", 1500000, 1, 800, 19.9975, 73.7898);
    addCity(g, 3, "Nagpur", 2400000, 1, 1500, 21.1458, 79.0882);
    addCity(g, 4, "Aurangabad", 1200000, 3, 600, 19.8762, 75.3433);
    addCity(g, 5, "Kolhapur", 550000, 0, 900, 16.7050, 74.2433);
    addCity(g, 6, "Thane", 1850000, 5, 400, 19.2183, 72.9781);
    
    // Add roads (bidirectional edges with distances in km)
    addEdge(g, 0, 1, 150);   // Mumbai - Pune
    addEdge(g, 0, 2, 165);   // Mumbai - Nashik
    addEdge(g, 0, 6, 30);    // Mumbai - Thane
    addEdge(g, 1, 4, 215);   // Pune - Aurangabad
    addEdge(g, 1, 5, 230);   // Pune - Kolhapur
    addEdge(g, 2, 3, 330);   // Nashik - Nagpur
    addEdge(g, 2, 4, 185);   // Nashik - Aurangabad
    addEdge(g, 3, 4, 325);   // Nagpur - Aurangabad
    addEdge(g, 4, 5, 290);   // Aurangabad - Kolhapur
    addEdge(g, 6, 2, 145);   // Thane - Nashik
    
    printf("\n✅ Sample disaster relief network initialized with 7 cities.\n");
}

// Add a new city to the network
void addNewCity(Graph* g) {
    printf("\n╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                      ADD NEW CITY                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    if (g->numCities >= MAX_CITIES) {
        printf("❌ Maximum city limit reached!\n");
        return;
    }
    
    char name[MAX_NAME_LEN];
    getStringInput("Enter city name: ", name, MAX_NAME_LEN);
    
    int population = getIntInput("Enter population: ", 1000, 50000000);
    int damageLevel = getIntInput("Enter damage level (0-10): ", 0, 10);
    int resources = getIntInput("Enter available resources: ", 0, 10000);
    double lat = getDoubleInput("Enter latitude: ");
    double lon = getDoubleInput("Enter longitude: ");
    
    addCity(g, g->numCities, name, population, damageLevel, resources, lat, lon);
    printf("\n✅ City '%s' added successfully!\n", name);
}

// Add a road between two cities
void addNewRoad(Graph* g) {
    printf("\n-------------------------------------------------------------\n");
    printf("!                    ADD ROAD BETWEEN CITIES                        !\n");
    printf("-------------------------------------------------------------\n\n");
    
    if (g->numCities < 2) {
        printf(" Need at least 2 cities to add a road!\n");
        return;
    }
    
    printf("Available cities:\n");
    for (int i = 0; i < g->numCities; i++) {
        printf("  %d. %s\n", i, g->cities[i].name);
    }
    
    int src = getIntInput("\nEnter source city ID: ", 0, g->numCities - 1);
    int dest = getIntInput("Enter destination city ID: ", 0, g->numCities - 1);
    
    if (src == dest) {
        printf(" Source and destination cannot be the same!\n");
        return;
    }
    
    int distance = getIntInput("Enter distance in km: ", 1, 10000);
    
    addEdge(g, src, dest, distance);
    printf("\nRoad added: %s ↔ %s (%d km)\n", 
           g->cities[src].name, g->cities[dest].name, distance);
}

// Raise a disaster request
void raiseDisasterRequest(Graph* g, PriorityQueue* pq, HashMap* map) {
    printf("\n------------------------------------------------------------------------\n");
    printf("!                   RAISE DISASTER REQUEST                          !\n");
    printf("----------------------------------------------------------------------\n\n");
    
    if (g->numCities == 0) {
        printf(" No cities in the network!\n");
        return;
    }
    
    printf("Cities in network:\n");
    for (int i = 0; i < g->numCities; i++) {
        printf("  %d. %s (Damage Level: %d/10)\n", 
               i, g->cities[i].name, g->cities[i].damageLevel);
    }
    
    int cityId = getIntInput("\nEnter disaster city ID: ", 0, g->numCities - 1);
    int urgency = getIntInput("Enter urgency level (1-10, 10=most urgent): ", 1, 10);
    int resourcesNeeded = getIntInput("Enter resources needed: ", 1, 10000);
    
    CityRequest req;
    req.cityId = cityId;
    strncpy(req.cityName, g->cities[cityId].name, MAX_NAME_LEN - 1);
    req.cityName[MAX_NAME_LEN - 1] = '\0';
    req.urgency = urgency;
    req.resourcesNeeded = resourcesNeeded;
    req.status = PENDING;
    
    insertRequest(pq, req);
    insertHashEntry(map, req.cityName, PENDING, 0, "N/A", 0);
}

int main() {
    Graph* graph = createGraph(MAX_CITIES);
    PriorityQueue* pq = createPriorityQueue();
    HashMap* map = createHashMap();
    
    // Initialize with sample data
    initializeSampleNetwork(graph);
    
    int choice;
    int running = 1;
    
    displayBanner();
    pressEnterToContinue();
    
    while (running) {
        clearScreen();
        displayBanner();
        displayMainMenu();
        
        choice = getIntInput("\nEnter your choice: ", 1, 8);
        
        switch (choice) {
            case 1:
                displayGraph(graph);
                pressEnterToContinue();
                break;
                
            case 2:
                addNewCity(graph);
                pressEnterToContinue();
                break;
                
            case 3:
                addNewRoad(graph);
                pressEnterToContinue();
                break;
                
            case 4:
                raiseDisasterRequest(graph, pq, map);
                pressEnterToContinue();
                break;
                
            case 5:
                allocateResources(graph, pq, map);
                pressEnterToContinue();
                break;
                
            case 6:
                displayResourceStatus(map);
                pressEnterToContinue();
                break;
                
            case 7:
                displayLogFile();
                pressEnterToContinue();
                break;
                
            case 8:
                printf("\n Thank you for using the Disaster Relief System!\n");
                printf(" All allocation logs saved to: allocation_logs.txt\n\n");
                running = 0;
                break;
                
            default:
                printf("Invalid choice!\n");
                pressEnterToContinue();
        }
    }
    
    // Cleanup
    freeGraph(graph);
    free(pq);
    freeHashMap(map);
    
    return 0;
}