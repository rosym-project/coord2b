#include <assert.h>
#include "coord2b/functions/fsm.h"
#include "coord2b/functions/event_loop.h"

void fsm_step_nbx(struct fsm_nbx* fsm) {
    assert(fsm->eventData);
    assert(fsm->states);
    assert(fsm->reactions);
    assert(fsm->transitions);
    assert(fsm->numStates > 1);
    assert(fsm->numTransitions > 0);
    assert(fsm->startStateIndex >= 0 && fsm->startStateIndex < fsm->numStates);
    assert(fsm->endStateIndex >= 0 && fsm->endStateIndex < fsm->numStates);
    assert(fsm->currentStateIndex >= 0 && fsm->currentStateIndex < fsm->numStates);

    /* Exit if end state is reached */
    if (fsm->currentStateIndex == fsm->endStateIndex) return;

    /* Handle transition into next state */
    int transIndex;
    for (int i = 0; i < fsm->numReactions; i++) {
        // skip if event reaction not triggered
        if (!consume_event(fsm->eventData, fsm->reactions[i].conditionEventIndex)) continue;

        // transition, skip if current state doesn't match transition's start state
        transIndex = fsm->reactions[i].transitionIndex;
        assert(transIndex < fsm->numTransitions);
        if (fsm->currentStateIndex != fsm->transitions[transIndex].startStateIndex) continue;

        // set transition state
        fsm->currentStateIndex = fsm->transitions[transIndex].endStateIndex;

        // fire any requested events
        if (fsm->reactions[i].numFiredEvents > 0) {
            assert(fsm->reactions[i].firedEventIndices);
            for (int j = 0; j < fsm->reactions[i].numFiredEvents; j++) {
                produce_event(fsm->eventData, fsm->reactions[i].firedEventIndices[j]);
            }
        }

        /* The following will stop processing further reactions. This implies that the order of reactions
         * and reactions signifies the priority in which they're handled, and that only the first transition
         * will be taken into account.
         */
        break;
    }
}
