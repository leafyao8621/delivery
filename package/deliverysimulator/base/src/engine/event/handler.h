#ifndef HANDLER_H__
#define HANDLER_H__

#include "event.h"
#include "../engine.h"

int event_demand_handler(struct Event *event, struct Engine *engine);
int event_demand_printer(struct Event *event);
int event_production_handler(struct Event *event, struct Engine *engine);
int event_production_printer(struct Event *event);
int event_movement_handler(struct Event *event, struct Engine *engine);
int event_movement_printer(struct Event *event);
int event_delivery_handler(struct Event *event, struct Engine *engine);
int event_delivery_printer(struct Event *event);
int event_rtb_handler(struct Event *event, struct Engine *engine);
int event_rtb_printer(struct Event *event);

#endif
