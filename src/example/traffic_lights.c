#include <stdio.h>
#include <time.h>
#include "coord2b/functions/fsm.h"

const double GLOBAL_TIMEOUT_SECS = 10.;

enum e_events {
    E_RED_COMPLETED = 0,
    E_YELLOW_COMPLETED,
    E_GREEN_COMPLETED,
    E_GLOBAL_TIMEOUT
};
const int NUM_EVENTS = E_GLOBAL_TIMEOUT - E_RED_COMPLETED + 1;

enum e_states {
    STATE_START=0,
    STATE_RED,
    STATE_YELLOW,
    STATE_GREEN,
    STATE_EXIT
};
const int NUM_STATES = STATE_EXIT - STATE_START + 1;

enum e_transitions {
    T_START_RED=0,
    T_RED_EXIT,
    T_RED_YELLOW,
    T_YELLOW_EXIT,
    T_YELLOW_GREEN,
    T_YELLOW_RED,
    T_GREEN_EXIT,
    T_GREEN_YELLOW
};
const int NUM_TRANSITIONS = T_GREEN_YELLOW - T_START_RED + 1;

_Bool compose_events_always_true(_Bool events[], int numEvents) { return true; }

_Bool compose_events_global_timer(_Bool events[], int numEvents) { return consume_event(events, E_GLOBAL_TIMEOUT); }

_Bool compose_events_red_completed(_Bool events[], int numEvents) { return consume_event(events, E_RED_COMPLETED); }

_Bool compose_events_green_completed(_Bool events[], int numEvents) {
    return consume_event(events, E_GREEN_COMPLETED);
}

struct user_data {
    int number;
};

void generic_behavior(struct user_data * userData) {
    printf("got user data: %d\n", userData->number);
    userData->number++;
    clock_t start = clock();
    // spins for 1 second
    while (clock() - start < 1 * CLOCKS_PER_SEC) {}
}

int main() {
    _Bool events[NUM_EVENTS];
    // initialize
    for (int i = 0; i < NUM_EVENTS; i++) events[i] = false;

    struct transition transitions[] = {
        [T_START_RED] = {
            .startStateIndex = STATE_START,
            .endStateIndex = STATE_RED,
            .compose_handled_event = &compose_events_always_true,
            .numFiredEvents = 0,
            .behavior = (void (*) (void *)) &generic_behavior
        },

        [T_RED_EXIT] = {
            .startStateIndex = STATE_RED,
            .endStateIndex = STATE_EXIT,
            .compose_handled_event = &compose_events_global_timer,
            .numFiredEvents = 1,
            .firedEventIndices = (int[]) { E_RED_COMPLETED },
            .behavior = (void (*) (void *)) &generic_behavior
        },

        [T_RED_YELLOW] = {
            .startStateIndex = STATE_RED,
            .endStateIndex = STATE_YELLOW,
            .numFiredEvents = 1,
            .compose_handled_event = &compose_events_always_true,
            .firedEventIndices = (int[]) {E_RED_COMPLETED},
            .behavior = (void (*) (void *)) &generic_behavior
        },

        [T_YELLOW_EXIT] = {
            .startStateIndex = STATE_YELLOW,
            .endStateIndex = STATE_EXIT,
            .compose_handled_event = &compose_events_global_timer,
            .numFiredEvents = 1,
            .firedEventIndices = (int[]) {E_YELLOW_COMPLETED},
            .behavior = (void (*) (void *)) &generic_behavior
        },

        [T_YELLOW_GREEN] = {
            .startStateIndex = STATE_YELLOW,
            .endStateIndex = STATE_GREEN,
            .compose_handled_event = &compose_events_red_completed,
            .numFiredEvents = 1,
            .firedEventIndices = (int[]) {E_YELLOW_COMPLETED},
            .behavior = (void (*) (void *)) &generic_behavior
        },

        [T_YELLOW_RED] = {
            .startStateIndex = STATE_YELLOW,
            .endStateIndex = STATE_RED,
            .compose_handled_event = &compose_events_green_completed,
            .numFiredEvents = 1,
            .firedEventIndices = (int[]) {E_YELLOW_COMPLETED},
            .behavior = (void (*) (void *)) &generic_behavior
        },

        [T_GREEN_EXIT] = {
            .startStateIndex = STATE_GREEN,
            .endStateIndex = STATE_EXIT,
            .compose_handled_event = &compose_events_global_timer,
            .numFiredEvents = 1,
            .firedEventIndices = (int[]) {E_GREEN_COMPLETED},
            .behavior = (void (*) (void *)) &generic_behavior
        },

        [T_GREEN_YELLOW] = {
            .startStateIndex = STATE_GREEN,
            .endStateIndex = STATE_YELLOW,
            .compose_handled_event = &compose_events_always_true,
            .numFiredEvents = 1,
            .firedEventIndices = (int[]) {E_GREEN_COMPLETED},
            .behavior = (void (*) (void *)) &generic_behavior
        }
    };

    struct state states[] = {
            [STATE_START] = { .name = "START", .numTransitions = 1, .transitionIndices = (int[]) {T_START_RED} },
            [STATE_RED] = { .name = "RED", .numTransitions = 2, .transitionIndices = (int[]) {T_RED_EXIT, T_RED_YELLOW} },
            [STATE_YELLOW] = {
                .name = "YELLOW", .numTransitions = 3,
                .transitionIndices = (int[]) {T_YELLOW_EXIT, T_YELLOW_GREEN, T_YELLOW_RED}
            },
            [STATE_GREEN] = {
                .name = "GREEN", .numTransitions = 2,
                .transitionIndices = (int[]) {T_GREEN_EXIT, T_GREEN_YELLOW}
            },
            [STATE_EXIT] = { .name = "EXIT", .numTransitions = 0 }
    };

    struct user_data data = {.number = 0};

    struct fsm_nbx fsm =  {
        .numEvents = NUM_EVENTS,
        .numTransitions = NUM_TRANSITIONS,
        .numStates = NUM_STATES,
        .startStateIndex = STATE_START,
        .endStateIndex = STATE_EXIT,

        .events = events,
        .transitions = transitions,
        .states = states,
        .data = &data
    };

    printf("Starting traffic light example\n");
    clock_t start = clock();
    double elapsedSecs;
    while (true) {
        printf("Current state: %s\n", fsm.states[fsm.currentStateIndex].name);
        if (fsm.currentStateIndex == STATE_EXIT) {
            printf("State machine completed successfully\n");
            break;
        }
        elapsedSecs = ((double) (clock() - start)) / CLOCKS_PER_SEC;
        if (elapsedSecs > GLOBAL_TIMEOUT_SECS) produce_event(events, E_GLOBAL_TIMEOUT);
        fsm_step_nbx(&fsm);
    }

    return 0;
}
