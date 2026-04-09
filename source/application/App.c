/***************************************************************************/ /**
   @file     App.c
   @brief    Application functions
   @author   Nicolás Magliola
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../../SDK/CMSIS/fsl_device_registers.h"
#include "../drivers/HAL/include/board.h"
#include "../drivers/HAL/include/display.h"
#include "../drivers/HAL/include/encoder.h"
#include "../drivers/HAL/include/reader.h"
#include "../drivers/HAL/include/shift_register.h"
#include "../drivers/HAL/include/switch.h"
#include "../drivers/HAL/include/timer.h"
#include "include/App_commons.h"
#include "include/auth.h"
#include "include/fsm_table.h"

#include <stdio.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/**
 * global variable, will be used by fsm
 * since we are working sequentially and interrupts dont access it or use it, it should be safe
 * */
AppContext_t g_app_ctx = {.current_state = NULL, // set after initing tabl
						  .menu_selected = 0,	 /* first by default*/
						  .retry_count = 3,
						  .timer_timeout_block = TIMER_INVALID_ID,
						  .timer_misc = TIMER_INVALID_ID,
						  .display_intensity = 5,
						  .operation_result = false,
						  .card_len = 0,
						  .pin_num = 0,
						  .card_input_ctr = 0,
						  .card_input_len = ID_LENGHT,
						  .manual = false};
/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
*******************************************************************************/

static EVENT App_CaptureEvent();
/********************************************************************************
******************************************************************************/
/* interrupts are disabled at this point*/
void App_Init(void) {
	timer_drv_init();
	switch_drv_init();
	encoder_drv_init();
	display_drv_init();
	reader_drv_init();

	uint8_t display_intensity = MAX_INTENSITY;
	uint8_t word[] = {SEG7_CHAR('H'), SEG7_CHAR('O'), SEG7_CHAR('L'), SEG7_CHAR('A'), SEG7_BLANK,
					  SEG7_CHAR('H'), SEG7_CHAR('O'), SEG7_CHAR('L'), SEG7_CHAR('A')};
	uint8_t word2[DIG_NUM] = {SEG7_BLANK, SEG7_CHAR('H'), SEG7_CHAR('I'), SEG7_EXCL};

	uint8_t nothing[DIG_NUM] = {SEG7_BLANK, SEG7_BLANK, SEG7_BLANK, SEG7_BLANK};

	sw_handle_t btn1 = switch_drv_register(PIN_SW_ENC, ACTIVE_ON_LOW, PULL_UP);
	btn1 == INVALID_SW_HANDLE ? printf("Couldnt initialize sw in pin: %d\n", PIN_SW_ENC) :
								printf("Initialized sw in pin: %d\n", PIN_SW_ENC);

	sw_handle_t btn2 = switch_drv_register(PIN_SW3, ACTIVE_ON_LOW, PULL_UP);
	btn2 == INVALID_SW_HANDLE ? printf("Couldnt initialize sw in pin: %d\n", PIN_SW3) :
								printf("Initialized sw in pin: %d\n", PIN_SW3);

	FSM_InitTable();

	// initial state
	g_app_ctx.current_state = FSM_GetInitState();

	// display_drv_write_word(word, 9);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {
	while (1) {
		timer_drv_update(); /* must be called every iteration */

		// Capture ONE event from all input sources
		EVENT curr_event = App_CaptureEvent();

		// Feed event to FSM if theres something
		if (curr_event != EV_NONE) {
			g_app_ctx.current_state = fsm(g_app_ctx.current_state, curr_event);
		}
	}
}

static EVENT App_CaptureEvent() {
	if (timer_drv_expired(g_app_ctx.timer_misc)) {
		printf("EVENT: EV_TIMEOUT_MISC\n");
		return EV_TIMEOUT_MISC;
	}

	if (timer_drv_expired(g_app_ctx.timer_timeout_block)) {
		printf("EVENT: EV_TIMEOUT_BLOCK\n");
		return EV_TIMEOUT_BLOCK_CTR;
	}

	if (timer_drv_expired(g_app_ctx.timer_misc_err)) {
		printf("EVENT_ EV_TIMEOUT_MISC_ERROR\n");
		return EV_TIMEOUT_MISC_ERROR;
	}
	if (reader_drv_event()) {
		reader_drv_card(g_app_ctx.card_buff, &g_app_ctx.card_len);
		if (auth_id_exists((uint8_t *) g_app_ctx.card_buff)) {
			printf("EVENT: RCV_CARD_S\n");
			return EV_RCV_CARD_S;
		}
		printf("EVENT: RCV_CARD_F\n");
		return EV_RCV_CARD_F;
	}
	swEvent sw_ev = switch_drv_pop_event();
	if (sw_ev.event_type == SW_EVENT_CLICK) {
		printf("EVENT: CLICK\n");
		return EV_CLICK;
	}
	if (sw_ev.event_type == SW_EVENT_DOUBLE_CLICK) {
		printf("EVENT: DOUBLE_CLICK\n");
		return EV_DOUBLE_CLICK;
	}
	if (sw_ev.event_type == SW_EVENT_LONG_CLICK) {
		printf("EVENT: LONG_CLICK\n");
		return EV_LONG_CLICK;
	}

	encoderDir enc_ev = encoder_drv_pop_event();
	if (enc_ev == ENC_CW) {
		// printf("EVENT: ENC_CW\n");
		return EV_ENC_CW;
	}
	if (enc_ev == ENC_CCW) {
		// printf("EVENT: ENC_CCW\n");
		return EV_ENC_CCW;
	}

	// No event
	return EV_NONE;
}
