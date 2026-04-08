#ifndef _APP_COMMONS_H_
#define _APP_COMMONS_H_
#include "../../drivers/HAL/include/font.h"
#include "fsm.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
	FSMState_t *current_state;
	uint8_t menu_selected;
	int retry_count;
	uint8_t timer_timeout_block;
	uint8_t timer_misc;
	uint8_t display_intensity;
	bool operation_result;
	EVENT pending_event;
} AppContext_t;

extern AppContext_t g_app_ctx;

#define MENU_ITEMS 3

#endif /* _APP_COMMONS_H_ */