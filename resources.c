// --- FILE: resources.c ---
#include "resources.h"
#include "dijkstra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

// --- Priority Queue ---
PriorityQueue* createPriorityQueue() {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pq->size = 0;
    return pq;
}

void heapifyUp(PriorityQueue* pq, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (pq->requests[idx].urgency > pq->requests[parent].urgency) {
            CityRequest tmp = pq->requests[idx];
            pq->requests[idx] = pq->requests[parent];
            pq->requests[parent] = tmp;
            idx = parent;
        } else break;
    }
}

void heapifyDown(PriorityQueue* pq, int idx) {
    int size = pq->size;
    while (1) {
        int largest = idx, left = 2 * idx + 1, right = 2 * idx + 2;
        if (left < size && pq->requests[left].urgency > pq->requests[largest].urgency)
            largest = left;
        if (right < size && pq->requests[right].urgency > pq->requests[largest].urgency)
            largest = right;
        if (largest != idx) {
            CityRequest tmp = pq->requests[idx];
            pq->requests[idx] = pq->requests[largest];
            pq->requests[largest] = tmp;
            idx = largest;
        } else break;
    }
}

void insertRequest(PriorityQueue* pq, CityRequest req) {
    if (pq->size >= MAX_REQUESTS) {
        printf("Queue full!\n");
        return;
    }
    pq->requests[pq->size] = req;
    heapifyUp(pq, pq->size);
    pq->size++;
    printf("Added request: %s (Urgency %d, Need %d)\n",
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

// --- HashMap ---
HashMap* createHashMap() {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    for (int i = 0; i < HASH_SIZE; i++)
        map->buckets[i] = NULL;
    return map;
}

unsigned int hashFunction(const char* str) {
    unsigned int hash = 0;
    while (*str)
        hash = (hash * 31) + *str++;
    return hash % HASH_SIZE;
}

void insertHashEntry(HashMap* map, const char* cityName, Status status,
                     int resources, const char* supportCity, int distance) {
    unsigned int idx = hashFunction(cityName);
    HashEntry* cur = map->buckets[idx];
    while (cur) {
        if (strcmp(cur->cityName, cityName) == 0) {
            cur->status = status;
            cur->resourcesAllocated = resources;
            strncpy(cur->supportCity, supportCity, MAX_NAME_LEN - 1);
            cur->distance = distance;
            return;
        }
        cur = cur->next;
    }

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
    HashEntry* cur = map->buckets[idx];
    while (cur) {
        if (strcmp(cur->cityName, cityName) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

void updateStatus(HashMap* map, const char* cityName, Status status) {
    HashEntry* e = getHashEntry(map, cityName);
    if (e) {
        e->status = status;
        const char* s[] = {"PENDING", "IN_TRANSIT", "COMPLETED"};
        printf("Status updated: %s → %s\n", cityName, s[status]);
    }
}

void displayResourceStatus(HashMap* map) {
    const char* s[] = {"PENDING", "IN_TRANSIT", "COMPLETED"};
    printf("\n------------------- RESOURCE STATUS -------------------\n\n");

    int found = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry* cur = map->buckets[i];
        while (cur) {
            found = 1;
            printf("City: %-20s | Status: %-12s\n",
                   cur->cityName, s[cur->status]);
            printf("Resources: %d | Support: %s (%d km)\n",
                   cur->resourcesAllocated, cur->supportCity, cur->distance);
            printf("-------------------------------------------------------\n");
            cur = cur->next;
        }
    }
    if (!found)
        printf("No allocations recorded yet.\n\n");
}

void freeHashMap(HashMap* map) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry* cur = map->buckets[i];
        while (cur) {
            HashEntry* tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
    free(map);
}

// --- Resource Allocation ---
int findNearestSupportCity(Graph* g, int disasterCity, int need, int* distance) {
    int dist[MAX_CITIES], parent[MAX_CITIES];
    dijkstra(g, disasterCity, dist, parent);

    int nearest = -1, minDist = INT_MAX;
    for (int i = 0; i < g->numCities; i++) {
        if (i != disasterCity &&
            g->cities[i].availableResources >= need &&
            g->cities[i].damageLevel < 3 &&
            dist[i] < minDist) {
            minDist = dist[i];
            nearest = i;
            printf("%s: dist=%d, res=%d, damage=%d\n",
                   g->cities[i].name, dist[i],
                   g->cities[i].availableResources,
                   g->cities[i].damageLevel);
        }
    }
    *distance = minDist;
    return nearest;
}

void logAllocation(const char* dCity, const char* sCity,
                   int res, int dist, const char* path) {
    FILE* fp = fopen("allocation_logs.txt", "a");
    if (!fp) return;

    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char timeStr[26];
    strftime(timeStr, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(fp, "-------------------------------------------------------\n");
    fprintf(fp, "Timestamp: %s\n", timeStr);
    fprintf(fp, "Disaster: %s | Support: %s | Res: %d | Dist: %d km\n",
            dCity, sCity, res, dist);
    fprintf(fp, "Route: %s\n", path);
    fprintf(fp, "-------------------------------------------------------\n\n");
    fclose(fp);
}

void allocateResources(Graph* g, PriorityQueue* pq, HashMap* map) {
    if (isPQEmpty(pq)) {
        printf("\nNo pending requests.\n");
        return;
    }

    CityRequest req = extractMostUrgent(pq);
    printf("\nProcessing request: %s | Urgency %d | Need %d\n",
           req.cityName, req.urgency, req.resourcesNeeded);

    int dist[MAX_CITIES], parent[MAX_CITIES];
    dijkstra(g, req.cityId, dist, parent);

    int remaining = req.resourcesNeeded, total = 0, donors = 0;
    int order[MAX_CITIES];
    for (int i = 0; i < g->numCities; i++) order[i] = i;

    for (int i = 0; i < g->numCities - 1; i++)
        for (int j = i + 1; j < g->numCities; j++)
            if (dist[order[i]] > dist[order[j]]) {
                int t = order[i];
                order[i] = order[j];
                order[j] = t;
            }

    FILE* fp = fopen("allocation_logs.txt", "a");
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char timeStr[26];
    strftime(timeStr, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(fp, "-------------------------------------------------------\n");
    fprintf(fp, "Timestamp: %s\nDisaster City: %s | Need: %d\n",
            timeStr, req.cityName, req.resourcesNeeded);

    for (int k = 0; k < g->numCities && remaining > 0; k++) {
        int i = order[k];
        if (i == req.cityId || g->cities[i].damageLevel > 6 ||
            g->cities[i].availableResources <= 0 || dist[i] == INF)
            continue;

        int give = (g->cities[i].availableResources >= remaining)
                   ? remaining : g->cities[i].availableResources;

        if (give <= 0) continue;

        g->cities[i].availableResources -= give;
        remaining -= give;
        total += give;
        donors++;

        printf("Support: %s | Sent: %d | Dist: %d | Remain: %d\n",
               g->cities[i].name, give, dist[i], remaining);
        fprintf(fp, "Support: %s | Sent: %d | Dist: %d km\n",
                g->cities[i].name, give, dist[i]);
    }

    if (remaining > 0) {
        printf("\nInsufficient resources. %d units still needed.\n", remaining);
        insertHashEntry(map, req.cityName, FAILED, total,
                        (donors > 0 ? "Partial" : "N/A"), 0);
        fprintf(fp, "Status: PARTIAL/FAILED (%d unfilled)\n", remaining);
    } else {
        printf("\nRequest fulfilled using %d support cities.\n", donors);
        insertHashEntry(map, req.cityName, IN_TRANSIT, total, "Multiple", 0);
        fprintf(fp, "Status: SUCCESS\n");
    }

    fprintf(fp, "-------------------------------------------------------\n\n");
    fclose(fp);
    printf("\nAllocation logged to file.\n");
}
