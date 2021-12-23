#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model.h"
#include "../util/errcode.h"

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
                     double *production_rate) {
    if (!model ||
        !unit_cost ||
        !unit_revenue ||
        !tolerance ||
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
    model->vehicle_cost = vehicle_cost;
    model->stats.profit = num_vehicles * (-vehicle_cost);
    model->stats.num_orders = 0;
    model->stats.num_delivered = 0;
    model->stats.num_dropped = 0;
    model->stats.lead_time = 0;
    memcpy(model->unit_cost, unit_cost, sizeof(double) * 3);
    memcpy(model->unit_revenue, unit_revenue, sizeof(double) * 3);
    memset(model->last_production, 0, sizeof(double) * 3);
    int ret = graph_initialize(&model->map,
                               size,
                               from,
                               to,
                               distance,
                               edge);
    if (ret) {
        return ret;
    }

    model->tolerance = malloc(sizeof(double) * size);
    if (!model->tolerance) {
        graph_finalize(&model->map);
        return ERR_OUT_OF_MEM;
    }
    memcpy(model->tolerance, tolerance, sizeof(double) * size);

    model->demand = malloc(sizeof(struct Generator) * size);
    if (!model->demand) {
        graph_finalize(&model->map);
        free(model->tolerance);
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
        iter_production->gen = model->gen;
        iter_production->rate = *iter_production_rate;
    }
    model->vehicles = malloc(sizeof(struct Vehicle) * num_vehicles);
    if (!model->vehicles) {
        free(model->demand);
        free(model->tolerance);
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
            free(model->tolerance);
            return ret;
        }
    }
    return ERR_OK;
}

int model_add_order(struct Model *model,
                    uint64_t dest,
                    uint32_t *amt,
                    double expiration) {
    if (!model || !amt) {
        return ERR_INPUT_NULL;
    }
    struct Order *order = malloc(sizeof(struct Order));
    if (!order) {
        return ERR_OUT_OF_MEM;
    }
    order->id = model->stats.num_orders++;
    order->dest = dest;
    order->fulfiled = 0;
    order->revenue = 0;
    memcpy(order->amt, amt, sizeof(uint32_t) * 3);
    memset(order->cur, 0, sizeof(uint32_t) * 3);
    double *iter_unit_cost = model->unit_cost;
    double *iter_unit_revenue = model->unit_revenue;
    uint32_t *iter_amt = amt;
    for (uint8_t i = 0; i < 3; ++i, ++iter_unit_cost, ++iter_amt) {
        model->stats.profit -= *iter_unit_cost * *iter_amt;
        order->revenue += *iter_unit_revenue * *iter_amt;
    }
    order->expiration = expiration;
    int ret = list_push_back(&model->orders, order);
    if (ret) {
        return ret;
    }
    return ERR_OK;
}

int model_remove_order(struct Model *model, uint64_t id) {
    if (!model) {
        return ERR_INPUT_NULL;
    }
    struct ListNode *iter = model->orders.front;
    for (;
         iter &&
         ((struct Order*)(iter->data))->id != id;
         iter = iter->next);
    if (iter) {
        free(iter->data);
        if (iter == model->orders.front) {
            list_pop_front(&model->orders);
        } else if (iter == model->orders.back) {
            list_pop_back(&model->orders);
        } else {
            iter->prev->next = iter->next;
            iter->next->prev = iter->prev;
            free(iter);
        }
    }

    return ERR_OK;
}

int model_launch_vehicle(struct Model *model) {
    if (!model) {
        return ERR_INPUT_NULL;
    }
    int ret = list_pop_front(&model->parked_vehicles);
    if (ret) {
        return ret;
    }
    return ERR_OK;
}

int model_park_vehicle(struct Model *model, uint64_t idx) {
    if (!model) {
        return ERR_INPUT_NULL;
    }
    if (idx >= model->num_vehicles) {
        return ERR_MODEL_PARK_VEHICLE_IDX;
    }
    int ret = list_push_back(&model->parked_vehicles, model->vehicles + idx);
    if (ret) {
        return ret;
    }
    return ERR_OK;
}

static void print_vehicle(void *data) {
    printf("idx: %lu\nlocation: %lu\n",
            ((struct Vehicle*)data)->idx,
            ((struct Vehicle*)data)->location);
}

static void print_order(void *data) {
    struct Order *order = (struct Order*)data;
    printf("id: %lu\n"
           "dest: %lu\n"
           "amt: [%u, %u, %u]\n"
           "revenue: %lf\n"
           "cur: [%u, %u, %u]\n"
           "expiration: %lf\n",
           order->id,
           order->dest,
           order->amt[0],
           order->amt[1],
           order->amt[2],
           order->revenue,
           order->cur[0],
           order->cur[1],
           order->cur[2],
           order->expiration);
}

int model_print(struct Model *model) {
    if (!model) {
        return ERR_INPUT_NULL;
    }
    puts("Model:");
    puts("Stats:");
    printf("profit: %lf\nnum_delivered: %lu\nnum_dropped: %lu\nlead_time: %lf\n",
           model->stats.profit,
           model->stats.num_delivered,
           model->stats.num_dropped,
           model->stats.lead_time);
    printf("Vehicle Cost: %lf\n", model->vehicle_cost);
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
    puts("Orders:");
    ret = list_print(&model->orders, print_order);
    if (ret) {
        return ret;
    }
    puts("Map:");
    ret = graph_print(&model->map);
    if (ret) {
        return ret;
    }
    puts("Unit Cost/Revenue:");
    double *iter_unit_cost = model->unit_cost;
    double *iter_unit_revenue = model->unit_revenue;
    for (uint64_t i = 0; i < 3; ++i, ++iter_unit_cost, ++iter_unit_revenue) {
        printf("idx: %lu\ncost: %lf\nrevenue: %lf\n",
               i,
               *iter_unit_cost,
               *iter_unit_revenue);
    }
    puts("Tolerance:");
    double *iter_tolerance = model->tolerance;
    for (uint64_t i = 0; i < model->map.size; ++i, ++iter_tolerance) {
        printf("idx: %lu\ntolerance: %lf\n",
               i,
               *iter_tolerance);
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

int model_reset(struct Model *model) {
    if (!model) {
        return ERR_INPUT_NULL;
    }
    for (; model->orders.front; list_pop_front(&model->orders)) {
        free(model->orders.front->data);
    }
    for (;
         model->parked_vehicles.front;
         list_pop_front(&model->parked_vehicles)) {
        free(model->parked_vehicles.front->data);
    }
    int ret = ERR_OK;
    struct Vehicle *iter_vehicle = model->vehicles;
    for (uint64_t i = 0; i < model->num_vehicles; ++i, ++iter_vehicle) {
        iter_vehicle->idx = i;
        iter_vehicle->location = 0;
        ret = list_push_back(&model->parked_vehicles, iter_vehicle);
        if (ret) {
            return ret;
        }
    }
    memset(model->last_production, 0, sizeof(double) * 3);
    model->stats.profit = model->num_vehicles * (-model->vehicle_cost);
    model->stats.num_orders = 0;
    model->stats.num_delivered = 0;
    model->stats.num_dropped = 0;
    model->stats.lead_time = 0;
    return ERR_OK;
}

int model_finalize(struct Model *model) {
    if (!model) {
        return ERR_INPUT_NULL;
    }
    list_finalize(&model->orders, 1);
    list_finalize(&model->parked_vehicles, 0);
    graph_finalize(&model->map);
    free(model->tolerance);
    free(model->demand);
    free(model->vehicles);
    return ERR_OK;
}
