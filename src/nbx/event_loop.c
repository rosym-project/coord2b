#include <assert.h>
#include "coord2b/functions/event_loop.h"

void produce_event(struct events * eventData, unsigned int eventIndex) {
    assert(eventData->futureEvents);
    assert(eventIndex < eventData->numEvents);
    eventData->futureEvents[eventIndex] = true;
}

_Bool consume_event(struct events * eventData, unsigned int eventIndex) {
    assert(eventData->currentEvents);
    assert(eventIndex < eventData->numEvents);
    return eventData->currentEvents[eventIndex];
}

void reconfig_event_buffers(struct events * eventData) {
    assert(eventData->futureEvents);
    assert(eventData->currentEvents);

    // swap pointers for current and future events
    _Bool * curEventsPtr = eventData->currentEvents;
    eventData->currentEvents = eventData->futureEvents;
    eventData->futureEvents = curEventsPtr;

    // reset future events
    for (int i = 0; i < eventData->numEvents; i++) {
        eventData->futureEvents[i] = false;
    }
}