#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "../errcode.h"
#include "list.h"
#include "priority_queue.h"

struct GraphEdge {
    struct GraphNode *to;
    double distance;
};

int graph_initialize(struct Graph *graph,
                     uint64_t size,
                     uint64_t *from,
                     uint64_t *to,
                     double *distance,
                     uint64_t edge) {
    if (!graph || !from || !to || !distance) {
        return ERR_INPUT_NULL;
    }
    graph->size = size;
    graph->adjacency_list = malloc(sizeof(struct GraphNode) * size);
    if (!graph->adjacency_list) {
        return ERR_OUT_OF_MEM;
    }
    struct GraphNode *iter_al = graph->adjacency_list;
    int ret = ERR_OK;
    for (uint64_t i = 0; i < size; ++i, ++iter_al) {
        iter_al->idx = i;
        ret = list_initialize(&iter_al->edges);
        if (ret) {
            return ret;
        }
    }
    uint64_t *iter_from = from;
    uint64_t *iter_to = to;
    double *iter_distance = distance;
    for (uint64_t i = 0;
         i < edge;
         ++i,
         ++iter_from,
         ++iter_to,
         ++iter_distance) {
        if (*iter_from >= size || *iter_to >= size) {
            return ERR_GRAPH_INITIALIZE_IDX;
        }
        struct GraphEdge *temp = malloc(sizeof(struct GraphEdge));
        if (!temp) {
            return ERR_OUT_OF_MEM;
        }
        temp->to = graph->adjacency_list + *iter_to;
        temp->distance = *iter_distance;
        ret = list_push_back(&graph->adjacency_list[*iter_from].edges, temp);
        if (ret) {
            return ret;
        }
        temp = malloc(sizeof(struct GraphEdge));
        if (!temp) {
            return ERR_OUT_OF_MEM;
        }
        temp->to = graph->adjacency_list + *iter_from;
        temp->distance = *iter_distance;
        ret = list_push_back(&graph->adjacency_list[*iter_to].edges, temp);
        if (ret) {
            return ret;
        }
    }
    return ERR_OK;
}

int graph_get_shortest(struct Graph *graph,
                       uint64_t from,
                       uint64_t to,
                       struct GraphNode ***path,
                       uint64_t *size) {
    if (!graph || !path || !size) {
        return ERR_INPUT_NULL;
    }
    if (from >= graph->size || to >= graph->size) {
        return ERR_GRAPH_GET_SHORTEST_IDX;
    }
    struct PriorityQueue pq;
    int ret = priority_queue_initialize(&pq, graph->size);
    if (ret) {
        return ret;
    }
    struct GraphNode *iter_al = graph->adjacency_list;
    for (uint64_t i = 0; i < graph->size; ++i, ++iter_al) {
        iter_al->distance = i == from ? 0 : -1;
        iter_al->prev = 0;
        iter_al->visited = 0;
        priority_queue_add(&pq, iter_al->distance, iter_al);
    }
    char found = 0;
    for (; pq.size;) {
        struct GraphNode *current = (pq.heap)->data;
        ret = priority_queue_remove(&pq);
        if (ret) {
            return priority_queue_finalize(&pq);
        }
        if (current->idx == to) {
            found = 1;
            break;
        }
        if (current->visited) {
            continue;
        }
        current->visited = 1;
        for (struct ListNode *iter = current->edges.front;
             iter;
             iter = iter->next) {
            struct GraphEdge *cur = iter->data;
            double new_key = current->distance + cur->distance;
            if (graph_key_comp(new_key,
                               graph->adjacency_list[cur->to->idx].distance) <
                0) {
                graph->adjacency_list[cur->to->idx].distance = new_key;
                graph->adjacency_list[cur->to->idx].prev = current;
                ret = priority_queue_add(&pq,
                                         new_key,
                                         graph->adjacency_list + cur->to->idx);
                if (ret) {
                    return ret;
                }
            }
        }
    }

    ret = priority_queue_finalize(&pq);
    if (ret) {
        return ret;
    }

    if (!found) {
        return ERR_GRAPH_GET_SHORTEST_NOT_FOUND;
    }

    struct List list;
    ret = list_initialize(&list);
    if (ret) {
        return ret;
    }

    uint64_t s = 0;
    for (struct GraphNode *i = graph->adjacency_list + to;
         i;
         i = i->prev,
         ++s) {
        ret = list_push_front(&list, i);
    }

    *path = malloc(sizeof(struct GraphNode*) * s);
    if (!*path) {
        return ERR_OUT_OF_MEM;
    }
    *size = s;
    struct GraphNode **iter_path = *path;
    struct ListNode *iter_list = list.front;
    for (uint64_t i = 0; i < s; ++i, ++iter_path, iter_list = iter_list->next) {
        *iter_path = iter_list->data;
    }
    ret = list_finalize(&list, 0);
    if (ret) {
        return ret;
    }
    return ERR_OK;
}

static void print_edge(void *data) {
    struct GraphEdge *ge = (struct GraphEdge*)data;
    printf("to: %lu distance %lf\n",
           ge->to->idx,
           ge->distance);
}

static int print_node(struct GraphNode *gn) {
    printf("idx: %lu\n", gn->idx);
    int ret = list_print(&gn->edges, print_edge);
    if (ret) {
        return ret;
    }
    return ERR_OK;
}

int graph_print(struct Graph *graph) {
    if (!graph) {
        return ERR_INPUT_NULL;
    }
    struct GraphNode *iter = graph->adjacency_list;
    for (uint64_t i = 0; i < graph->size; ++i, ++iter) {
        int ret = print_node(iter);
        if (ret) {
            return ret;
        }
    }
    return ERR_OK;
}

int graph_print_path(struct GraphNode **path, uint64_t size) {
    if (!path) {
        return ERR_INPUT_NULL;
    }
    printf("distance: %lf\n", path[size - 1]->distance);
    struct GraphNode **iter = path;
    for (uint64_t i = 0; i < size; ++i, ++iter) {
        printf("idx: %lu\ndistance: %lf", (*iter)->idx, (*iter)->distance);
        if (i < size - 1) {
            printf("%s", " ->");
        }
        putchar(10);
    }
    return ERR_OK;
}

int graph_path_finalize(struct GraphNode **path) {
    if (!path) {
        return ERR_INPUT_NULL;
    }
    free(path);
    return ERR_OK;
}

int graph_finalize(struct Graph *graph) {
    if (!graph) {
        return ERR_INPUT_NULL;
    }
    struct GraphNode *iter = graph->adjacency_list;
    for (uint64_t i = 0; i < graph->size; ++i, ++iter) {
        int ret = list_finalize(&iter->edges, 1);
        if (ret) {
            return ret;
        }
    }
    free(graph->adjacency_list);
    return ERR_OK;
}
