#ifndef EVENT_H__
#define EVENT_H__

#include <stdint.h>

#include "../engine.h"

struct Event {
    union {
        struct {
            uint64_t idx;
            uint32_t amt[3];
        } demand;
        struct {
            uint64_t id;
            uint8_t idx;
        } production;
        struct {
            uint64_t idx;
            uint32_t to;
        } movement;
        uint64_t rtb;
    } data;
    int (*handler)(struct Event*, struct Engine*);
    int (*printer)(struct Event*);
};

int event_handle(struct Event *event, struct Engine *engine);
int event_print(struct Event *event);
int event_demand_initialize(struct Event **event, uint64_t idx, uint32_t *amt);
int event_production_initialize(struct Event **event, uint64_t id, uint8_t idx);
int event_movement_initialize(struct Event **event, uint64_t idx, uint32_t to);
int event_delivery_initialize(struct Event **event);
int event_rtb_initialize(struct Event **event, uint64_t idx);

#endif
