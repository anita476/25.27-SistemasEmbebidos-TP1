/***************************************************************************/ /**
   @file     App.c
   @brief    Application functions
   @author   Nicolás Magliola
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../../SDK/CMSIS/fsl_device_registers.h"
#include "../drivers/HAL/include/encoder.h"
#include "../drivers/HAL/include/switch.h"
#include "../drivers/HAL/include/timer.h"

#include "include/board.h"
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
	timerInit();
	swInit();
	// encoderInit(PIN_ENC_CHNA, PIN_ENC_CHNB);
	sw_handle_t btn1 = swRegister(PIN_SW_ENC, ACTIVE_ON_LOW, PULL_UP);
	swRegister(PIN_SW3, ACTIVE_ON_LOW, PULL_UP);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {
	// EncoderDir ev;
	// while ((ev = encPopEvent()) != ENC_NONE) { // pop all in the queue
	//	if (ev == ENC_CCW) {
	/**
	 * Obs!! For printf must enable semihosting:
	 * Set to RedLink (-semihosting) in:
	 *  proj properties > C/C++ Build > Settings > MCU Linker >Managed Linker Script > Library
	 *
	 */
	//		printf("CCW step\n");
	//	}
	//	if (ev == ENC_CW) {
	//		printf("CW step\n");
	//	}
	//}

	while (1) {
		timerUpdate(); /* must be called every iteration  @todo ask if this is ok*/

		swEvent ev = swPopEvent();
		switch (ev.event_type) {
			case SW_EVENT_CLICK:
				printf("Pin %d -> CLICK\n", ev.swPin);
				break;
			case SW_EVENT_DOUBLE_CLICK:
				printf("Pin %d -> DOUBLE CLICK\n", ev.swPin);
				break;
			case SW_EVENT_LONG_CLICK:
				printf("Pin %d -> LONG CLICK\n", ev.swPin);
				break;
			case SW_EVENT_NONE:
			default:
				break;
		}
	}
}
