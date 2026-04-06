#include "include/fsm_table.h"
#include "include/App_commons.h"

/*  Forward declarations of state arrays */
FSMState_t state_init[];
FSMState_t state_menu_main[];
FSMState_t state_op_read_card[];
FSMState_t state_op_input_card[];
FSMState_t state_op_set_intensity[];
FSMState_t state_input_pin[];
FSMState_t state_success[];
FSMState_t state_failure[];

/* ── Forward declarations of actions @todo missing */
// @todo these are missing ! they dont do anythin now.
// obs! ojo con los timers, varias acciones tienen que stoppear or inicializarlos
static void action_show_greeting(void); // start misc timer, write and exit
static void action_show_menu(void);
static void action_do_nothing(void);
static void action_menu_next(void);
static void action_menu_prev(void);
static void action_menu_select(void);
static void action_reset_retries(void);
static void action_show_card(void);
static void action_show_error_card(void);
static void action_intensity_increase(void);
static void action_intensity_decrease(void);
static void action_retry_or_fail(void);
static void action_input_card_increment_dig(void);
static void action_input_card_decrement_dig(void);
static void action_advance_card_num(void);
static void action_rollback_card_num(void);
static void action_stop_misc_timer(); /* in case we perform an action, stop timer to be safe */

/*************************************************************************************************************************/
/***********************************************  TRANSITIONS. **************************************************/
/*************************************************************************************************************************/
FSMState_t state_init[] = {{EV_NONE, NULL, action_show_greeting},
						   {EV_CLICK, NULL, action_stop_misc_timer}, /* if someone clicks or smth, exit to main menu*/
						   {EV_DOUBLE_CLICK, NULL, action_stop_misc_timer},
						   {EV_LONG_CLICK, NULL, action_stop_misc_timer},
						   {EV_TIMEOUT_MISC, NULL, action_do_nothing},
						   {TABLE_END, NULL, action_do_nothing}};

FSMState_t state_menu_main[] = {{EV_ENC_CW, NULL, action_menu_next},
								{EV_ENC_CCW, NULL, action_menu_prev},
								{EV_CLICK, NULL, action_menu_select},
								{EV_LONG_CLICK, NULL, action_do_nothing},
								{TABLE_END, NULL, action_do_nothing}};

FSMState_t state_op_read_card[] = {{EV_RCV_CARD_S, NULL, action_show_card},
								   {EV_RCV_CARD_F, NULL, action_show_error_card},
								   {EV_LONG_CLICK, NULL, action_do_nothing},
								   {TABLE_END, NULL, action_do_nothing}};

FSMState_t state_op_input_card[] = {{EV_ENC_CW, NULL, action_input_card_increment_dig},
									{EV_ENC_CCW, NULL, action_input_card_decrement_dig},
									{EV_CLICK, NULL, action_advance_card_num},
									{EV_DOUBLE_CLICK, NULL, action_rollback_card_num},
									{EV_SUCCESS, NULL, action_show_card},
									{EV_RCV_CARD_F, NULL, action_retry_or_fail},
									{EV_END_TRIES, NULL, action_show_error_card},
									{EV_LONG_CLICK, NULL, action_show_menu},
									{TABLE_END, NULL, action_do_nothing}};

FSMState_t state_op_set_intensity[] = {{EV_ENC_CW, NULL, action_intensity_increase},
									   {EV_ENC_CCW, NULL, action_intensity_decrease},
									   {EV_CLICK, NULL, action_show_menu},
									   {EV_LONG_CLICK, NULL, action_show_menu},
									   {TABLE_END, NULL, action_do_nothing}};

FSMState_t state_input_pin[] = {{EV_ENC_CW, NULL, action_input_card_increment_dig},
								{EV_ENC_CCW, NULL, action_input_card_decrement_dig},
								{EV_CLICK, NULL, action_advance_card_num},
								{EV_SUCCESS, NULL, action_show_card},
								{EV_RCV_CARD_F, NULL, action_retry_or_fail},
								{EV_END_TRIES, NULL, action_show_error_card},
								{EV_LONG_CLICK, NULL, action_show_menu},
								{TABLE_END, NULL, action_do_nothing}};

FSMState_t state_success[] = {{EV_TIMEOUT_MISC, NULL, action_show_menu}, {TABLE_END, NULL, action_do_nothing}};

