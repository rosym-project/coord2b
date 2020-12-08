#ifndef COORD2B_FUNCTIONS_EVENT_LOOP_H
#define COORD2B_FUNCTIONS_EVENT_LOOP_H
#include "coord2b/types/event_loop.h"

#ifdef __cplusplus
#include <stdbool.h>
extern "C" {
#endif

void produce_event(struct events * eventData, unsigned int eventIndex);
_Bool consume_event(struct events * eventData, unsigned int eventIndex);
void event_loop_step(struct events * eventData);

#ifdef __cplusplus
}
#endif

#endif // COORD2B_FUNCTIONS_EVENT_LOOP_H