#include <stdio.h>
#include <stdlib.h>
#include "../handler.h"
#include "../event.h"
#include "../../engine.h"
#include "../../../util/errcode.h"

int event_delivery_handler(struct Event *event, struct Engine *engine) {
    if (!event || !engine) {
        return ERR_INPUT_NULL;
    }
    engine->model.stats.profit += event->data.delivery;
    return ERR_OK;
}

int event_delivery_printer(struct Event *event) {
    if (!event) {
        return ERR_INPUT_NULL;
    }
    puts("type: delivery");
    return ERR_OK;
}
