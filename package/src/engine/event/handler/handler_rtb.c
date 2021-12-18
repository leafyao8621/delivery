#include <stdio.h>
#include <stdlib.h>
#include "../handler.h"
#include "../event.h"
#include "../../engine.h"
#include "../../../util/errcode.h"

int event_rtb_handler(struct Event *event, struct Engine *engine) {
    if (!event || !engine) {
        return ERR_INPUT_NULL;
    }
    return ERR_OK;
}

int event_rtb_printer(struct Event *event) {
    if (!event) {
        return ERR_INPUT_NULL;
    }
    puts("type: rtb");
    return ERR_OK;
}
