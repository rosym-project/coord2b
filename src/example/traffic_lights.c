#include <stdio.h>
#include <time.h>
#include "coord2b/functions/fsm.h"
#include "coord2b/functions/event_loop.h"

const double GLOBAL_TIMEOUT_SECS = 10.;
const double SINGLE_LIGHT_TIMEOUT_SECS = 0.5;

enum e_events {
    E_RED_ENTERED = 0,
    E_RED_YELLOW_ENTERED,
    E_GREEN_ENTERED,
    E_GREEN_YELLOW_ENTERED,
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
    R_ALWAYS_TRUE = 0,
    R_GLOBAL_TIMER,
    R_NOT_SINGLE_LIGHT_TIMEOUT,
    R_SINGLE_LIGHT_TIMEOUT,
    NUM_REACTIONS
};

enum e_behaviors {
    B_RED_ENTERED = 0,
    B_RED_YELLOW_ENTERED,
    B_GREEN_ENTERED,
    B_GREEN_YELLOW_ENTERED,
    NUM_BEHAVIORS
};

_Bool condition_always_true(struct events *eventData) { return true; }

_Bool condition_global_timeout(struct events *eventData) { return consume_event(eventData, E_GLOBAL_TIMEOUT); }

_Bool condition_not_single_light_timeout(struct events *eventData) {
    return !consume_event(eventData, E_SINGLE_LIGHT_TIMEOUT);
}

_Bool condition_single_light_timeout(struct events *eventData) {
    return consume_event(eventData, E_SINGLE_LIGHT_TIMEOUT);
}

_Bool condition_red_entered(struct events *eventData) { return consume_event(eventData, E_RED_ENTERED); }

_Bool condition_red_yel_entered(struct events *eventData) { return consume_event(eventData, E_RED_YELLOW_ENTERED); }

_Bool condition_green_entered(struct events *eventData) { return consume_event(eventData, E_GREEN_ENTERED); }

_Bool condition_green_yel_entered(struct events *eventData) {
    return consume_event(eventData, E_GREEN_YELLOW_ENTERED);
}

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
    generic_behavior(userData);
}

void red_yel_behavior(struct user_data * userData) {
    userData->redOn = true;
    userData->yellowOn = true;
    userData->greenOn = false;
    generic_behavior(userData);
}

void green_behavior(struct user_data * userData) {
    userData->redOn = false;
    userData->yellowOn = false;
    userData->greenOn = true;
    generic_behavior(userData);
}

