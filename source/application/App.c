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
#include "../drivers/HAL/include/shift_register.h"
#include "../drivers/HAL/include/switch.h"
#include "../drivers/HAL/include/timer.h"

#include <stdio.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/* interrupts are disabled at this point*/
void App_Init(void) {
	timer_drv_init();
	switch_drv_init();
	encoder_drv_init();
	display_drv_init();

	sw_handle_t btn1 = switch_drv_register(PIN_SW_ENC, ACTIVE_ON_LOW, PULL_UP);
	btn1 == INVALID_SW_HANDLE ? printf("Couldnt initialize sw in pin: %d\n", PIN_SW_ENC) :
								printf("Initialized sw in pin: %d\n", PIN_SW_ENC);

	sw_handle_t btn2 = switch_drv_register(PIN_SW3, ACTIVE_ON_LOW, PULL_UP);
	btn2 == INVALID_SW_HANDLE ? printf("Couldnt initialize sw in pin: %d\n", PIN_SW3) :
								printf("Initialized sw in pin: %d\n", PIN_SW3);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {
	encoderDir enc_ev;
	uint8_t word[DIG_NUM] = {SEG7_CHAR('H'), SEG7_CHAR('O'), SEG7_CHAR('L'), SEG7_CHAR('A')};
	uint8_t nothing[DIG_NUM] = {SEG7_BLANK, SEG7_BLANK, SEG7_BLANK, SEG7_BLANK};
	/**
	 * Obs!! For printf must enable semihosting:
	 * Set to RedLink (-semihosting) in:
	 *  proj properties > C/C++ Build > Settings > MCU Linker >Managed Linker Script > Library
	 *
	 */

	while (1) {
		timer_drv_update(); /* must be called every iteration  @todo ask if this is ok*/
		swEvent ev = switch_drv_pop_event();
		switch (ev.event_type) {
			case SW_EVENT_CLICK:
				printf("Pin %d -> CLICK\n", ev.swPin);
				display_drv_write_word(word);
				shift_register_drv_sel_led(LED_SEL_FIRST);
				break;
			case SW_EVENT_DOUBLE_CLICK:
				printf("Pin %d -> DOUBLE CLICK\n", ev.swPin);
				shift_register_drv_sel_led(LED_SEL_SECOND);

				// display_drv_write_to_digit(1, SEG7_CHAR('L'));

				break;
			case SW_EVENT_LONG_CLICK:
				printf("Pin %d -> LONG CLICK\n", ev.swPin);
				shift_register_drv_sel_led(LED_SEL_THIRD);
				display_drv_write_word(nothing);

				break;
			case SW_EVENT_NONE:
			default:
				break;
		}
		while ((enc_ev = encoder_drv_pop_event()) != ENC_NONE) { // pop all in the queue
			if (enc_ev == ENC_CCW) {
				printf("CCW step\n");
			}
			if (enc_ev == ENC_CW) {
				printf("CW step\n");
			}
		}
	}
}
