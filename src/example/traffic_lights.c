#include <stdio.h>
#include <time.h>
#include "coord2b/functions/fsm.h"
#include "coord2b/functions/event_loop.h"

const double GLOBAL_TIMEOUT_SECS = 15.;
const double SINGLE_LIGHT_TIMEOUT_SECS = 0.6;

enum e_events {
    E_STEP = 0,
    E_LIGHT_CHANGED,
    E_SINGLE_LIGHT_TIMEOUT,
    E_GLOBAL_TIMEOUT,
    NUM_EVENTS
};

enum e_states {
    S_START = 0,
    S_RED,
    S_RED_YELLOW,
    S_GREEN,
    S_GREEN_YELLOW,
    S_EXIT,
    NUM_STATES
};

enum e_transitions {
    T_START_RED = 0,
    T_RED_EXIT,
    T_RED_RED,
    T_RED_YELLOW,
    T_RED_YELLOW_EXIT,
    T_RED_YELLOW_YELLOW,
    T_RED_YELLOW_GREEN,
    T_GREEN_EXIT,
    T_GREEN_GREEN,
    T_GREEN_YELLOW,
    T_GREEN_YELLOW_EXIT,
    T_GREEN_YELLOW_YELLOW,
    T_GREEN_YELLOW_RED,
    NUM_TRANSITIONS
};

enum e_reactions {
    R_GLOBAL_TIMER_RED = 0,
    R_GLOBAL_TIMER_RED_YELLOW,
    R_GLOBAL_TIMER_GREEN,
    R_GLOBAL_TIMER_GREEN_YELLOW,
    R_SINGLE_LIGHT_TIMEOUT_RED,
    R_SINGLE_LIGHT_TIMEOUT_RED_YELLOW,
    R_SINGLE_LIGHT_TIMEOUT_GREEN,
    R_SINGLE_LIGHT_TIMEOUT_GREEN_YELLOW,
    R_ALWAYS_TRUE_START,
    R_ALWAYS_TRUE_RED,
    R_ALWAYS_TRUE_RED_YELLOW,
    R_ALWAYS_TRUE_GREEN,
    R_ALWAYS_TRUE_GREEN_YELLOW,
    NUM_REACTIONS
};

struct user_data {
    _Bool redOn;
    _Bool yellowOn;
    _Bool greenOn;
};

void generic_behavior(struct user_data * userData) {
    const char *redStr = (userData->redOn) ? "x" : " ";
    const char *yellowStr = (userData->yellowOn) ? "x" : " ";
    const char *greenStr = (userData->greenOn) ? "x" : " ";
    printf("lights: r=[%s] y=[%s] g=[%s]\n", redStr, yellowStr, greenStr);
}

void red_behavior(struct user_data * userData) {
    userData->redOn = true;
    userData->yellowOn = false;
    userData->greenOn = false;
}

void red_yel_behavior(struct user_data * userData) {
    userData->redOn = true;
    userData->yellowOn = true;
    userData->greenOn = false;
}

void green_behavior(struct user_data * userData) {
    userData->redOn = false;
    userData->yellowOn = false;
    userData->greenOn = true;
}

void green_yel_behavior(struct user_data * userData) {
    userData->redOn = false;
    userData->yellowOn = true;
    userData->greenOn = true;
}

void fsm_behavior(struct fsm_nbx *fsm, struct user_data * userData) {
    switch (fsm->currentStateIndex) {
        case S_START:
        case S_EXIT:
            break;
        case S_RED:
            red_behavior(userData);
            break;
        case S_RED_YELLOW:
            red_yel_behavior(userData);
            break;
        case S_GREEN:
            green_behavior(userData);
            break;
        case S_GREEN_YELLOW:
            green_yel_behavior(userData);
            break;
        default:
            printf("Unrecognized current state: %d\n", fsm->currentStateIndex);
            break;
    }

    if (consume_event(fsm->eventData, E_LIGHT_CHANGED)) {
        generic_behavior(userData);
    }
}

