#include "include/timer.h"
#include "../MCAL/include/pisr.h"
#include <stddef.h>
/***************************************************************************/ /**
   @file     timer.c
   @brief    Timer driver implementation
  ******************************************************************************/

/*
* Idea : we do (timertick - expired_at) = smth ,
because its unsigned if it wrapped around it will give a big number
(more than half, so timertick ++ than expires at  but because it was about to wrap around so expires at was small ->
this means ACTIVE) othrwise it will give a SMALL number -> expired !!

Examples :
255 - 1 = 254 -> more than half , its ACTIVE (missing 1 tick)
10 - 254 = -244 + 255 = 11 <<< half -> its expired
*/
#define TIM_TICK_HALF_MAX (UINT32_MAX / 2)
// @todo ask why volatile is necessary, so its not cached ??
static tim_tick_t volatile timer_tick; // since its uint , it wraps around if it gets to max (253 .. 254 ... 255 ... 0
									   // .. 1, etc)

typedef enum {
	TIM_FREE,
	TIM_OCCUPIED,
	TIM_ACTIVE,
	TIM_EXPIRED
} tim_state; // Occupied would be before calling timerStart on it

typedef struct {
	tim_state state;
	tim_callback_t callback;
	tim_tick_t expires_at;
	uint32_t period; // for periodics only
	uint8_t mode;

} timerType_t;

static timerType_t timers[TIMERS_MAX_CANT];

static void timerPISR();
// should register pisr
void timerInit(void) {
	for (int i = 0; i < TIMERS_MAX_CANT; i++) {
		timers[i].state = TIM_FREE;
	}
	pisrRegister((pisr_callback_t) timerPISR, 1); // on every tick ?
}

tim_id_t timerGetId(void) {
	for (int i = 0; i < TIMERS_MAX_CANT; i++) {
		if (timers[i].state == TIM_FREE) {
			timers[i].state = TIM_OCCUPIED;
			return i;
		}
	}
	return TIMER_INVALID_ID;
}

bool timerStart(tim_id_t id, tim_tick_t ticks, uint8_t mode, tim_callback_t callback) {
	if (id >= TIMERS_MAX_CANT || timers[id].state != TIM_OCCUPIED) {
		return false;
	}

	timers[id].expires_at = timer_tick + ticks;
	timers[id].mode = mode;
	timers[id].callback = callback;
	timers[id].period = ticks;
	timers[id].state = TIM_ACTIVE;
	return true;
}

void timerStop(tim_id_t id) {
	if (id >= TIMERS_MAX_CANT)
		return;
	timers[id].callback = NULL;
	timers[id].state = TIM_OCCUPIED;
}

bool timerExpired(tim_id_t id) {
	if (id >= TIMERS_MAX_CANT || timers[id].state != TIM_EXPIRED)
		return false;
	if (timers[id].mode == TIM_MODE_PERIODIC) {
		timers[id].expires_at += timers[id].period; // reload
		timers[id].state = TIM_ACTIVE;
	} else {
		timers[id].state = TIM_OCCUPIED;
	}
	return true;
}

// Obs!! since update is called from application, so if callbacks are long it doesnt hold up ISR
void timerUpdate(void) {
	for (int i = 0; i < TIMERS_MAX_CANT; i++) {
		if (timers[i].state != TIM_ACTIVE)
			continue;

		bool expired = (tim_tick_t) (timer_tick - timers[i].expires_at) < TIM_TICK_HALF_MAX;
		if (!expired)
			continue;

		if (timers[i].callback) {
			timers[i].callback();
			if (timers[i].mode == TIM_MODE_PERIODIC) {
				timers[i].expires_at += timers[i].period; // drift-free
			} else {
				timers[i].state = TIM_EXPIRED;
			}
		} else {
			//  timerExpired() handles reload
			timers[i].state = TIM_EXPIRED;
		}
	}
}

void timerDelete(tim_id_t id) {
	if (id >= TIMERS_MAX_CANT)
		return;
	timers[id].callback = NULL;
	timers[id].state = TIM_FREE;
}

static void timerPISR() {
	timer_tick++;
}
