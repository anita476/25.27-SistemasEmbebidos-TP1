#include "include/encoder.h"
#include "../../application/include/board.h"
#include "../MCAL/include/gpio.h"
static enc_channels channel_pins; // to keep them somewhere

static EncoderDir queue[MAX_PENDING_EVENTS];
static volatile uint8_t _head = 0; // newst the ISR writes here
static volatile uint8_t _tail = 0; // oldest , the app will read from here -> 0 a MAX_PENDING -1

static uint8_t previous;

static void pushEvent(EncoderDir dir);

pisr_callback_t encPisr(void);

bool encoderInit(pin_t chnA, pin_t chnB) {
	channel_pins.channelA = chnA;
	channel_pins.channelB = chnB;
	gpioMode(chnA, INPUT_PULLUP); // theres already a pullup but oh well
	gpioMode(chnB, INPUT_PULLUP);
	_head = 0;
	_tail = 0;
	return pisrRegister((pisr_callback_t) encPisr, ENC_PISR_PERIOD);
}

enc_step popEvent() {
	if (_head == _tail) {
		return ENC_NONE;
	}
	EncoderDir dir = queue[_tail];
	_tail = (_tail + 1) & (MAX_PENDING_EVENTS - 1); // add 1 and & only if _tail is the last possible one
	return dir;
}
/**
 * Idea:
 * Two channels A and B
 * We need to maintain the current vs previous values of both.
 *
 * AB -> one uitn8
 * 00 -> nothing
 * 01 -> A is inactive, B is active
 * 10 -> A is active, B is not
 * 11 -> both are active
 *
 *
 * Obs! 4x 4 puedo hacer un array de lookup para que que pasa en cada caso de previous vs actual  -> pero solo me
 * interesa cuando pasa de 0 a algo Obs! Depende de cuantos eventos quiero por "click"
 *
 * Por ejemplo:
 * 00 + 10 -> estaaban en 0 y ahora A tocó antes que B  -> es un mov cwise
 * 00 + 01 -> estaban en 0 y ahora B tocó antes que A -> counter clockwise
 * todos los otros casos no los toco ahora ... no debería ser necesario
 *
 */

pisr_callback_t encPisr(void) {
	uint8_t current = (gpioRead(channel_pins.channelA) << 1) | gpioRead(channel_pins.channelB);
	uint8_t index = (previous << 2) | current;
	switch (index) {
		case 0b1110: //
			pushEvent(ENC_CW);
			break;
		case 0b1101:
			pushEvent(ENC_CCW);
			break;
		default:
			break;
	}

	previous = current;
}

static void pushEvent(EncoderDir dir) {
	uint8_t next = (_head + 1u) & (MAX_PENDING_EVENTS - 1u); // if full, then go to start

	if (next == _tail) // if we caught up to the tail, events were never consumed!!
		return;		   // drop events if queue is full

	queue[_head] = dir;
	_head = next;
}
