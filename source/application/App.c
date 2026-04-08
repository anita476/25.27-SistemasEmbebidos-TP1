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
						  .menu_selected = 0,
						  .retry_count = 3,
						  .timer_timeout_block = TIMER_INVALID_ID,
						  .timer_misc = TIMER_INVALID_ID,
						  .display_intensity = 50,
						  .operation_result = false,
						  .pending_event = EV_NONE};
/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
*******************************************************************************/

EVENT App_CaptureEvent();
/********************************************************************************
******************************************************************************/
/* interrupts are disabled at this point*/
void App_Init(void) {
	timer_drv_init();
	switch_drv_init();
	encoder_drv_init();
	display_drv_init();
	reader_drv_init();

	sw_handle_t btn1 = switch_drv_register(PIN_SW_ENC, ACTIVE_ON_LOW, PULL_UP);
	btn1 == INVALID_SW_HANDLE ? printf("Couldnt initialize sw in pin: %d\n", PIN_SW_ENC) :
								printf("Initialized sw in pin: %d\n", PIN_SW_ENC);

	sw_handle_t btn2 = switch_drv_register(PIN_SW3, ACTIVE_ON_LOW, PULL_UP);
	btn2 == INVALID_SW_HANDLE ? printf("Couldnt initialize sw in pin: %d\n", PIN_SW3) :
								printf("Initialized sw in pin: %d\n", PIN_SW3);

	g_app_ctx.timer_timeout_block = timer_drv_get_id();
	g_app_ctx.timer_misc = timer_drv_get_id();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {
	while (1) {
		timer_drv_update(); /* must be called every iteration */

		// Capture ONE event from all input sources
		EVENT curr_event = App_CaptureEvent();

		// Feed event to FSM
		/*
		if (curr_event != EV_NONE) {
			g_app_ctx.current_state = fsm(g_app_ctx.current_state, curr_event);
		}
		*/
	}
}

EVENT App_CaptureEvent() {
	// @todo add magnetic reading logic !
	// if(reader_drv_has_card()){
	//	get card and set success or failure
	// }
	reader_drv_event();
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
		printf("EVENT: ENC_CW\n");
		return EV_ENC_CW;
	}
	if (enc_ev == ENC_CCW) {
		printf("EVENT: ENC_CCW\n");
		return EV_ENC_CCW;
	}

	if (timer_drv_expired(g_app_ctx.timer_timeout_block)) {
		return EV_TIMEOUT_BLOCK_CTR;
	}
	if (timer_drv_expired(g_app_ctx.timer_misc)) {
		return EV_TIMEOUT_MISC;
	}
	// No event
	return EV_NONE;
}
