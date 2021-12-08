#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "../util/errcode.h"

int engine_initialize(struct Engine *engine,
                      double time_end,
                      uint32_t seed,
                      uint64_t num_vehicles,
                      uint64_t size,
                      uint64_t *from,
                      uint64_t *to,
                      double *distance,
                      uint64_t edge,
                      double *rate,
                      uint32_t *upper_limit,
                      double *production_rate) {
    if (!engine) {
        return ERR_INPUT_NULL;
    }
    engine->time_end = time_end;
    mt19937_initialize(&engine->gen, seed);
    int ret = model_initialize(&engine->model,
                               &engine->gen,
                               num_vehicles,
                               size,
                               from,
                               to,
                               distance,
                               edge,
                               rate,
                               upper_limit,
                               production_rate);
    if (ret) {
        return ret;
    }
    ret = priority_queue_initialize(&engine->events, 1000);
    if (ret) {
        model_finalize(&engine->model);
        return ret;
    }
    return ERR_OK;
}

int engine_finalize(struct Engine *engine) {
    if (!engine) {
        return ERR_INPUT_NULL;
    }
    model_finalize(&engine->model);
    priority_queue_finalize(&engine->events);
    return ERR_OK;
}
