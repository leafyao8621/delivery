#ifndef MODEL_H__
#define MODEL_H__

#include <stdint.h>

#include "../util/data_structure/list.h"
#include "../util/data_structure/graph.h"
#include "../util/generator/generator.h"

struct Model {
    struct MT19937 *gen;
    uint64_t num_vehicles;
    double vehicle_cost, unit_cost[3], unit_revenue[3], *tolerance;
    struct Vehicle *vehicles;
    struct List orders, parked_vehicles;
    struct Graph map;
    struct Generator *demand, production[3];
    struct {
        double profit;
        uint64_t num_delivered;
        uint64_t num_dropped;
        double lead_time;
    } stats;
};

int model_initialize(struct Model *model,
                     struct MT19937 *gen,
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
int model_add_order(struct Model *model,
                    uint64_t dest,
                    uint32_t *amt,
                    double expiration);
int model_print(struct Model *model);
int model_finalize(struct Model *model);

#endif
