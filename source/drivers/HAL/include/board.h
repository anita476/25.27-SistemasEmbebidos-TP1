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

#include "../../MCAL/include/gpio.h"

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

// Channel & switch pins for encoder
#define PIN_ENC_CHNA PORTNUM2PIN(PC, 5)
#define PIN_ENC_CHNB PORTNUM2PIN(PC, 3)
#define PIN_SW_ENC PORTNUM2PIN(PB, 23)

// DEnc board status leds selection pins
#define PIN_U1B_STATUS0 PORTNUM2PIN(PC, 7)
#define PIN_U1B_STATUS1 PORTNUM2PIN(PC, 0)

// DEnc board digit display selection pins
#define PIN_U1A_SEL0 PORTNUM2PIN(PC, 9)
#define PIN_U1A_SEL1 PORTNUM2PIN(PC, 8)

#define DEC_ACTIVE LOW

#define SW_ACTIVE LOW
#define SW_INPUT_TYPE INPUT_PULLUP // en realidad para sw3 no hace falta, para sw2 SI

#define DISPLAY_PINS 8
#define DISPLAY_SEG_ACTIVE HIGH
static pin_t display_seg_pins[DISPLAY_PINS] = {
	PORTNUM2PIN(PE, 24), // PIN_CSEGA
	PORTNUM2PIN(PE, 25), // PIN_CSEGB
	// miss 2
	PORTNUM2PIN(PD, 1),	 // PIN_CSEGC
	PORTNUM2PIN(PD, 3),	 // PIN_CSEGD
	PORTNUM2PIN(PD, 2),	 // PIN_CSEGE
	PORTNUM2PIN(PD, 0),	 // PIN_CSEGF
	PORTNUM2PIN(PC, 4),	 // PIN_CSEGG
	PORTNUM2PIN(PC, 12), // PIN_CSEGDP
};

#endif // _BOARD_H_
