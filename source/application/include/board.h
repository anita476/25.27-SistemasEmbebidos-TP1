/***************************************************************************/ /**
   @file     board.h
   @brief    Board management
   @author   Nicolás Magliola
  ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "../../drivers/MCAL/include/gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

// On Board User LEDs
#define PIN_LED_RED PORTNUM2PIN(PB, 22)	  // PTB22
#define PIN_LED_GREEN PORTNUM2PIN(PE, 26) // PTE26
#define PIN_LED_BLUE PORTNUM2PIN(PB, 21)  // PTB21
#define LED_ACTIVE LOW

// On Board User Switches
#define PIN_SW2 PORTNUM2PIN(PC, 6) // PTC6
#define PIN_SW3 PORTNUM2PIN(PA, 4) // PTA4

// Channel pins for encoder
#define PIN_ENC_CHNA PORTNUM2PIN(PC, 5)
#define PIN_ENC_CHNB PORTNUM2PIN(PC, 3)

// Encoder board leds
// #define PIN_ENC_LED1
// #define PIN_ENC_LED2

#define SW_ACTIVE LOW
#define SW_INPUT_TYPE INPUT_PULLUP // en realidad para sw3 no hace falta, para sw2 SI

/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
