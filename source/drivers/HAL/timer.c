/* ── timer.c ──────────────────────────────────────────────────────── */
#include "include/timer.h"
#include "../MCAL/include/pisr.h"
#include <stddef.h>

#define TIM_TICK_HALF_MAX (UINT32_MAX / 2)

static volatile tim_tick_t timer_tick;

typedef enum {
	TIM_FREE,
	TIM_OCCUPIED,
	TIM_ACTIVE,
	TIM_EXPIRED,
} tim_state_t;

typedef struct {
	tim_state_t state;
	tim_callback_t callback;
	tim_tick_t expires_at;
	uint32_t period;
	uint8_t mode;
} timerType_t;

static timerType_t timers[TIMERS_MAX_CANT];

static void timerPISR(void);

void timerInit(void) {
	for (int i = 0; i < TIMERS_MAX_CANT; i++)
		timers[i].state = TIM_FREE;

	pisrRegister(timerPISR, 1); /* no cast needed          */
}

tim_id_t timerGetId(void) {
	for (int i = 0; i < TIMERS_MAX_CANT; i++) {
		if (timers[i].state == TIM_FREE) {
			timers[i].state = TIM_OCCUPIED;
			return (tim_id_t) i;
		}
	}
	return TIMER_INVALID_ID;
}

bool timerStart(tim_id_t id, tim_tick_t ticks, uint8_t mode, tim_callback_t callback) {
	if (id >= TIMERS_MAX_CANT)
		return false;

	/* allow restart from OCCUPIED or ACTIVE (e.g. state transitions) */
	if (timers[id].state != TIM_OCCUPIED && timers[id].state != TIM_ACTIVE)
		return false;

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

	/* reset to OCCUPIED — caller must timerStart again to reuse      */
	timers[id].state = TIM_OCCUPIED;
	return true;
}

void timerUpdate(void) {
	for (int i = 0; i < TIMERS_MAX_CANT; i++) {
		if (timers[i].state != TIM_ACTIVE)
			continue;

		bool expired = (tim_tick_t) (timer_tick - timers[i].expires_at) < TIM_TICK_HALF_MAX;
		if (!expired)
			continue;

		timers[i].state = TIM_EXPIRED; /* mark first, then fire   */

		if (timers[i].callback) {
			timers[i].callback();
			if (timers[i].mode == TIM_MODE_PERIODIC) {
				timers[i].expires_at += timers[i].period; /* drift-free reload */
				timers[i].state = TIM_ACTIVE;
			}
		} else if (timers[i].mode == TIM_MODE_PERIODIC) {
			/* no callback, still reload for polling via timerExpired */
			timers[i].expires_at += timers[i].period;
			timers[i].state = TIM_ACTIVE;
		}
	}
}

void timerDelete(tim_id_t id) {
	if (id >= TIMERS_MAX_CANT || timers[id].state == TIM_FREE)
		return;
	timers[id].callback = NULL;
	timers[id].state = TIM_FREE;
}

static void timerPISR(void) {
	timer_tick++;
}