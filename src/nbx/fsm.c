#include <assert.h>
#include<stdio.h>
#include "coord2b/functions/fsm.h"
#include "coord2b/functions/event_loop.h"

void fsm_step_nbx(struct fsm_nbx* fsm) {
    assert(fsm->eventData);
    assert(fsm->states);
    assert(fsm->reactions);
    assert(fsm->behaviors);
    assert(fsm->transitions);
    assert(fsm->numStates > 1);
    assert(fsm->numTransitions > 0);
    assert(fsm->startStateIndex >= 0 && fsm->startStateIndex < fsm->numStates);
    assert(fsm->endStateIndex >= 0 && fsm->endStateIndex < fsm->numStates);
    assert(fsm->currentStateIndex >= 0 && fsm->currentStateIndex < fsm->numStates);

    /* Exit if end state is reached */
    if (fsm->currentStateIndex == fsm->endStateIndex) return;

    /* Handle current state behavior */
    for (int i = 0; i < fsm->numBehaviors; i++) {
        // skip if condition of behavior not met
        if (!fsm->behaviors[i].condition(fsm->eventData)) continue;

        // execute behavior
        fsm->behaviors[i].execute(fsm->data);
    }

    /* Handle transition into next state */
    int transIndex;
    for (int i = 0; i < fsm->numReactions; i++) {
        // skip if event reaction not triggered
        if (!fsm->reactions[i].condition(fsm->eventData)) continue;

        // transition
        assert(fsm->reactions[i].numTransitions > 0);
        assert(fsm->reactions[i].transitionIndices);
        for (int j = 0; j < fsm->reactions[i].numTransitions; j++) {
            transIndex = fsm->reactions[i].transitionIndices[j];
            assert(transIndex < fsm->numTransitions);

            // skip if current state doesn't match transition's start state
            if (fsm->currentStateIndex != fsm->transitions[transIndex].startStateIndex) continue;

            // transition
            fsm->currentStateIndex = fsm->transitions[transIndex].endStateIndex;

            // fire any requested events
            if (fsm->transitions[transIndex].numFiredEvents == 0) continue;
            assert(fsm->transitions[transIndex].firedEventIndices);
            for (int j = 0; j < fsm->transitions[transIndex].numFiredEvents; j++) {
                produce_event(fsm->eventData, fsm->transitions[transIndex].firedEventIndices[j]);
            }

            break;
        }
        // TODO: should checking reactions stop after finding the first valid one?
    }
}
