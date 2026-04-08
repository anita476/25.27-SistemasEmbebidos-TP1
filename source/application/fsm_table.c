#include "include/fsm_table.h"
#include "../drivers/HAL/include/display.h"
#include "../drivers/HAL/include/timer.h"
#include "include/App_commons.h"
#include <stdio.h>

/****************************CONST DECLARATION OF DISPLAY ARRAYS ********************************************/
const uint8_t GREETING[] = {SEG7_H, SEG7_E, SEG7_L, SEG7_L, SEG7_O, SEG7_EXCL};
const uint8_t GREETING_NUM = 6; // if i do strlen here, bc its constant its defined at compiletime ?

const uint8_t OPEN[] = {SEG7_O, SEG7_P, SEG7_E, SEG7_N};
const uint8_t OPEN_NUM = 4;

const uint8_t FAIL[] = {SEG7_F, SEG7_A, SEG7_I, SEG7_L};
const uint8_t FAIL_NUM = 4;

const uint8_t CLR[] = {SEG7_BLANK, SEG7_BLANK, SEG7_BLANK, SEG7_BLANK};
const uint8_t CLR_NUM = 4;

/* 1_ CARD */
const uint8_t MENU_CARD[] = {SEG7_1, SEG7_DP, SEG7_BLANK, SEG7_C, SEG7_A, SEG7_R, SEG7_D};
const uint8_t MENU_CARD_NUM = 7;

/* 2_ INPUT*/
const uint8_t MENU_MANUAL[] = {SEG7_2, SEG7_DP, SEG7_BLANK, SEG7_I, SEG7_N, SEG7_P, SEG7_U, SEG7_T};
const uint8_t MENU_MANUAL_NUM = 8;

/* 3_ INT.*/
const uint8_t MENU_INTENSITY[] = {SEG7_3, SEG7_DP, SEG7_BLANK, SEG7_I, SEG7_N, SEG7_T, SEG7_DP};
const uint8_t MENU_INTENSITY_NUM = 7;

/* .......   ........ */
const uint8_t WAITING[] = {SEG7_DP,	   SEG7_DP, SEG7_DP, SEG7_DP, SEG7_DP, SEG7_DP, SEG7_BLANK,
						   SEG7_BLANK, SEG7_DP, SEG7_DP, SEG7_DP, SEG7_DP, SEG7_DP, SEG7_DP};
const uint8_t WAITING_NUM = 14;

/*  Forward declarations of state arrays */
FSMState_t state_init[];
FSMState_t state_menu_main[];
FSMState_t state_op_read_card[];
FSMState_t state_op_input_card[];
FSMState_t state_op_set_intensity[];
FSMState_t state_input_pin[];
FSMState_t state_success[];
FSMState_t state_failure[];

typedef struct {
	uint8_t *item;
	uint8_t item_length;
	FSMState_t *next_state;
} MenuItem_t;

const MenuItem_t menu[MENU_ITEMS] = {
	{.item = (uint8_t *) MENU_CARD, .item_length = MENU_CARD_NUM, .next_state = state_op_read_card},
	{.item = (uint8_t *) MENU_MANUAL, .item_length = MENU_MANUAL_NUM, .next_state = state_op_input_card},
	{.item = (uint8_t *) MENU_INTENSITY, .item_length = MENU_INTENSITY_NUM, .next_state = state_op_set_intensity}};

/* ── Forward declarations of actions @todo missing */
// obs! ojo con los timers, varias acciones tienen que stoppear or inicializarlos
static void action_show_greeting();
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
static void action_stop_misc_timer_and_menu(); /* in case we perform an action, stop timer to be safe */
static void action_clear_display();

