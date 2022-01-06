#ifndef ENGINE_H__
#define ENGINE_H__

#include <stdint.h>

#include "../model/model.h"
#include "../util/generator/mt19937.h"
#include "../util/data_structure/priority_queue.h"

struct Engine {
    double time_now, time_end;
    struct MT19937 gen;
    struct Model model;
    struct PriorityQueue events;
};

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
                      double *production_rate);
int engine_run(struct Engine *engine,
               uint64_t iter,
               uint8_t verbosity,
               double *profit,
               uint64_t *num_orders,
               uint64_t *num_delivered);
int engine_finalize(struct Engine *engine);

#endif