FSMState_t state_failure[] = {{EV_TIMEOUT_BLOCK_CTR, NULL, action_show_menu}, {TABLE_END, NULL, action_do_nothing}};

void FSM_InitTable(void) {
	state_init[0].next_state = state_menu_main;
	state_init[1].next_state = state_menu_main;
	state_init[2].next_state = state_menu_main;
	state_init[3].next_state = state_menu_main;
	state_init[4].next_state = state_menu_main;
	state_init[5].next_state = state_menu_main;

	state_menu_main[0].next_state = state_menu_main; // EV_ENC_CW
	state_menu_main[1].next_state = state_menu_main; // EV_ENC_CCW
	state_menu_main[2].next_state = state_menu_main; // EV_CLICK (action decides)
	state_menu_main[3].next_state = state_menu_main; // EV_LONG_CLICK
	state_menu_main[4].next_state = state_menu_main; // TABLE_END

	state_op_read_card[0].next_state = state_input_pin;	   // EV_RCV_CARD_S
	state_op_read_card[1].next_state = state_menu_main;	   // EV_RCV_CARD_F
	state_op_read_card[2].next_state = state_menu_main;	   // EV_LONG_CLICK
	state_op_read_card[3].next_state = state_op_read_card; // TABLE_END

	state_op_input_card[0].next_state = state_op_input_card; // EV_ENC_CW
	state_op_input_card[1].next_state = state_op_input_card; // EV_ENC_CCW
	state_op_input_card[2].next_state = state_op_input_card; // EV_CLICK
	state_op_input_card[3].next_state = state_op_input_card; // EV_DOUBLE_CLICK
	state_op_input_card[4].next_state = state_success;		 // EV_SUCCESS
	state_op_input_card[5].next_state = state_op_input_card; // EV_RCV_CARD_F
	state_op_input_card[6].next_state = state_failure;		 // EV_END_TRIES
	state_op_input_card[7].next_state = state_menu_main;	 // EV_LONG_CLICK
	state_op_input_card[8].next_state = state_op_input_card; // TABLE_END

	state_op_set_intensity[0].next_state = state_op_set_intensity; // EV_ENC_CW
	state_op_set_intensity[1].next_state = state_op_set_intensity; // EV_ENC_CCW
	state_op_set_intensity[2].next_state = state_menu_main;		   // EV_CLICK
	state_op_set_intensity[3].next_state = state_menu_main;		   // EV_LONG_CLICK
	state_op_set_intensity[4].next_state = state_op_set_intensity; // TABLE_END

	state_input_pin[0].next_state = state_input_pin; // EV_ENC_CW
	state_input_pin[1].next_state = state_input_pin; // EV_ENC_CCW
	state_input_pin[2].next_state = state_input_pin; // EV_CLICK
	state_input_pin[3].next_state = state_success;	 // EV_SUCCESS
	state_input_pin[4].next_state = state_input_pin; // EV_RCV_CARD_F
	state_input_pin[5].next_state = state_failure;	 // EV_END_TRIES
	state_input_pin[6].next_state = state_menu_main; // EV_LONG_CLICK
	state_input_pin[7].next_state = state_input_pin; // TABLE_END

	state_success[0].next_state = state_init;	 // EV_TIMEOUT_MISC
	state_success[1].next_state = state_success; // TABLE_END

	state_failure[0].next_state = state_init;	 // EV_TIMEOUT_BLOCK_CTR
	state_failure[1].next_state = state_failure; // TABLE_END
}

FSMState_t *FSM_GetInitState(void) {
	return state_init;
}

/*************************************************************************************************************************/
/***********************************************  ACTIONS *******************************************************/
/*************************************************************************************************************************/
static void action_show_greeting(void) {
}
static void action_show_menu(void) {
}
static void action_do_nothing(void) {
}
static void action_menu_next(void) {
}
static void action_menu_prev(void) {
}
static void action_menu_select(void) {
}
static void action_reset_retries(void) {
}
static void action_show_card(void) {
}
static void action_show_error_card(void) {
}
static void action_intensity_increase(void) {
}
static void action_intensity_decrease(void) {
}
static void action_retry_or_fail(void) {
}
static void action_input_card_increment_dig(void) {
}
static void action_input_card_decrement_dig(void) {
}
static void action_advance_card_num(void) {
}
static void action_rollback_card_num(void) {
}
static void action_stop_misc_timer() {
}