#ifndef COORD2B_TYPES_FSM_H
#define COORD2B_TYPES_FSM_H

#include "coord2b/types/event_loop.h"

#ifdef __cplusplus
extern "C" {
#endif

struct event_reaction {
    unsigned int numTransitions;

    /* reaction will occur if event with this index is fired */
    unsigned int conditionEventIndex;

    /* transitions that can occur if the above event is fired */
    unsigned int *transitionIndices;    // [numTransitions]
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
    unsigned int numReactions;
    unsigned int numTransitions;
    unsigned int numStates;

    struct state * states;              // [numStates]
    const unsigned int startStateIndex;
    const unsigned int endStateIndex;
    unsigned int currentStateIndex;

    struct events * eventData;

    struct event_reaction * reactions;  // [numReactions]
    struct transition * transitions;    // [numTransitions]
};

#ifdef __cplusplus
}
#endif

#endif  // COORD2B_TYPES_FSM_H