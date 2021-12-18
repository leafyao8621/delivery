#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "handler.h"
#include "../../util/errcode.h"

int event_handle(struct Event *event, struct Engine *engine) {
    if (!event || !engine) {
        return ERR_INPUT_NULL;
    }
    engine->time_now = engine->events.heap->key;
    int ret = event->handler(event, engine);
    if (ret) {
        return ret;
    }
    ret = priority_queue_remove(&engine->events, 1);
    return ret;
}

int event_print(struct Event *event) {
    if (!event) {
        return ERR_INPUT_NULL;
    }
    return event->printer(event);
}

int event_demand_initialize(struct Event **event, uint64_t idx, uint32_t *amt) {
    *event = malloc(sizeof(struct Event));
    if (!event) {
        return ERR_OUT_OF_MEM;
    }
    (*event)->data.demand.idx = idx;
    memcpy((*event)->data.demand.amt, amt, sizeof(uint32_t) * 3);
    (*event)->handler = event_demand_handler;
    (*event)->printer = event_demand_printer;
    return ERR_OK;
}

int event_production_initialize(struct Event **event, uint8_t idx) {
    *event = malloc(sizeof(struct Event));
    if (!event) {
        return ERR_OUT_OF_MEM;
    }
    (*event)->data.production = idx;
    (*event)->handler = event_production_handler;
    (*event)->printer = event_production_printer;
    return ERR_OK;
}

int event_movement_initialize(struct Event **event, uint64_t idx, uint32_t to) {
    *event = malloc(sizeof(struct Event));
    if (!event) {
        return ERR_OUT_OF_MEM;
    }
    (*event)->data.movement.idx = idx;
    (*event)->data.movement.to = to;
    (*event)->handler = event_movement_handler;
    (*event)->printer = event_movement_printer;
    return ERR_OK;
}

int event_delivery_initialize(struct Event **event) {
    *event = malloc(sizeof(struct Event));
    if (!event) {
        return ERR_OUT_OF_MEM;
    }
    (*event)->handler = event_delivery_handler;
    (*event)->printer = event_delivery_printer;
    return ERR_OK;
}

int event_rtb_initialize(struct Event **event, uint64_t idx) {
    *event = malloc(sizeof(struct Event));
    if (!event) {
        return ERR_OUT_OF_MEM;
    }
    (*event)->data.rtb = idx;
    (*event)->handler = event_rtb_handler;
    (*event)->printer = event_rtb_printer;
    return ERR_OK;
}
