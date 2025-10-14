// --- FILE: main.c ---
#include "graph.h"
#include "dijkstra.h"
#include "resources.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialize sample disaster relief network (Uttarakhand)
void initializeSampleNetwork(Graph* g) {
    // Add cities with realistic data
    addCity(g, 0, "Dehradun", 1200000, 7, 600, 30.3165, 78.0322);
    addCity(g, 1, "Rishikesh", 100000, 5, 400, 30.0869, 78.2676);
    addCity(g, 2, "Haridwar", 230000, 3, 800, 29.9457, 78.1642);
    addCity(g, 3, "Nainital", 42000, 6, 300, 29.3919, 79.4542);
    addCity(g, 4, "Haldwani", 560000, 4, 700, 29.2183, 79.5120);
    addCity(g, 5, "Pithoragarh", 56000, 2, 500, 29.5830, 80.2170);
    addCity(g, 6, "Almora", 35000, 3, 450, 29.5950, 79.6467);

    // Add roads (bidirectional edges with distances in km)
    addEdge(g, 0, 1, 45);     // Dehradun - Rishikesh
    addEdge(g, 1, 2, 25);     // Rishikesh - Haridwar
    addEdge(g, 0, 3, 285);    // Dehradun - Nainital
    addEdge(g, 3, 4, 40);     // Nainital - Haldwani
    addEdge(g, 4, 5, 200);    // Haldwani - Pithoragarh
    addEdge(g, 4, 6, 90);     // Haldwani - Almora
    addEdge(g, 6, 5, 160);    // Almora - Pithoragarh
    addEdge(g, 2, 4, 190);    // Haridwar - Haldwani
    addEdge(g, 1, 3, 275);    // Rishikesh - Nainital

    printf("\n Sample disaster relief network initialized with 7 Uttarakhand cities.\n");
}

// Add a new city to the network
void addNewCity(Graph* g) {
    printf("\n--------------------------------------------------------------------\n");
    printf("!                      ADD NEW CITY                                 !\n");
    printf("--------------------------------------------------------------------\n\n");

    if (g->numCities >= MAX_CITIES) {
        printf(" Maximum city limit reached!\n");
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
    printf("\n City '%s' added successfully!\n", name);
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

    // Initialize with sample data (Uttarakhand)
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
                printf("\nThank you for using the Disaster Relief System!\n");
                printf("All allocation logs saved to: allocation_logs.txt\n\n");
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
