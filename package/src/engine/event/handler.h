#ifndef HANDLER_H__
#define HANDLER_H__

#include "event.h"
#include "../engine.h"

int event_demand_handler(struct Event *event, struct Engine *engine);
int event_demand_printer(struct Event *event);

#endif
