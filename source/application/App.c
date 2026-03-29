/***************************************************************************/ /**
   @file     App.c
   @brief    Application functions
   @author   Nicolás Magliola
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../../SDK/CMSIS/fsl_device_registers.h"
#include "../drivers/HAL/include/MC74HC139.h"
#include "../drivers/HAL/include/board.h"
#include "../drivers/HAL/include/encoder.h"
#include "../drivers/HAL/include/switch.h"
#include "../drivers/HAL/include/timer.h"

#include <stdio.h>

typedef enum {
	STATUS_LED_NONE = MC74HC139_Y0,
	STATUS_LED_D1 = MC74HC139_Y1,
	STATUS_LED_D2 = MC74HC139_Y2,
	STATUS_LED_D3 = MC74HC139_Y3,
} DEncStatusLeds;

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
	timerInit();
	swInit();
	encoderInit();
	MC74HC139Init(MC74HC139_DEV_U1B);
	MC74HC139Select(MC74HC139_DEV_U1B, STATUS_LED_NONE);

	sw_handle_t btn1 = swRegister(PIN_SW_ENC, ACTIVE_ON_LOW, PULL_UP);
	btn1 == INVALID_SW_HANDLE ? printf("Couldnt initialize sw in pin: %d\n", PIN_SW_ENC) :
								printf("Initialized sw in pin: %d\n", PIN_SW_ENC);

	sw_handle_t btn2 = swRegister(PIN_SW3, ACTIVE_ON_LOW, PULL_UP);
	btn2 == INVALID_SW_HANDLE ? printf("Couldnt initialize sw in pin: %d\n", PIN_SW3) :
								printf("Initialized sw in pin: %d\n", PIN_SW3);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {
	EncoderDir enc_ev;
	/**
	 * Obs!! For printf must enable semihosting:
	 * Set to RedLink (-semihosting) in:
	 *  proj properties > C/C++ Build > Settings > MCU Linker >Managed Linker Script > Library
	 *
	 */

	while (1) {
		timerUpdate(); /* must be called every iteration  @todo ask if this is ok*/

		swEvent ev = swPopEvent();
		switch (ev.event_type) {
			case SW_EVENT_CLICK:
				printf("Pin %d -> CLICK\n", ev.swPin);
				MC74HC139Select(MC74HC139_DEV_U1B, STATUS_LED_D1);
				break;
			case SW_EVENT_DOUBLE_CLICK:
				printf("Pin %d -> DOUBLE CLICK\n", ev.swPin);
				MC74HC139Select(MC74HC139_DEV_U1B, STATUS_LED_D2);

				break;
			case SW_EVENT_LONG_CLICK:
				printf("Pin %d -> LONG CLICK\n", ev.swPin);
				MC74HC139Select(MC74HC139_DEV_U1B, STATUS_LED_D3);

				break;
			case SW_EVENT_NONE:
			default:
				break;
		}
		while ((enc_ev = encPopEvent()) != ENC_NONE) { // pop all in the queue
			if (enc_ev == ENC_CCW) {
				printf("CCW step\n");
			}
			if (enc_ev == ENC_CW) {
				printf("CW step\n");
			}
		}
	}
}
