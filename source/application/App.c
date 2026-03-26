/***************************************************************************/ /**
   @file     App.c
   @brief    Application functions
   @author   Nicolás Magliola
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../../SDK/CMSIS/fsl_device_registers.h"
#include "../drivers/MCAL/include/gpio.h"
#include "../drivers/MCAL/include/pisr.h"
#include "include/board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void redLed_PISR(void);
void blueLed_PISR(void);
/*******************************************************************************
 *******************************************************************************
						GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
/* interrupts are disabled at this point*/
void App_Init(void) {
	gpioMode(PIN_LED_BLUE, OUTPUT);
	gpioMode(PIN_LED_RED, OUTPUT);

	gpioWrite(PIN_LED_BLUE, !LED_ACTIVE);
	gpioWrite(PIN_LED_RED, !LED_ACTIVE);

	// configure periodic leds
	pisrRegister((pisr_callback_t) redLed_PISR, 4); // every half a second

	pisrRegister((pisr_callback_t) blueLed_PISR, 8); // every second
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {
	// nothing
}

/*******************************************************************************
 *************************************************************/

void redLed_PISR(void) {
	gpioToggle(PIN_LED_RED);
}
void blueLed_PISR(void) {
	gpioToggle(PIN_LED_BLUE);
}

/*******************************************************************************
 ******************************************************************************/
