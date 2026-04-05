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

#define MAX_PIN_LEN 5
	uint8_t pin[MAX_PIN_LEN] = {0};
	uint8_t pin_index = 0;
	int cur_value = 0;

	uint8_t display_digits[DIG_NUM] = {SEG7_BLANK, SEG7_BLANK, SEG7_BLANK, SEG7_BLANK};
	for (int i = 0; i < pin_index && i < DIG_NUM; i++) {
		display_digits[i] = SEG7_CHAR('0' + pin[i]);
	}

	uint8_t display_intensity = MAX_INTENSITY;

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
				if (pin_index < MAX_PIN_LEN) {
					pin[pin_index++] = cur_value;
					printf("Stored digit %d at pos %d\n", cur_value, pin_index - 1);

					cur_value = 0; // reset for next digit
				}

				printf("Pin %d -> CLICK\n", ev.swPin);
				display_drv_write_word(display_digits, DIG_NUM);
				shift_register_drv_sel_led(LED_SEL_FIRST);
				break;
			case SW_EVENT_DOUBLE_CLICK:
				// Reset system
				pin_index = 0;
				cur_value = 0;
				break;
			case SW_EVENT_LONG_CLICK:
				printf("Pin %d -> LONG CLICK\n", ev.swPin);
				shift_register_drv_sel_led(LED_SEL_THIRD);
				display_drv_write_word(nothing, DIG_NUM);

				break;
			case SW_EVENT_NONE:
			default:
				break;
		}
		while ((enc_ev = encoder_drv_pop_event()) != ENC_NONE) { // pop all in the queue
			if (enc_ev == ENC_CCW) {
				printf("CCW step\n");
				cur_value++;
			}
			if (enc_ev == ENC_CW) {
				printf("CW step\n");
				cur_value--;
			}
		}
	}
}