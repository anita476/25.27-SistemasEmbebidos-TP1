#include "include/fsm.h"
#include <stdio.h>

/**
 * Using given fsm example
 */
FSMState_t *fsm(FSMState_t *p_state_table, EVENT curr_event) {
	printf("Event >>%d<<\n", curr_event);
	while (p_state_table->event != curr_event // Go through the state tables
		   && p_state_table->event != TABLE_END) {
		++p_state_table;
	}
	//(*p_state_table->action_fun)();			   /* perform action routine */
	p_state_table->action_fun();
	p_state_table = p_state_table->next_state; /*next state*/
	// Obs! Event isnt changed
	return (p_state_table);
}