/*************************************************************************************************************************/
/***********************************************  TRANSITIONS. **************************************************/
/*************************************************************************************************************************/
FSMState_t state_init[] = {
	{EV_CLICK, NULL, action_stop_misc_timer_and_menu}, /* if someone clicks or smth, exit to main menu*/
	{EV_DOUBLE_CLICK, NULL, action_stop_misc_timer_and_menu},
	{EV_LONG_CLICK, NULL, action_stop_misc_timer_and_menu},
	{EV_TIMEOUT_MISC, NULL, action_show_menu},
	{TABLE_END, NULL, action_do_nothing}};

FSMState_t state_menu_main[] = {{EV_ENC_CW, NULL, action_menu_next},
								{EV_ENC_CCW, NULL, action_menu_prev},
								{EV_CLICK, NULL, action_menu_select},
								{EV_LONG_CLICK, NULL, action_do_nothing},
								{TABLE_END, NULL, action_do_nothing}};

FSMState_t state_op_read_card[] = {{EV_RCV_CARD_S, NULL, action_show_card},
								   {EV_RCV_CARD_F, NULL, action_show_error_card},
								   {EV_LONG_CLICK, NULL, action_show_menu}, // go back to menu
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
	/* TIMERS BEFORE INITING TABLES !*/
	g_app_ctx.timer_timeout_block = timer_drv_get_id();
	g_app_ctx.timer_misc = timer_drv_get_id();

	// todo: fix later if theres time

	state_init[0].next_state = state_menu_main;
	state_init[1].next_state = state_menu_main;
	state_init[2].next_state = state_menu_main;
	state_init[3].next_state = state_menu_main;
	state_init[4].next_state = state_init; // if nothing happened, stay here

	state_menu_main[0].next_state = state_menu_main; // EV_ENC_CW
	state_menu_main[1].next_state = state_menu_main; // EV_ENC_CCW
	state_menu_main[2].next_state = NULL;			 // EV_CLICK (action decides the next state)
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

	/* Leave things in initial state*/
	action_show_greeting();
}

FSMState_t *FSM_GetInitState(void) {
	return state_init;
}

/*************************************************************************************************************************/
/***********************************************  ACTIONS *******************************************************/
/*************************************************************************************************************************/
static void action_show_greeting(void) {
	// start timer
	bool ok = timer_drv_start(g_app_ctx.timer_misc, 3000, TIM_MODE_SINGLESHOT, NULL);
	printf("timer started: %d, id: %d\n", ok, g_app_ctx.timer_misc);
	// show display
	display_drv_write_word((uint8_t *) GREETING, GREETING_NUM);
	// exit
}

static void action_show_menu(void) {
	display_drv_write_word(menu[g_app_ctx.menu_selected].item, menu[g_app_ctx.menu_selected].item_length);
}

static void action_do_nothing(void) {
	/*/*/
}
static void action_menu_next(void) {
	g_app_ctx.menu_selected = (g_app_ctx.menu_selected + 1) % MENU_ITEMS;
	printf("Switched to menu item: %d\n", g_app_ctx.menu_selected);
	display_drv_write_word(menu[g_app_ctx.menu_selected].item, menu[g_app_ctx.menu_selected].item_length);
}
static void action_menu_prev(void) {
	g_app_ctx.menu_selected = (g_app_ctx.menu_selected - 1) % MENU_ITEMS;
	printf("Switched to menu item: %d\n", g_app_ctx.menu_selected);
	display_drv_write_word(menu[g_app_ctx.menu_selected].item, menu[g_app_ctx.menu_selected].item_length);
}
static void action_menu_select(void) {
	printf("Selected menu item %d", g_app_ctx.menu_selected);
	g_app_ctx.current_state = menu[g_app_ctx.menu_selected].next_state;
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
static void action_stop_misc_timer_and_menu() {
	// stop timer,
	timer_drv_stop(g_app_ctx.timer_misc);
	// display_drv_write_word((uint8_t *) OPEN, OPEN_NUM);
	printf("Stopped misc timer prematurely\n");

	action_show_menu();
	// exit
}

static void action_clear_display() {
	display_drv_write_word(CLR, CLR_NUM);
}
