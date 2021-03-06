#ifndef PRIORITY_QUEUE_H__
#define PRIORITY_QUEUE_H__

#include <stdint.h>

struct PriorityQueueNode {
    double key;
    void *data;
};

struct PriorityQueue {
    uint64_t capacity, size;
    struct PriorityQueueNode *heap;
    struct PriorityQueueNode *tail;
};

int priority_queue_initialize(struct PriorityQueue *pq, uint64_t capacity);
int graph_key_comp(double a, double b);
int priority_queue_add(struct PriorityQueue *pq, double key, void *data);
int priority_queue_remove(struct PriorityQueue *pq, char remove_data);
int priority_queue_clear(struct PriorityQueue *pq, char remove_data);
int priority_queue_finalize(struct PriorityQueue *pq, char remove_data);
int priority_queue_print(struct PriorityQueue *pq, void (*handler)(void*));

#endif
