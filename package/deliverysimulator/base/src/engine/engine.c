#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "event/event.h"
#include "../util/errcode.h"

int engine_initialize(struct Engine *engine,
                      double time_end,
                      uint32_t seed,
                      uint64_t num_vehicles,
                      double vehicle_cost,
                      double *unit_cost,
                      double *unit_revenue,
                      uint64_t size,
                      double *tolerance,
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
                               vehicle_cost,
                               unit_cost,
                               unit_revenue,
                               size,
                               tolerance,
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

static void engine_event_print(void *data) {
    struct Event *event = (struct Event*)data;
    event_print(event);
}

static void print_engine(struct Engine *engine) {
    puts("Engine:");
    printf("time_now: %lf\ntime_end: %lf\n",
           engine->time_now,
           engine->time_end);
    if (engine->events.size) {
        puts("Events:");
        priority_queue_print(&engine->events, engine_event_print);
    }
    model_print(&engine->model);
}

static int main_loop(struct Engine *engine, uint8_t verbosity) {
    if (!engine) {
        return ERR_INPUT_NULL;
    }
    engine->time_now = 0;
    priority_queue_clear(&engine->events, 1);

    struct Generator *iter_demand = engine->model.demand;
    for (uint64_t i = 0; i < engine->model.map.size; ++i, ++iter_demand) {
        struct Event *event = 0;
        double next = 0;
        uint32_t amt[3];
        int ret = generator_generate_next(iter_demand, &next);
        if (ret) {
            return ret;
        }
        ret = generator_generate_amount(iter_demand, amt);
        if (ret) {
            return ret;
        }
        ret = event_demand_initialize(&event, i, amt);
        if (ret) {
            return ret;
        }
        ret = priority_queue_add(&engine->events, next, event);
        if (ret) {
            return ret;
        }
    }

    if (verbosity == 2) {
        print_engine(engine);
    }

    for (; engine->events.size && engine->time_now < engine->time_end;) {
        int ret = event_handle(engine->events.heap->data, engine);
        if (ret) {
            return ret;
        }
        if (verbosity == 2) {
            print_engine(engine);
        }
    }
    return ERR_OK;
}

int engine_run(struct Engine *engine, uint64_t iter, uint8_t verbosity) {
    if (!engine) {
        return ERR_INPUT_NULL;
    }
    int ret = ERR_OK;
    for (uint64_t i = 0; i < iter; ++i) {
        ret = main_loop(engine, verbosity);
        if (ret) {
            return ret;
        }
        if (verbosity) {
            printf("iteration: %lu\n"
                   "profit: %lf\n"
                   "num_orders: %lu\n"
                   "num_delivered: %lu\n",
                   i,
                   engine->model.stats.profit,
                   engine->model.stats.num_orders,
                   engine->model.stats.num_delivered);
        }
        ret = model_reset(&engine->model);
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
    priority_queue_finalize(&engine->events, 1);
    return ERR_OK;
}
