#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <stdint.h>
#include "list.h"

struct GraphNode {
    uint64_t idx;
    char visited;
    struct GraphNode *prev;
    double distance;
    struct List edges;
};

struct Graph {
    uint64_t size;
    struct GraphNode *adjacency_list;
};

int graph_initialize(struct Graph *graph,
                     uint64_t size,
                     uint64_t *from,
                     uint64_t *to,
                     double *distance,
                     uint64_t edge);
int graph_get_shortest(struct Graph *graph,
                       uint64_t from,
                       uint64_t to,
                       struct GraphNode ***path,
                       uint64_t *size);
int graph_print(struct Graph *graph);
int graph_print_path(struct GraphNode **path, uint64_t size);
int graph_path_finalize(struct GraphNode **path);
int graph_finalize(struct Graph *graph);

#endif