void green_yel_behavior(struct user_data * userData) {
    userData->redOn = false;
    userData->yellowOn = true;
    userData->greenOn = false;
    generic_behavior(userData);
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
        [T_START_RED] = {
            .startStateIndex = S_START, .endStateIndex = S_RED,
            .numFiredEvents = 1, .firedEventIndices = (unsigned int[]) { E_RED_ENTERED }
        },
        [T_RED_EXIT] = { .startStateIndex = S_RED, .endStateIndex = S_EXIT, .numFiredEvents = 0 },
        [T_RED_RED]  = { .startStateIndex = S_RED, .endStateIndex = S_RED,  .numFiredEvents = 0 },
        [T_RED_YELLOW] = {
            .startStateIndex = S_RED, .endStateIndex = S_RED_YELLOW,
            .numFiredEvents = 1, .firedEventIndices = (unsigned int []) { E_RED_YELLOW_ENTERED }
        },
        [T_RED_YELLOW_EXIT] = {
            .startStateIndex = S_RED_YELLOW, .endStateIndex = S_EXIT, .numFiredEvents = 0
        },
        [T_RED_YELLOW_YELLOW] = {
            .startStateIndex = S_RED_YELLOW, .endStateIndex = S_RED_YELLOW, .numFiredEvents = 0
        },
        [T_RED_YELLOW_GREEN] = {
            .startStateIndex = S_RED_YELLOW, .endStateIndex = S_GREEN,
            .numFiredEvents = 1, .firedEventIndices = (unsigned int []) { E_GREEN_ENTERED }
        },
        [T_GREEN_EXIT]  = { .startStateIndex = S_GREEN, .endStateIndex = S_EXIT,  .numFiredEvents = 0 },
        [T_GREEN_GREEN] = { .startStateIndex = S_GREEN, .endStateIndex = S_GREEN, .numFiredEvents = 0 },
        [T_GREEN_YELLOW] = {
            .startStateIndex = S_GREEN, .endStateIndex = S_GREEN_YELLOW,
            .numFiredEvents = 1, .firedEventIndices = (unsigned int []) { E_GREEN_YELLOW_ENTERED }
        },
        [T_GREEN_YELLOW_EXIT] = {
            .startStateIndex = S_GREEN_YELLOW, .endStateIndex = S_EXIT, .numFiredEvents = 0
        },
        [T_GREEN_YELLOW_YELLOW] = {
            .startStateIndex = S_GREEN_YELLOW, .endStateIndex = S_GREEN_YELLOW, .numFiredEvents = 0
        },
        [T_GREEN_YELLOW_RED] = {
            .startStateIndex = S_GREEN_YELLOW, .endStateIndex = S_RED,
            .numFiredEvents = 1, .firedEventIndices = (unsigned int []) { E_RED_ENTERED }
        }
    };

    struct event_reaction reactions[NUM_REACTIONS] = {
        [R_ALWAYS_TRUE] = {
            .numTransitions = 1,
            .condition = &condition_always_true,
            .transitionIndices = (unsigned int[]) {T_START_RED}
        },
        [R_GLOBAL_TIMER] = {
            .numTransitions = 4,
            .condition = &condition_global_timeout,
            .transitionIndices = (unsigned int[]) {T_RED_EXIT, T_RED_YELLOW_EXIT, T_GREEN_EXIT, T_GREEN_YELLOW_EXIT}
        },
        [R_NOT_SINGLE_LIGHT_TIMEOUT] = {
            .numTransitions = 4,
            .condition = &condition_not_single_light_timeout,
            .transitionIndices = (unsigned int[]) {
                T_RED_RED, T_RED_YELLOW_YELLOW, T_GREEN_GREEN, T_GREEN_YELLOW_YELLOW
            }
        },
        [R_SINGLE_LIGHT_TIMEOUT] = {
            .numTransitions = 4,
            .condition = &condition_single_light_timeout,
            .transitionIndices = (unsigned int[]) {
                T_RED_YELLOW, T_RED_YELLOW_GREEN, T_GREEN_YELLOW, T_GREEN_YELLOW_RED
            }
        }
    };

    struct event_behavior behaviors[NUM_BEHAVIORS] = {
        [B_RED_ENTERED] = {
            .condition = &condition_red_entered, .execute = (void (*)(void *)) &red_behavior
        },
        [B_RED_YELLOW_ENTERED] = {
            .condition = &condition_red_yel_entered, .execute = (void (*)(void *)) &red_yel_behavior
        },
        [B_GREEN_ENTERED] = {
            .condition = &condition_green_entered, .execute = (void (*)(void *)) &green_behavior
        },
        [B_GREEN_YELLOW_ENTERED] = {
            .condition = &condition_green_yel_entered, .execute = (void (*)(void *)) &green_yel_behavior
        },
    };

    struct fsm_nbx fsm = {
        .numStates = NUM_STATES,
        .numTransitions = NUM_TRANSITIONS,
        .numReactions = NUM_REACTIONS,
        .numBehaviors = NUM_BEHAVIORS,

        .startStateIndex = S_START,
        .currentStateIndex = S_START,
        .endStateIndex = S_EXIT,
        .states = states,

        .eventData = &eventData,
        .transitions = transitions,
        .reactions = reactions,
        .behaviors = behaviors,

        .data = (void *) &userData
    };

    printf("Starting traffic light example\n");
    clock_t start = clock(), startSingleLight = clock(), now;
    double elapsedSecs, elapsedSingleLightSecs;
    while (true) {
        if (fsm.currentStateIndex == S_EXIT) {
            printf("State machine completed successfully\n");
            break;
        }

        /* setup timer events */
        now = clock();

        // timer for each light
        elapsedSingleLightSecs = ((double) (now - startSingleLight)) / CLOCKS_PER_SEC;
        if (elapsedSingleLightSecs > SINGLE_LIGHT_TIMEOUT_SECS) {
            produce_event(&eventData, E_SINGLE_LIGHT_TIMEOUT);
            startSingleLight = now;
        }

        // global timer for stopping program
        elapsedSecs = ((double) (now - start)) / CLOCKS_PER_SEC;
        if (elapsedSecs > GLOBAL_TIMEOUT_SECS) produce_event(&eventData, E_GLOBAL_TIMEOUT);

        /* run state machine, event loop */
        fsm_step_nbx(&fsm);
        event_loop_step(&eventData);
    }

    return 0;
}
