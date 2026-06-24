#ifndef COORD2B_TYPES_EVENT_LOOP_H
#define COORD2B_TYPES_EVENT_LOOP_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct events {
    unsigned int numEvents;
    _Bool * currentEvents;  // [numEvents]
    _Bool * futureEvents;   // [numEvents]
};

#ifdef __cplusplus
}
#endif

#endif  // COORD2B_TYPES_EVENT_LOOP_H