#include "include/MC74HC139.h"
#include "../MCAL/include/gpio.h"
#include "include/board.h"
/***************************************************************************/ /**
   @file     MC74HC139.c
   @brief    Selection MC74HC139 decoder driver implementation.
   @note     Note that the outputs are all are active on low
  ******************************************************************************/
typedef struct {
	bool initialized;
	pin_t pinA;
	pin_t pinB;
} Decoder_t;

static Decoder_t decoders[MC74HC139_DEV_COUNT];

bool MC74HC139Init(MC74HC139_Dec_t type) {
	switch (type) {
		case MC74HC139_DEV_U1A:
			decoders[MC74HC139_DEV_U1A].pinA = PIN_U1A_SEL0;
			decoders[MC74HC139_DEV_U1A].pinB = PIN_U1A_SEL1;
			gpioMode(PIN_U1A_SEL1, OUTPUT);
			gpioMode(PIN_U1A_SEL0, OUTPUT);
			gpioWrite(PIN_U1A_SEL0, !DEC_ACTIVE);
			gpioWrite(PIN_U1A_SEL1, !DEC_ACTIVE);
			decoders[MC74HC139_DEV_U1A].initialized = true;
			return true;
		case MC74HC139_DEV_U1B:
			decoders[MC74HC139_DEV_U1B].pinA = PIN_U1B_STATUS0;
			decoders[MC74HC139_DEV_U1B].pinB = PIN_U1B_STATUS1;
			gpioMode(PIN_U1B_STATUS0, OUTPUT);
			gpioMode(PIN_U1B_STATUS1, OUTPUT);
			gpioWrite(PIN_U1B_STATUS0, !DEC_ACTIVE);
			gpioWrite(PIN_U1B_STATUS1, !DEC_ACTIVE);
			decoders[MC74HC139_DEV_U1B].initialized = true;

			return true;
		default:
			return false;
	}
}

void MC74HC139Select(MC74HC139_Dec_t type, MC74HC139_Out_t output) {
	if (type >= MC74HC139_DEV_COUNT || !decoders[type].initialized)
		return;

	uint8_t a = (output >> 1) & 0x01; // bit1 is  STATUS0
	uint8_t b = (output >> 0) & 0x01;
	gpioWrite(decoders[type].pinA, a);
	gpioWrite(decoders[type].pinB, b);
}
