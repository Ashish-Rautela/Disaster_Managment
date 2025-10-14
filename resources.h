// --- FILE: resources.h ---
#ifndef RESOURCES_H
#define RESOURCES_H

#include "graph.h"

#define MAX_REQUESTS 1000
#define HASH_SIZE 50

// Status enumeration
typedef enum {
    PENDING,
    IN_TRANSIT,
    COMPLETED,
    FAILED
} Status;

// City request structure for priority queue
typedef struct CityRequest {
    int cityId;
    char cityName[MAX_NAME_LEN];
    int urgency;            // 1-10 scale (10 = most urgent)
    int resourcesNeeded;
    Status status;
} CityRequest;

// Priority queue for disaster requests
typedef struct PriorityQueue {
    CityRequest requests[MAX_REQUESTS];
    int size;
} PriorityQueue;

// HashMap entry for tracking request status
typedef struct HashEntry {
    char cityName[MAX_NAME_LEN];
    Status status;
    int resourcesAllocated;
    char supportCity[MAX_NAME_LEN];
    int distance;
    struct HashEntry* next;
} HashEntry;

// HashMap structure
typedef struct HashMap {
    HashEntry* buckets[HASH_SIZE];
} HashMap;

// Priority Queue functions
PriorityQueue* createPriorityQueue();
void insertRequest(PriorityQueue* pq, CityRequest req);
CityRequest extractMostUrgent(PriorityQueue* pq);
int isPQEmpty(PriorityQueue* pq);
void heapifyUp(PriorityQueue* pq, int idx);
void heapifyDown(PriorityQueue* pq, int idx);

// HashMap functions
HashMap* createHashMap();
unsigned int hashFunction(const char* str);
void insertHashEntry(HashMap* map, const char* cityName, Status status, 
                     int resources, const char* supportCity, int distance);
HashEntry* getHashEntry(HashMap* map, const char* cityName);
void updateStatus(HashMap* map, const char* cityName, Status status);
void displayResourceStatus(HashMap* map);
void freeHashMap(HashMap* map);

// Resource allocation functions
void allocateResources(Graph* g, PriorityQueue* pq, HashMap* map);
int findNearestSupportCity(Graph* g, int disasterCity, int resourcesNeeded, 
                           int* distance);
void logAllocation(const char* disasterCity, const char* supportCity, 
                   int resources, int distance, const char* path);

#endif // RESOURCES_H