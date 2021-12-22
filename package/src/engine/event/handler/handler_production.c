#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../handler.h"
#include "../event.h"
#include "../../engine.h"
#include "../../../util/errcode.h"

int event_production_handler(struct Event *event, struct Engine *engine) {
    if (!event || !engine) {
        return ERR_INPUT_NULL;
    }
    if (engine->model.orders.front) {
        struct ListNode *iter = engine->model.orders.front;
        for (;
             iter &&
             ((struct Order*)(iter->data))->id != event->data.production.id;
             iter = iter->next);
        if (iter) {
            ++((struct Order*)(iter->data))->cur[event->data.production.idx];
            if (!memcmp(((struct Order*)(iter->data))->cur,
                        ((struct Order*)(iter->data))->amt,
                        sizeof(uint32_t) * 3)) {
                int ret = model_remove_order(&engine->model,
                                             event->data.production.id);
                if (ret) {
                    return ret;
                }
            }
        }

    }
    return ERR_OK;
}

int event_production_printer(struct Event *event) {
    if (!event) {
        return ERR_INPUT_NULL;
    }
    printf("type: production\nid: %lu\nidx: %hhu\n",
           event->data.production.id,
           event->data.production.idx);
    return ERR_OK;
}
