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
    _Bool transitionFound = false;
    for (int i = 0; i < fsm->numReactions; i++) {
        // skip if event reaction not triggered
        if (!consume_event(fsm->eventData, fsm->reactions[i].conditionEventIndex)) continue;

        // transition
        assert(fsm->reactions[i].numTransitions > 0);
        assert(fsm->reactions[i].transitionIndices);
        for (int j = 0; j < fsm->reactions[i].numTransitions; j++) {
            transIndex = fsm->reactions[i].transitionIndices[j];
            assert(transIndex < fsm->numTransitions);

            // skip if current state doesn't match transition's start state
            if (fsm->currentStateIndex != fsm->transitions[transIndex].startStateIndex) continue;

            // transition
            transitionFound = true;
            fsm->currentStateIndex = fsm->transitions[transIndex].endStateIndex;

            // fire any requested events
            if (fsm->transitions[transIndex].numFiredEvents == 0) break;
            assert(fsm->transitions[transIndex].firedEventIndices);
            for (int j = 0; j < fsm->transitions[transIndex].numFiredEvents; j++) {
                produce_event(fsm->eventData, fsm->transitions[transIndex].firedEventIndices[j]);
            }

            break;
        }

        /* The following will stop processing further reactions. This implies that the order of reactions
         * and reactions signifies the priority in which they're handled, and that only the first transition
         * will be taken into account.
         */
        if (transitionFound) break;
    }
}
