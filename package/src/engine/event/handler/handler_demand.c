#include <stdio.h>
#include <stdlib.h>
#include "../handler.h"
#include "../event.h"
#include "../../engine.h"
#include "../../../util/errcode.h"

int event_demand_handler(struct Event *event, struct Engine *engine) {
    if (!event || !engine) {
        return ERR_INPUT_NULL;
    }
    uint64_t idx = event->data.demand.idx;
    double next = 0;
    uint32_t amt[3];
    struct Event *next_event = 0;
    int ret = generator_generate_next(&engine->model.demand[idx], &next);
    if (ret) {
        return ret;
    }
    next += engine->time_now;
    ret = generator_generate_amount(&engine->model.demand[idx], amt);
    if (ret) {
        return ret;
    }
    ret = event_demand_initialize(&next_event, idx, amt);
    if (ret) {
        return ret;
    }
    ret = priority_queue_add(&engine->events, next, next_event);
    if (ret) {
        return ret;
    }
    struct Generator *iter_production = engine->model.production;
    uint32_t *iter_amt = amt;
    double *iter_cost = engine->model.unit_cost;
    for (uint8_t i = 0;
         i < 3;
         ++i,
         ++iter_production,
         ++iter_amt,
         ++iter_cost) {
        engine->model.stats.profit -= *iter_cost * *iter_amt;
        double cur = engine->time_now;
        for (uint32_t j = 0; j < *iter_amt; ++j) {
            ret = generator_generate_next(iter_production, &next);
            if (ret) {
                return ret;
            }
            cur += next;
            ret = event_production_initialize(&next_event,
                                              engine->model.stats.num_orders,
                                              i);
            if (ret) {
                return ret;
            }
            ret = priority_queue_add(&engine->events, cur, next_event);
            if (ret) {
                return ret;
            }
        }
    }
    ret = model_add_order(&engine->model,
                          idx,
                          amt,
                          engine->time_now +
                          engine->model.tolerance[idx]);
    if (ret) {
        return ret;
    }
    return ERR_OK;
}

int event_demand_printer(struct Event *event) {
    if (!event) {
        return ERR_INPUT_NULL;
    }
    printf("type: demand\nidx: %lu\namt: [%u, %u, %u]\n",
           event->data.demand.idx,
           event->data.demand.amt[0],
           event->data.demand.amt[1],
           event->data.demand.amt[2]);
    return ERR_OK;
}
