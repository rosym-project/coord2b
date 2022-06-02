#ifndef COORD2B_TYPES_EVENT_LOOP_H
#define COORD2B_TYPES_EVENT_LOOP_H

#ifdef __cplusplus
#include <stdbool.h>
extern "C" {
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
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