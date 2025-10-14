// --- FILE: resources.c ---
#include "resources.h"
#include "dijkstra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

// ============ PRIORITY QUEUE IMPLEMENTATION ============

PriorityQueue* createPriorityQueue() {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pq->size = 0;
    return pq;
}

void heapifyUp(PriorityQueue* pq, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (pq->requests[idx].urgency > pq->requests[parent].urgency) {
            CityRequest temp = pq->requests[idx];
            pq->requests[idx] = pq->requests[parent];
            pq->requests[parent] = temp;
            idx = parent;
        } else {
            break;
        }
    }
}

void heapifyDown(PriorityQueue* pq, int idx) {
    int size = pq->size;
    while (1) {
        int largest = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        
        if (left < size && pq->requests[left].urgency > pq->requests[largest].urgency)
            largest = left;
        
        if (right < size && pq->requests[right].urgency > pq->requests[largest].urgency)
            largest = right;
        
        if (largest != idx) {
            CityRequest temp = pq->requests[idx];
            pq->requests[idx] = pq->requests[largest];
            pq->requests[largest] = temp;
            idx = largest;
        } else {
            break;
        }
    }
}

void insertRequest(PriorityQueue* pq, CityRequest req) {
    if (pq->size >= MAX_REQUESTS) {
        printf(" Priority queue full!\n");
        return;
    }
    
    pq->requests[pq->size] = req;
    heapifyUp(pq, pq->size);
    pq->size++;
    
    printf("Disaster request added: %s (Urgency: %d/10, Resources needed: %d)\n",
           req.cityName, req.urgency, req.resourcesNeeded);
}

CityRequest extractMostUrgent(PriorityQueue* pq) {
    CityRequest req = pq->requests[0];
    pq->requests[0] = pq->requests[pq->size - 1];
    pq->size--;
    heapifyDown(pq, 0);
    return req;
}

int isPQEmpty(PriorityQueue* pq) {
    return pq->size == 0;
}

// ============ HASHMAP IMPLEMENTATION ============

HashMap* createHashMap() {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    for (int i = 0; i < HASH_SIZE; i++) {
        map->buckets[i] = NULL;
    }
    return map;
}

unsigned int hashFunction(const char* str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash * 31) + *str;
        str++;
    }
    return hash % HASH_SIZE;
}

void insertHashEntry(HashMap* map, const char* cityName, Status status, 
                     int resources, const char* supportCity, int distance) {
    unsigned int idx = hashFunction(cityName);
    
    // Check if entry exists
    HashEntry* current = map->buckets[idx];
    while (current) {
        if (strcmp(current->cityName, cityName) == 0) {
            current->status = status;
            current->resourcesAllocated = resources;
            strncpy(current->supportCity, supportCity, MAX_NAME_LEN - 1);
            current->distance = distance;
            return;
        }
        current = current->next;
    }
    
    // Create new entry
    HashEntry* entry = (HashEntry*)malloc(sizeof(HashEntry));
    strncpy(entry->cityName, cityName, MAX_NAME_LEN - 1);
    entry->cityName[MAX_NAME_LEN - 1] = '\0';
    entry->status = status;
    entry->resourcesAllocated = resources;
    strncpy(entry->supportCity, supportCity, MAX_NAME_LEN - 1);
    entry->supportCity[MAX_NAME_LEN - 1] = '\0';
    entry->distance = distance;
    entry->next = map->buckets[idx];
    map->buckets[idx] = entry;
}

