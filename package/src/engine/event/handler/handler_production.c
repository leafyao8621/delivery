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
            struct Order *order = (struct Order*)(iter->data);
            ++order->cur[event->data.production.idx];
            if (!memcmp(order->cur,
                        order->amt,
                        sizeof(uint32_t) * 3)) {
                order->fulfiled = 1;
                if (engine->model.parked_vehicles.front) {
                    struct Order data = *order;
                    int ret = model_remove_order(&engine->model,
                                                event->data.production.id);
                    if (ret) {
                        return ret;
                    }
                    struct GraphNode **path = 0;
                    uint64_t size = 0;
                    ret = graph_get_shortest(&engine->model.map,
                                            0,
                                            data.dest,
                                            &path,
                                            &size);
                    if (ret) {
                        return ret;
                    }
                    if (engine->time_now + path[size - 1]->distance <=
                        data.expiration) {
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
