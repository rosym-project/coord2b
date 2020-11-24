#ifndef COORD2B_FUNCTIONS_FSM_H
#define COORD2B_FUNCTIONS_FSM_H

#include "coord2b/types/fsm.h"

#ifdef __cplusplus
extern "C" {
#endif

void fsm_step_nbx(struct fsm_nbx * fsm);
void produce_event(_Bool * events, int eventIndex);
_Bool consume_event(_Bool * events, int eventIndex);

#ifdef __cplusplus
}
#endif

#endif // COORD2B_FUNCTIONS_FSM_H