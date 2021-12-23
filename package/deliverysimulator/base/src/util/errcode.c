#include "errcode.h"

const char *errcode_lookup[9] = {
    "ERR_OK",
    "ERR_INPUT_NULL",
    "ERR_OUT_OF_MEM",
    "ERR_LIST_POP_EMPTY",
    "ERR_PQ_REMOVE_EMPTY",
    "ERR_GRAPH_INITIALIZE_IDX",
    "ERR_GRAPH_GET_SHORTEST_IDX",
    "ERR_GRAPH_GET_SHORTEST_NOT_FOUND",
    "ERR_MODEL_PARK_VEHICLE_IDX"
};
