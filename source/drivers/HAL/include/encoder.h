/**
 * encoder.h
 *
 *  Created on: Mar 26, 2026
 *      Author: anegre
 **/

#ifndef DRIVERS_HAL_ENCODER_H_
#define DRIVERS_HAL_ENCODER_H_
#include "../..//MCAL/include/gpio.h"
#include "../../MCAL/include/pisr.h"

#define ENC_PISR_PERIOD 1 // in ticks -> @todo i think 125ms per tick is much too slow -> maybe not

#define MAX_PENDING_EVENTS 32

typedef enum : int8_t {
	ENC_CCW = -1,
	ENC_NONE = 0,
	ENC_CW = +1,
} EncoderDir;

typedef struct {
	pin_t channelA; // Obs! absolute mcu pins
	pin_t channelB;
} enc_channels;

typedef int8_t enc_step; // step is -1 or 1, or 0 if empty

// idea : we have a counter that tracks steps, so we know how many positions its been shifted.
// The counter can then be reset each time the counter is read -> ask about concurrency ?

// the idea is to use periodic polling to see if the encoder has "moved"
// ¿ideal time? 1ms? 5ms?

// idea: isr ONLY does the counting, the reading and actions based on the counter should be OUTSIDE (application)

/**
 * @brief Initialize the with the corresponding channel pins
 * @return True if successful, false if not
 **/
bool encoderInit(pin_t chnA, pin_t chnB);

/**
 * @brief Pop an event from the encoder event queue. Queue is circular with max MAX_PENDING_EVENTS, otherwise overwrites
 * @todo preguntar sobre la queue ciruclar
 * @returns 1 for a clockwise step, -1 for counter clockwise. Returns 0 if queue is empty.
 */
enc_step popEvent();

#endif /* DRIVERS_HAL_ENCODER_H_ */
