#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "event/event.h"
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

static void print_engine(struct Engine *engine) {
    printf("time_now: %lf\ntime_end: %lf\n",
           engine->time_now,
           engine->time_end);
    if (engine->events.size) {
        event_print(engine->events.heap->data);
    }
    model_print(&engine->model);
}

static int main_loop(struct Engine *engine, uint8_t verbosity) {
    if (!engine) {
        return ERR_INPUT_NULL;
    }
    engine->time_now = 0;

    if (verbosity == 2) {
        print_engine(engine);
    }
    return ERR_OK;
}

int engine_run(struct Engine *engine, uint64_t iter, uint8_t verbosity) {
    if (!engine) {
        return ERR_INPUT_NULL;
    }
    int ret = ERR_OK;
    for (uint64_t i = 0; i < iter; ++i) {
        if (verbosity) {
            printf("iteration: %lu\n", i);
        }
        ret = main_loop(engine, verbosity);
        if (ret) {
            return ret;
        }
    }
    return ret;
}

int engine_finalize(struct Engine *engine) {
    if (!engine) {
        return ERR_INPUT_NULL;
    }
    model_finalize(&engine->model);
    priority_queue_finalize(&engine->events);
    return ERR_OK;
}
