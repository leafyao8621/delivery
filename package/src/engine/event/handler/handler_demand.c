#include <stdio.h>
#include <stdlib.h>
#include "../handler.h"
#include "../event.h"
#include "../../../util/errcode.h"

int event_demand_handler(struct Event *event, struct Engine *engine) {
    if (!event || !engine) {
        return ERR_INPUT_NULL;
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