HashEntry* getHashEntry(HashMap* map, const char* cityName) {
    unsigned int idx = hashFunction(cityName);
    HashEntry* current = map->buckets[idx];
    
    while (current) {
        if (strcmp(current->cityName, cityName) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void updateStatus(HashMap* map, const char* cityName, Status status) {
    HashEntry* entry = getHashEntry(map, cityName);
    if (entry) {
        entry->status = status;
        const char* statusStr[] = {"PENDING", "IN_TRANSIT", "COMPLETED"};
        printf("📊 Status updated: %s → %s\n", cityName, statusStr[status]);
    }
}

void displayResourceStatus(HashMap* map) {
    const char* statusStr[] = {"PENDING", "IN_TRANSIT", "COMPLETED"};

    printf("\n---------------------------------------------------------------------\n");
    printf("              RESOURCE ALLOCATION STATUS                           \n");
    printf("---------------------------------------------------------------------\n\n");

    int found = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry* current = map->buckets[i];
        while (current) {
            found = 1;
            printf("🏙️  City: %-20s | Status: %-12s\n", 
                   current->cityName, statusStr[current->status]);
            printf("   Resources Allocated: %d units\n", current->resourcesAllocated);
            printf("   Support from: %s (%d km)\n", 
                   current->supportCity, current->distance);
            printf("   ---------------------------------------------------------------------\n");
            current = current->next;
        }
    }
    
    if (!found) {
        printf("   No allocations recorded yet.\n\n");
    }
}

void freeHashMap(HashMap* map) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry* current = map->buckets[i];
        while (current) {
            HashEntry* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(map);
}

// ============ RESOURCE ALLOCATION ============

int findNearestSupportCity(Graph* g, int disasterCity, int resourcesNeeded, 
                           int* distance) {
    int dist[MAX_CITIES];
    int parent[MAX_CITIES];
    dijkstra(g, disasterCity, dist, parent);
    
    int nearestCity = -1;
    int minDist = INT_MAX;
    
    for (int i = 0; i < g->numCities; i++) {
        if (i != disasterCity && 
            g->cities[i].availableResources >= resourcesNeeded &&
            g->cities[i].damageLevel < 3 &&
            dist[i] < minDist) {
            minDist = dist[i];
            nearestCity = i;
                printf("%s: dist=%d, res=%d, damage=%d\n",
           g->cities[i].name, dist[i],
           g->cities[i].availableResources,
           g->cities[i].damageLevel);
        }
    }
    
    *distance = minDist;
    return nearestCity;
}

void logAllocation(const char* disasterCity, const char* supportCity, 
                   int resources, int distance, const char* path) {
    FILE* fp = fopen("allocation_logs.txt", "a");
    if (!fp) {
        printf("  Warning: Could not open log file\n");
        return;
    }
    
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char timeStr[26];
    strftime(timeStr, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(fp, "-------------------------------------------------------------------\n");
    fprintf(fp, "Timestamp: %s\n", timeStr);
    fprintf(fp, "Disaster City: %s\n", disasterCity);
    fprintf(fp, "Support City: %s\n", supportCity);
    fprintf(fp, "Resources Allocated: %d units\n", resources);
    fprintf(fp, "Distance: %d km\n", distance);
    fprintf(fp, "Route: %s\n", path);
    fprintf(fp, "---------------------------------------------------------------------\n\n");
    
    fclose(fp);
}

void allocateResources(Graph* g, PriorityQueue* pq, HashMap* map) {
    if (isPQEmpty(pq)) {
        printf("\n  No pending disaster requests.\n");
        return;
    }
    
    CityRequest req = extractMostUrgent(pq);

    printf("\n---------------------------------------------------------------------\n");
    printf("              PROCESSING DISASTER REQUEST\n");
    printf("---------------------------------------------------------------------\n");
    printf(" Disaster City: %s\n", req.cityName);
    printf(" Urgency Level: %d/10\n", req.urgency);
    printf(" Resources Needed: %d units\n\n", req.resourcesNeeded);

    int dist[MAX_CITIES], parent[MAX_CITIES];
    dijkstra(g, req.cityId, dist, parent);

    int remaining = req.resourcesNeeded;
    int totalAllocated = 0;
    int donorCount = 0;

    // Sorting cities by distance (simple selection sort)
    int order[MAX_CITIES];
    for (int i = 0; i < g->numCities; i++) order[i] = i;
    for (int i = 0; i < g->numCities - 1; i++) {
        for (int j = i + 1; j < g->numCities; j++) {
            if (dist[order[i]] > dist[order[j]]) {
                int tmp = order[i];
                order[i] = order[j];
                order[j] = tmp;
            }
        }
    }

    // Open log file
    FILE* fp = fopen("allocation_logs.txt", "a");
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char timeStr[26];
    strftime(timeStr, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(fp, "-------------------------------------------------------------------\n");
    fprintf(fp, "Timestamp: %s\n", timeStr);
    fprintf(fp, "Disaster City: %s\n", req.cityName);
    fprintf(fp, "Resources Needed: %d units\n", req.resourcesNeeded);

    printf(" Allocating resources from nearest cities...\n");

    for (int k = 0; k < g->numCities && remaining > 0; k++) {
        int i = order[k];
        if (i == req.cityId) continue;
        if (g->cities[i].damageLevel > 6) continue; // skip heavily damaged
        if (g->cities[i].availableResources <= 0) continue;
        if (dist[i] == INF) continue;

        int give = (g->cities[i].availableResources >= remaining)
                     ? remaining
                     : g->cities[i].availableResources;

        if (give <= 0) continue;

        g->cities[i].availableResources -= give;
        remaining -= give;
        totalAllocated += give;
        donorCount++;

        printf("\n Support City: %s", g->cities[i].name);
        printf("\n   Distance: %d km", dist[i]);
        printf("\n   Sent: %d units", give);
        printf("\n   Remaining Need: %d units\n", remaining);

        fprintf(fp, "Support City: %s | Sent: %d | Distance: %d km\n",
                g->cities[i].name, give, dist[i]);
    }

    // Final status
    if (remaining > 0) {
        printf("\n  Still need %d more units! Not enough resources nearby.\n", remaining);
        insertHashEntry(map, req.cityName, FAILED, totalAllocated,
                        (donorCount > 0 ? "Partial" : "N/A"), 0);
        fprintf(fp, "Status: PARTIAL/FAILED (Unfulfilled %d units)\n", remaining);
    } else {
        printf("\n Request fulfilled successfully using %d support cities!\n", donorCount);
        insertHashEntry(map, req.cityName, IN_TRANSIT, totalAllocated, "Multiple", 0);
        fprintf(fp, "Status: SUCCESS\n");
    }

    fprintf(fp, "-------------------------------------------------------------------\n\n");
    fclose(fp);

    printf("\n💾 Allocation logged to file: allocation_logs.txt\n");
}