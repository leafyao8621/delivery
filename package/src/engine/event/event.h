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
    } data;
    int (*handler)(struct Event*, struct Engine*);
    int (*printer)(struct Event*);
};

int event_handle(struct Event *event, struct Engine *engine);
int event_print(struct Event *event);
int event_demand_initialize(struct Event **event, uint64_t idx, uint32_t *amt);

#endif
