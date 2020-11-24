#ifndef COORD2B_TYPES_FSM_H
#define COORD2B_TYPES_FSM_H

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

struct transition {
    int startStateIndex;
    int endStateIndex;
    int numFiredEvents;

    /* function that check events and return whether the this reaction should take place */
    _Bool (*compose_handled_event) (_Bool * events, int numEvents);
    /* the behavior to be executed if compose_handled_event returns true */
    void (*behavior) (void *);
    /* indices of the events to set to true after the behavior is executed */
    int * firedEventIndices;     // [numFiredEvents]
};

struct state {
    int numTransitions;
    int * transitionIndices;     // [numTransitions]
    char * name;
};

struct fsm_nbx {
    // configurations
    int numEvents;
    int numTransitions;
    int numStates;
    int startStateIndex;
    int endStateIndex;

    int currentStateIndex;

    _Bool * events;                     // [numEvents]
    struct transition * transitions;    // [numTransitions]
    struct state * states;              // [numStates]
    void * data;
};

#ifdef __cplusplus
}
#endif

#endif  // COORD2B_TYPES_FSM_H