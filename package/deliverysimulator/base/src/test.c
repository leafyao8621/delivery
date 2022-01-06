#include <stdio.h>
#include "util/errcode.h"
#include "engine/engine.h"

#define LOG_ERR printf("errcode: %d\nerrmsg: %s\n", ret, errcode_lookup[ret])

int main(void) {
    double unit_cost[3] = {
        3,
        4,
        5
    };
    double unit_revenue[3] = {
        4.5,
        5,
        7.5
    };
    double tolerance[5] = {
        25000,
        27000,
        26000,
        22000,
        24000
    };
    uint64_t from[7] = {
        0,
        0,
        1,
        1,
        1,
        2,
        3
    };
    uint64_t to[7] = {
        1,
        3,
        2,
        3,
        4,
        4,
        4
    };
    double distance[7] = {
        0.6,
        0.1,
        0.5,
        0.2,
        0.2,
        0.5,
        0.1
    };
    double rate[5] = {
        1,
        1,
        1,
        1,
        1
    };
    uint32_t upper_limit[15] = {
        4, 2, 3,
        4, 2, 3,
        4, 2, 3,
        4, 2, 3,
        4, 2, 3
    };
    double production_rate[3] = {
        10, 20, 30
    };
    struct Engine engine;
    int ret = engine_initialize(&engine,
                                3600,
                                100000,
                                3,
                                10000,
                                unit_cost,
                                unit_revenue,
                                5,
                                tolerance,
                                from,
                                to,
                                distance,
                                7,
                                rate,
                                upper_limit,
                                production_rate);
    LOG_ERR;
    double profit[10];
    uint64_t num_orders[10];
    uint64_t num_delivered[10];
    ret = engine_run(&engine, 10, 1, profit, num_orders, num_delivered);
    LOG_ERR;
    ret = engine_finalize(&engine);
    LOG_ERR;
    for (int i = 0; i < 10; ++i) {
        printf("iteration: %d\n"
               "profit: %lf\n"
               "num_orders: %lu\n"
               "num_delivered: %lu\n",
               i,
               profit[i],
               num_orders[i],
               num_delivered[i]);
    }
    return 0;
}
