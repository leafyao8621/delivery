#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.h"
#include "../errcode.h"

int priority_queue_initialize(struct PriorityQueue *pq, uint64_t capacity) {
    if (!pq) {
        return ERR_INPUT_NULL;
    }
    pq->capacity = capacity;
    pq->size = 0;
    pq->heap = malloc(sizeof(struct PriorityQueueNode) * capacity);
    if (!pq->heap) {
        return ERR_OUT_OF_MEM;
    }
    pq->tail = pq->heap;
    return ERR_OK;
}

static void swap(struct PriorityQueueNode *a, struct PriorityQueueNode *b) {
    struct PriorityQueueNode temp = *a;
    *a = *b;
    *b = temp;
}

int graph_key_comp(double a, double b) {
    if (a == -1 && b == -1) {
        return 0;
    } else if (a == -1) {
        return 1;
    } else if (b == -1) {
        return -1;
    } else {
        return a - b;
    }
}
static void heapify_up(struct PriorityQueueNode *heap,
                       struct PriorityQueueNode *tail,
                       uint64_t idx) {
    uint64_t cur_idx = idx;
    struct PriorityQueueNode *cur = tail;
    uint64_t next_idx = cur_idx >> 1;
    for (; cur_idx;) {
        next_idx = cur_idx >> 1;
        if (graph_key_comp(heap[next_idx].key, cur->key) > 0) {
            swap(heap + next_idx, cur);
            cur_idx = next_idx;
            cur = heap + next_idx;
        } else {
            break;
        }
    }
}

int priority_queue_add(struct PriorityQueue *pq, double key, void *data) {
    if (!pq) {
        return ERR_INPUT_NULL;
    }
    if (pq->size == pq->capacity) {
        pq->capacity <<= 1;
        pq->heap = realloc(pq->heap,
                           sizeof(struct PriorityQueue) * pq->capacity);
        if (!pq->heap) {
            return ERR_OUT_OF_MEM;
        }
        pq->tail = pq->heap + pq->size;
    }
    pq->tail->key = key;
    pq->tail->data = data;
    heapify_up(pq->heap, pq->tail, pq->size);
    ++pq->size;
    ++pq->tail;
    return ERR_OK;
}

static void heapify_down(struct PriorityQueueNode *head,
                         uint64_t size) {
    struct PriorityQueueNode *cur = head;
    for (uint64_t idx = 0; idx < size;) {
        uint64_t left_idx = (idx << 1) + 1;
        uint64_t right_idx = left_idx + 1;
        struct PriorityQueueNode *left = head + left_idx;
        struct PriorityQueueNode *right = left + 1;
        if (left_idx < size && right_idx < size) {
            char side = graph_key_comp(left->key, right->key) < 0;
            double key = side ? left->key : right->key;
            uint64_t next_idx = 0;
            struct PriorityQueueNode *next = 0;
            next_idx = side ? left_idx : right_idx;
            next = side ? left : right;
            if (graph_key_comp(cur->key, key) < 0) {
                break;
            }
            swap(next, cur);
            cur = next;
            idx = next_idx;
        } else if (left_idx < size) {
            if (graph_key_comp(cur->key, left->key) < 0) {
                break;
            }
            swap(left, cur);
            cur = left;
            idx = left_idx;
        } else {
            break;
        }
    }
}

int priority_queue_remove(struct PriorityQueue *pq) {
    if (!pq) {
        return ERR_INPUT_NULL;
    }
    --pq->size;
    --pq->tail;
    swap(pq->tail, pq->heap);
    heapify_down(pq->heap, pq->size);
    return ERR_OK;
}

int priority_queue_finalize(struct PriorityQueue *pq) {
    if (!pq) {
        return ERR_INPUT_NULL;
    }
    free(pq->heap);
    return ERR_OK;
}

int priority_queue_print(struct PriorityQueue *pq, void (*handler)(void*)) {
    if (!pq || !handler) {
        return ERR_INPUT_NULL;
    }
    printf("capacity: %lu\nsize: %lu\n", pq->capacity, pq->size);
    struct PriorityQueueNode *iter = pq->heap;
    for (uint64_t i = 0; i < pq->size; ++i, ++iter) {
        printf("key: %lf\n", iter->key);
        handler(iter->data);
    }
    return ERR_OK;
}
