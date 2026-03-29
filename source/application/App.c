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
#include "../drivers/MCAL/include/gpio.h"
#include "../drivers/MCAL/include/pisr.h"
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
	gpioMode(PIN_LED_GREEN, OUTPUT);
	gpioMode(PIN_LED_RED, OUTPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);

	gpioWrite(PIN_LED_GREEN, !LED_ACTIVE);
	gpioWrite(PIN_LED_RED, !LED_ACTIVE);
	gpioWrite(PIN_LED_BLUE, !LED_ACTIVE);

	encoderInit(PIN_ENC_CHNA, PIN_ENC_CHNB);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {
	EncoderDir ev;
	while ((ev = popEvent()) != ENC_NONE) { // pop all in the queue
		if (ev == ENC_CCW) {
			/**
			 * Obs!! For printf must enable semihosting:
			 * Set to RedLink (-semihosting) in:
			 *  proj properties > C/C++ Build > Settings > MCU Linker >Managed Linker Script > Library
			 *
			 */
			printf("CCW step\n");
		}
		if (ev == ENC_CW) {
			printf("CW step\n");
		}
	}
}
