#ifndef COORD2B_TYPES_FSM_H
#define COORD2B_TYPES_FSM_H

#include "coord2b/types/event_loop.h"

#ifdef __cplusplus
extern "C" {
#endif

struct event_reaction {
    unsigned int numTransitions;

    unsigned int *transitionIndices;    // [numTransitions]

    /* function that check events and return whether the this reaction should take place
     * must use consume_event() function
     */
    _Bool (*condition) (struct events *);
};

struct event_behavior {
    /* function that check events and return whether the this reaction should take place
     * must use consume_event() function
     */
    _Bool (*condition) (struct events *);

    /* the behavior to be executed if compose_handled_event returns true */
    void (*execute) (void *);
};

struct transition {
    unsigned int startStateIndex;
    unsigned int endStateIndex;
    unsigned int numFiredEvents;
    unsigned int *firedEventIndices;    // [numFiredEvents]
};

struct state {
    char * name;
};

struct fsm_nbx {
    // configurations
    unsigned int numBehaviors;
    unsigned int numReactions;
    unsigned int numTransitions;
    unsigned int numStates;

    struct state * states;              // [numStates]
    unsigned int startStateIndex;
    unsigned int endStateIndex;
    unsigned int currentStateIndex;

    struct events * eventData;

    struct event_reaction * reactions;  // [numReactions]
    struct event_behavior * behaviors;  // [numBehaviors]
    struct transition * transitions;    // [numTransitions]
    void * data;
};

#ifdef __cplusplus
}
#endif

#endif  // COORD2B_TYPES_FSM_H