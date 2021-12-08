#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model.h"
#include "../util/errcode.h"

struct Vehicle {
    uint64_t idx, location;
};

int model_initialize(struct Model *model,
                     struct MT19937 *gen,
                     uint64_t num_vehicles,
                     uint64_t size,
                     uint64_t *from,
                     uint64_t *to,
                     double *distance,
                     uint64_t edge,
                     double *rate,
                     uint32_t *upper_limit,
                     double *production_rate) {
    if (!model ||
        !from ||
        !to ||
        !distance ||
        !rate ||
        !upper_limit ||
        !production_rate) {
        return ERR_INPUT_NULL;
    }
    model->gen = gen;
    list_initialize(&model->orders);
    list_initialize(&model->parked_vehicles);
    model->num_vehicles = num_vehicles;
    int ret = graph_initialize(&model->map,
                               size,
                               from,
                               to,
                               distance,
                               edge);
    if (ret) {
        return ret;
    }
    model->demand = malloc(sizeof(struct Generator) * size);
    if (!model->demand) {
        graph_finalize(&model->map);
        return ERR_OUT_OF_MEM;
    }
    struct Generator *iter_demand = model->demand;
    double *iter_rate = rate;
    uint32_t *iter_upper_limit = upper_limit;
    for (uint64_t i = 0; i < size; ++i, ++iter_demand, ++iter_rate) {
        iter_demand->gen = model->gen;
        iter_demand->rate = *iter_rate;
        uint32_t *iter_demand_upper_limit = iter_demand->upper_limit;
        for (uint64_t j = 0;
             j < 3;
             ++j,
             ++iter_upper_limit,
             ++iter_demand_upper_limit) {
            *iter_demand_upper_limit = *iter_upper_limit;
        }
    }
    struct Generator *iter_production = model->production;
    double *iter_production_rate = production_rate;
    for (uint64_t i = 0;
         i < 3;
         ++i,
         ++iter_production,
         ++iter_production_rate) {
        iter_production->rate = *iter_production_rate;
    }
    model->vehicles = malloc(sizeof(struct Vehicle) * num_vehicles);
    if (!model->vehicles) {
        free(model->demand);
        graph_finalize(&model->map);
        return ERR_OUT_OF_MEM;
    }
    struct Vehicle *iter_vehicle = model->vehicles;
    for (uint64_t i = 0; i < num_vehicles; ++i, ++iter_vehicle) {
        iter_vehicle->idx = i;
        iter_vehicle->location = 0;
        ret = list_push_back(&model->parked_vehicles, iter_vehicle);
        if (ret) {
            free(model->demand);
            graph_finalize(&model->map);
            free(model->vehicles);
            return ret;
        }
    }
    return ERR_OK;
}

static void print_vehicle(void *data) {
    printf("idx: %lu\nlocation: %lu\n",
            ((struct Vehicle*)data)->idx,
            ((struct Vehicle*)data)->location);
}

int model_print(struct Model *model) {
    if (!model) {
        return ERR_INPUT_NULL;
    }
    puts("Model:");
    puts("Vehicles:");
    struct Vehicle *iter_vehicle = model->vehicles;
    for (uint64_t i = 0; i < model->num_vehicles; ++i, ++iter_vehicle) {
        printf("idx: %lu\nlocation: %lu\n",
               iter_vehicle->idx,
               iter_vehicle->location);
    }
    puts("Parked Vehicles:");
    int ret = list_print(&model->parked_vehicles, print_vehicle);
    if (ret) {
        return ret;
    }
    puts("Map:");
    ret = graph_print(&model->map);
    if (ret) {
        return ret;
    }
    puts("Demand:");
    struct Generator *iter_demand = model->demand;
    for (uint64_t i = 0; i < model->map.size; ++i, ++iter_demand) {
        printf("idx: %lu\nrate: %lf\nupper_limit: [%u, %u, %u]\n",
               i,
               iter_demand->rate,
               iter_demand->upper_limit[0],
               iter_demand->upper_limit[1],
               iter_demand->upper_limit[2]);
    }
    struct Generator *iter_production = model->production;
    for (uint64_t i = 0; i < 3; ++i, ++iter_production) {
        printf("idx: %lu\nrate: %lf\n",
               i,
               iter_production->rate);
    }
    return ERR_OK;
}

int model_finalize(struct Model *model) {
    if (!model) {
        return ERR_INPUT_NULL;
    }
    list_finalize(&model->orders, 1);
    list_finalize(&model->parked_vehicles, 0);
    graph_finalize(&model->map);
    free(model->demand);
    free(model->vehicles);
    return ERR_OK;
}
