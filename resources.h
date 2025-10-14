// --- FILE: resources.h ---
#ifndef RESOURCES_H
#define RESOURCES_H

#include "graph.h"

#define MAX_REQUESTS 1000
#define HASH_SIZE 5000

// Request status
typedef enum {
    PENDING,
    IN_TRANSIT,
    COMPLETED,
    FAILED
    
} Status;

// Disaster request
typedef struct CityRequest {
    int cityId;
    char cityName[MAX_NAME_LEN];
    int urgency;            // 1–10 (10 = most urgent)
    int resourcesNeeded;
    Status status;
} CityRequest;

// Priority queue
typedef struct PriorityQueue {
    CityRequest requests[MAX_REQUESTS];
    int size;
} PriorityQueue;

// Hash entry
typedef struct HashEntry {
    char cityName[MAX_NAME_LEN];
    Status status;
    int resourcesAllocated;
    char supportCity[MAX_NAME_LEN];
    int distance;
    struct HashEntry* next;
} HashEntry;

// Hash map
typedef struct HashMap {
    HashEntry* buckets[HASH_SIZE];
} HashMap;

// Priority queue functions
PriorityQueue* createPriorityQueue();
void insertRequest(PriorityQueue* pq, CityRequest req);
CityRequest extractMostUrgent(PriorityQueue* pq);
int isPQEmpty(PriorityQueue* pq);
void heapifyUp(PriorityQueue* pq, int idx);
void heapifyDown(PriorityQueue* pq, int idx);

// Hash map functions
HashMap* createHashMap();
unsigned int hashFunction(const char* str);
void insertHashEntry(HashMap* map, const char* cityName, Status status,
                     int resources, const char* supportCity, int distance);
HashEntry* getHashEntry(HashMap* map, const char* cityName);
void updateStatus(HashMap* map, const char* cityName, Status status);
void displayResourceStatus(HashMap* map);
void freeHashMap(HashMap* map);

// Resource allocation
void allocateResources(Graph* g, PriorityQueue* pq, HashMap* map);
int findNearestSupportCity(Graph* g, int disasterCity, int resourcesNeeded,
                           int* distance);
void logAllocation(const char* disasterCity, const char* supportCity,
                   int resources, int distance, const char* path);

#endif 