int main() {

    struct events eventData = {
        .numEvents = NUM_EVENTS,
        .currentEvents = (_Bool[NUM_EVENTS]) {false},
        .futureEvents = (_Bool[NUM_EVENTS]) {false}
    };

    struct state states[NUM_STATES] = {
        [S_START] = {.name = "Start"}, [S_RED] = {.name = "Red"}, [S_RED_YELLOW] = {.name = "Yellow From Red"},
        [S_GREEN] = {.name = "Green"}, [S_GREEN_YELLOW] = {.name = "Yellow From Green"}, [S_EXIT] = {.name = "Exit"}
    };

    struct user_data userData = { .greenOn = false, .yellowOn = false, .redOn = false };

    struct transition transitions[NUM_TRANSITIONS] = {
        [T_START_RED] = { .startStateIndex = S_START, .endStateIndex = S_RED },
        [T_RED_EXIT] = { .startStateIndex = S_RED, .endStateIndex = S_EXIT },
        [T_RED_RED]  = { .startStateIndex = S_RED, .endStateIndex = S_RED },
        [T_RED_YELLOW] = { .startStateIndex = S_RED, .endStateIndex = S_RED_YELLOW },
        [T_RED_YELLOW_EXIT] = { .startStateIndex = S_RED_YELLOW, .endStateIndex = S_EXIT },
        [T_RED_YELLOW_YELLOW] = { .startStateIndex = S_RED_YELLOW, .endStateIndex = S_RED_YELLOW },
        [T_RED_YELLOW_GREEN] = { .startStateIndex = S_RED_YELLOW, .endStateIndex = S_GREEN },
        [T_GREEN_EXIT]  = { .startStateIndex = S_GREEN, .endStateIndex = S_EXIT },
        [T_GREEN_GREEN] = { .startStateIndex = S_GREEN, .endStateIndex = S_GREEN },
        [T_GREEN_YELLOW] = { .startStateIndex = S_GREEN, .endStateIndex = S_GREEN_YELLOW },
        [T_GREEN_YELLOW_EXIT] = { .startStateIndex = S_GREEN_YELLOW, .endStateIndex = S_EXIT },
        [T_GREEN_YELLOW_YELLOW] = { .startStateIndex = S_GREEN_YELLOW, .endStateIndex = S_GREEN_YELLOW },
        [T_GREEN_YELLOW_RED] = { .startStateIndex = S_GREEN_YELLOW, .endStateIndex = S_RED }
    };

    struct event_reaction reactions[NUM_REACTIONS] = {
        [R_GLOBAL_TIMER_RED] = {
            .conditionEventIndex = E_GLOBAL_TIMEOUT, .numFiredEvents = 0, .transitionIndex = T_RED_EXIT
        },
        [R_GLOBAL_TIMER_RED_YELLOW] = {
            .conditionEventIndex = E_GLOBAL_TIMEOUT, .numFiredEvents = 0, .transitionIndex = T_RED_YELLOW_EXIT
        },
        [R_GLOBAL_TIMER_GREEN] = {
            .conditionEventIndex = E_GLOBAL_TIMEOUT, .numFiredEvents = 0, .transitionIndex = T_GREEN_EXIT
        },
        [R_GLOBAL_TIMER_GREEN_YELLOW] = {
            .conditionEventIndex = E_GLOBAL_TIMEOUT, .numFiredEvents = 0, .transitionIndex = T_GREEN_YELLOW_EXIT
        },
        [R_SINGLE_LIGHT_TIMEOUT_RED] = {
            .conditionEventIndex = E_SINGLE_LIGHT_TIMEOUT, .transitionIndex = T_RED_YELLOW,
            .numFiredEvents = 1, .firedEventIndices = (unsigned int[]) { E_LIGHT_CHANGED }
        },
        [R_SINGLE_LIGHT_TIMEOUT_RED_YELLOW] = {
            .conditionEventIndex = E_SINGLE_LIGHT_TIMEOUT, .transitionIndex = T_RED_YELLOW_GREEN,
            .numFiredEvents = 1, .firedEventIndices = (unsigned int[]) { E_LIGHT_CHANGED }
        },
        [R_SINGLE_LIGHT_TIMEOUT_GREEN] = {
            .conditionEventIndex = E_SINGLE_LIGHT_TIMEOUT, .transitionIndex = T_GREEN_YELLOW,
            .numFiredEvents = 1, .firedEventIndices = (unsigned int[]) { E_LIGHT_CHANGED }
        },
        [R_SINGLE_LIGHT_TIMEOUT_GREEN_YELLOW] = {
            .conditionEventIndex = E_SINGLE_LIGHT_TIMEOUT, .transitionIndex = T_GREEN_YELLOW_RED,
            .numFiredEvents = 1, .firedEventIndices = (unsigned int[]) { E_LIGHT_CHANGED }
        },
        [R_ALWAYS_TRUE_START] = { .conditionEventIndex = E_STEP, .numFiredEvents = 0, .transitionIndex = T_START_RED },
        [R_ALWAYS_TRUE_RED] = { .conditionEventIndex = E_STEP, .numFiredEvents = 0, .transitionIndex = T_RED_RED },
        [R_ALWAYS_TRUE_RED_YELLOW] = { .conditionEventIndex = E_STEP, .numFiredEvents = 0, .transitionIndex = T_RED_YELLOW_YELLOW },
        [R_ALWAYS_TRUE_GREEN] = { .conditionEventIndex = E_STEP, .numFiredEvents = 0, .transitionIndex = T_GREEN_GREEN },
        [R_ALWAYS_TRUE_GREEN_YELLOW] = { .conditionEventIndex = E_STEP, .numFiredEvents = 0, .transitionIndex = T_GREEN_YELLOW_YELLOW }
    };

    struct fsm_nbx fsm = {
        .numStates = NUM_STATES,
        .numTransitions = NUM_TRANSITIONS,
        .numReactions = NUM_REACTIONS,

        .startStateIndex = S_START,
        .currentStateIndex = S_START,
        .endStateIndex = S_EXIT,
        .states = states,

        .eventData = &eventData,
        .transitions = transitions,
        .reactions = reactions,
    };

    printf("Starting traffic light example\n");
    clock_t start = clock(), startSingleLight = clock(), now;
    double elapsedSecs, elapsedSingleLightSecs;
    while (true) {
        if (fsm.currentStateIndex == S_EXIT) {
            printf("State machine completed successfully\n");
            break;
        }

        produce_event(fsm.eventData, E_STEP);

        /* setup timer events */
        now = clock();

        // timer for each light
        elapsedSingleLightSecs = ((double) (now - startSingleLight)) / CLOCKS_PER_SEC;
        if (elapsedSingleLightSecs > SINGLE_LIGHT_TIMEOUT_SECS) {
            // printf("current state: %s\n", fsm.states[fsm.currentStateIndex].name);
            produce_event(&eventData, E_SINGLE_LIGHT_TIMEOUT);
            startSingleLight = now;
        }

        // global timer for stopping program
        elapsedSecs = ((double) (now - start)) / CLOCKS_PER_SEC;
        if (elapsedSecs > GLOBAL_TIMEOUT_SECS) produce_event(&eventData, E_GLOBAL_TIMEOUT);

        /* run state machine, event loop */
        fsm_behavior(&fsm, &userData);
        fsm_step_nbx(&fsm);
        reconfig_event_buffers(&eventData);
    }

    return 0;
}
