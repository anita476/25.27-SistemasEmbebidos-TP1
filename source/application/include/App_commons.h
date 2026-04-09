#ifndef _APP_COMMONS_H_
#define _APP_COMMONS_H_
#include "../../drivers/HAL/include/font.h"
#include "../../drivers/HAL/include/reader.h"
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
	uint8_t timer_misc_err;
	uint8_t display_intensity;
	bool operation_result;
	uint8_t card_buff[ID_LENGHT];
	uint8_t card_len;
	uint8_t pin_num; /* could b 4 or 5*/

} AppContext_t;

extern AppContext_t g_app_ctx;

#define MENU_ITEMS 3

#endif /* _APP_COMMONS_H_ */