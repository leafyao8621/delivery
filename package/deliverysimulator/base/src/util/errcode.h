#ifndef ERRCODE_H__
#define ERRCODE_H__

#define ERR_OK 0
#define ERR_INPUT_NULL 1
#define ERR_OUT_OF_MEM 2
#define ERR_LIST_POP_EMPTY 3
#define ERR_PQ_REMOVE_EMPTY 4
#define ERR_GRAPH_INITIALIZE_IDX 5
#define ERR_GRAPH_GET_SHORTEST_IDX 6
#define ERR_GRAPH_GET_SHORTEST_NOT_FOUND 7
#define ERR_MODEL_PARK_VEHICLE_IDX 8

extern const char *errcode_lookup[9];

#endif
