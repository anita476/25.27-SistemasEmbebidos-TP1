#include "include/switch.h"
#include "../MCAL/include/pisr.h"
#include "include/timer.h"

#define DEBOUNCE_MS 20u
#define DOUBLE_CLICK_MS 300u
#define LONG_CLICK_MS 800u

#define ISACTIVE(curr, act) (!(curr ^ act))

static swEvent queue[SW_MAX_PENDING_EVENTS];
static volatile uint8_t _head = 0;
static volatile uint8_t _tail = 0;

typedef enum {
	SW_STATE_IDLE,
	SW_STATE_DEBOUNCE,
	SW_STATE_PRESSED,
	SW_STATE_RELEASED,
	SW_STATE_WAIT_RELEASE, /*waiting for button release */
} swState_t;

typedef struct {
	swState_t state;
	tim_id_t timer;
} swContext_t;

typedef struct {
	uint8_t pin;
	ACTIVE_ON active_on;
	bool registered;
} swConfig_t;

static swConfig_t config[SW_MAX_SWS];
static swContext_t ctx[SW_MAX_SWS];
static uint8_t sw_count = 0;

static void swPushEvent(swEvent ev);
static void swProcessSwitch(uint8_t i);

sw_handle_t swRegister(uint8_t pin, ACTIVE_ON active_level, PULL pullconfig) {
	if (sw_count >= SW_MAX_SWS)
		return INVALID_SW_HANDLE;

	tim_id_t tim = timerGetId();
	if (tim == TIMER_INVALID_ID)
		return INVALID_SW_HANDLE;

	uint8_t handle = sw_count++;

	gpioMode(pin, pullconfig == PULL_DOWN ? INPUT_PULLDOWN : (pullconfig == PULL_UP ? INPUT_PULLUP : INPUT));

	config[handle].pin = pin;
	config[handle].active_on = active_level;
	config[handle].registered = true;

	ctx[handle].state = SW_STATE_IDLE;
	ctx[handle].timer = tim;

	return (sw_handle_t) handle;
}

void swUnregister(sw_handle_t handle) {
	if (handle < 0 || handle >= SW_MAX_SWS)
		return;

	timerStop(ctx[handle].timer);
	timerDelete(ctx[handle].timer); /* free the timer slot */
	config[handle].registered = false;
}

swEvent swPopEvent(void) {
	swEvent ret = {.event_type = SW_EVENT_NONE, .swPin = 0};
	if (_head == _tail)
		return ret;

	ret = queue[_tail];
	_tail = (_tail + 1u) & (SW_MAX_PENDING_EVENTS - 1u);
	return ret;
}

pisr_callback_t swPisr(void) {
	for (uint8_t i = 0; i < sw_count; i++)
		if (config[i].registered)
			swProcessSwitch(i);
}

static void swPushEvent(swEvent ev) {
	uint8_t next = (_head + 1u) & (SW_MAX_PENDING_EVENTS - 1u);
	if (next == _tail)
		return; /* queue full, drop event      */
	queue[_head] = ev;
	_head = next;
}

// @todo Uses state machine, if its too expensive we may have to do processing from main loop
static void swProcessSwitch(uint8_t i) {
	uint8_t current = gpioRead(config[i].pin);
	bool active = ISACTIVE(current, config[i].active_on);
	swContext_t *c = &ctx[i];
	swEvent ev = {.swPin = config[i].pin, .event_type = SW_EVENT_NONE};

	switch (c->state) {
		case SW_STATE_IDLE:
			if (active) {
				timerStart(c->timer, TIMER_MS2TICKS(DEBOUNCE_MS), TIM_MODE_SINGLESHOT, NULL);
				c->state = SW_STATE_DEBOUNCE;
			}
			break;

		case SW_STATE_DEBOUNCE:
			if (!active) {
				timerStop(c->timer);
				c->state = SW_STATE_IDLE;
			} else if (timerExpired(c->timer)) {
				timerStart(c->timer, TIMER_MS2TICKS(LONG_CLICK_MS), TIM_MODE_SINGLESHOT, NULL);
				c->state = SW_STATE_PRESSED;
			}
			break;

		case SW_STATE_PRESSED:
			if (!active) {
				timerStart(c->timer, TIMER_MS2TICKS(DOUBLE_CLICK_MS), TIM_MODE_SINGLESHOT, NULL);
				c->state = SW_STATE_RELEASED;
			} else if (timerExpired(c->timer)) {
				ev.event_type = SW_EVENT_LONG_CLICK;
				swPushEvent(ev);
				c->state = SW_STATE_WAIT_RELEASE;
			}
			break;

		case SW_STATE_WAIT_RELEASE: /* consume release, no event */
			if (!active)
				c->state = SW_STATE_IDLE;
			break;

		case SW_STATE_RELEASED:
			if (active) {
				timerStop(c->timer);
				ev.event_type = SW_EVENT_DOUBLE_CLICK;
				swPushEvent(ev);
				c->state = SW_STATE_IDLE;
			} else if (timerExpired(c->timer)) {
				ev.event_type = SW_EVENT_CLICK;
				swPushEvent(ev);
				c->state = SW_STATE_IDLE;
			}
			break;
	}
}