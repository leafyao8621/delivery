#include <stdio.h>
#include <stdlib.h>
#include "../handler.h"
#include "../event.h"
#include "../../engine.h"
#include "../../../util/errcode.h"

int event_movement_handler(struct Event *event, struct Engine *engine) {
    if (!event || !engine) {
        return ERR_INPUT_NULL;
    }
    return ERR_OK;
}

int event_movement_printer(struct Event *event) {
    if (!event) {
        return ERR_INPUT_NULL;
    }
    printf("type: movement\nidx: %lu\nto: %hhu\n",
           event->data.movement.idx,
           event->data.movement.to);
    return ERR_OK;
}
