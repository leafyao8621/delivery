#include <stdio.h>
#include "util/errcode.h"
#include "engine/engine.h"

#define LOG_ERR printf("errcode: %d\nerrmsg: %s\n", ret, errcode_lookup[ret])

int main(void) {
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
        6,
        1,
        5,
        2,
        2,
        5,
        1
    };
    double rate[7] = {
        1,
        1,
        1,
        1,
        1,
        1,
        1
    };
    uint32_t upper_limit[21] = {
        1, 2, 3,
        1, 2, 3,
        1, 2, 3,
        1, 2, 3,
        1, 2, 3,
        1, 2, 3,
        1, 2, 3
    };
    double production_rate[3] = {
        2, 4, 6
    };
    struct Engine engine;
    int ret = engine_initialize(&engine,
                                3600000,
                                1000,
                                3,
                                5,
                                from,
                                to,
                                distance,
                                7,
                                rate,
                                upper_limit,
                                production_rate);
    LOG_ERR;
    ret = model_print(&engine.model);
    LOG_ERR;
    ret = engine_finalize(&engine);
    LOG_ERR;
    return 0;
}