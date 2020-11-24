#include <assert.h>
#include<stdio.h>
#include "coord2b/functions/fsm.h"

void produce_event(_Bool * events, int eventIndex) {
    events[eventIndex] = true;
}

_Bool consume_event(_Bool * events, int eventIndex) {
    _Bool retVal = events[eventIndex];
    events[eventIndex] = false;
    return  retVal;
}

void fsm_step_nbx(struct fsm_nbx* fsm) {
    assert(fsm->events);
    assert(fsm->states);
    assert(fsm->transitions);
    assert(fsm->numEvents >= 0);
    assert(fsm->numStates > 1);
    assert(fsm->numTransitions > 0);
    assert(fsm->startStateIndex >= 0 && fsm->startStateIndex < fsm->numStates);
    assert(fsm->endStateIndex >= 0 && fsm->endStateIndex < fsm->numStates);
    assert(fsm->currentStateIndex >= 0 && fsm->currentStateIndex < fsm->numStates);

    if (fsm->currentStateIndex == fsm->endStateIndex) return;

    for (int i = 0; i < fsm->states[fsm->currentStateIndex].numTransitions; i++) {
        int transitionIndex = fsm->states[fsm->currentStateIndex].transitionIndices[i];

        assert(transitionIndex >= 0 && transitionIndex < fsm->numTransitions);
        assert(fsm->currentStateIndex == fsm->transitions[transitionIndex].startStateIndex);

        struct transition trans = fsm->transitions[transitionIndex];
        assert(trans.compose_handled_event);

        // check events to see if transition should happen
        if (!(*trans.compose_handled_event)(fsm->events, fsm->numEvents)) continue;

        // perform associated behavior with given user data if one is specified
        if (trans.behavior) (*trans.behavior)(fsm->data);

        // set next state
        fsm->currentStateIndex = fsm->transitions[transitionIndex].endStateIndex;

        // produce requested events if there are any
        if (trans.numFiredEvents > 0) {
            assert(trans.numFiredEvents < fsm->numEvents);
            assert(trans.firedEventIndices);
            for (int j = 0; j < trans.numFiredEvents; j++) {
                assert(trans.firedEventIndices[j] >= 0 && trans.firedEventIndices[j] < fsm->numEvents);
                produce_event(fsm->events, trans.firedEventIndices[j]);
            }
        }
        // break out of loop since transition happened
        break;
    }
}
