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
    int ret = model_park_vehicle(&engine->model, event->data.rtb);
    if (ret) {
        return ret;
    }
    struct ListNode *iter = engine->model.orders.front;
    struct Order *order = 0;
    struct GraphNode **path = 0;
    uint64_t size = 0;
    for (; iter;) {
        order = (struct Order*)iter->data;
        ret = graph_get_shortest(&engine->model.map,
                                 0,
                                 order->dest,
                                 &path,
                                 &size);
        if (engine->time_now + path[size - 1]->distance >
            order->expiration ||
            !order->fulfiled) {
            iter = iter->next;
            graph_path_finalize(path);
        } else {
            break;
        }
    }
    if (iter) {
        struct Order data = *order;
        int ret = model_remove_order(&engine->model,
                                     data.id);
        if (ret) {
            return ret;
        }
        uint64_t idx =
            ((struct Vehicle*)
             (engine->model.parked_vehicles.front->data))->idx;
        ret = model_launch_vehicle(&engine->model);
        if (ret) {
            return ret;
        }
        struct GraphNode **iter = path + 1;
        struct Event *next = 0;
        double cur = engine->time_now;
        for (uint64_t i = 1; i < size; ++i, ++iter) {
            ret = event_movement_initialize(&next,
                                            idx,
                                            (*iter)->idx);
            if (ret) {
                return ret;
            }
            cur += (*iter)->distance;
            ret = priority_queue_add(&engine->events,
                                    cur,
                                    next);
            if (ret) {
                return ret;
            }
        }
        ret = event_delivery_initialize(&next, data.revenue);
        if (ret) {
            return ret;
        }
        ret = priority_queue_add(&engine->events,
                                cur,
                                next);
        if (ret) {
            return ret;
        }
        iter -= 2;
        for (uint64_t i = 1; i < size; ++i, --iter) {
            ret = event_movement_initialize(&next,
                                            idx,
                                            (*iter)->idx);
            if (ret) {
                return ret;
            }
            cur += iter[1]->distance - (*iter)->distance;
            ret = priority_queue_add(&engine->events,
                                        cur,
                                        next);
            if (ret) {
                return ret;
            }
        }
        graph_path_finalize(path);
        ret = event_rtb_initialize(&next, idx);
        if (ret) {
            return ret;
        }
        ret = priority_queue_add(&engine->events, cur, next);
        if (ret) {
            return ret;
        }
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
