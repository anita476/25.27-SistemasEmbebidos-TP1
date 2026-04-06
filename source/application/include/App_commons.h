#ifndef _APP_COMMONS_H_
#define _APP_COMMONS_H_
#include "fsm.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../drivers/HAL/include/font.h"

typedef struct {
    FSMState_t *current_state;
    uint8_t     menu_selected;
    int         retry_count;
    uint8_t     timer_timeout_block;
    uint8_t     timer_misc;
    uint8_t     display_intensity;
    bool        operation_result;
    EVENT       pending_event;
} AppContext_t;

extern AppContext_t g_app_ctx;

/*********************************** DISPLAY ARRAYS ****************************** */
static const uint8_t MENU_READ_CARD_MSG[] = {};
#define MENU_READ_CARD_MSG_NUM 10
static const  uint8_t MENU_MANUAL_CARD_MSG[] = {};
#define MENU_MANUAL_CARD_MSG_NUM 10
static const  uint8_t MENU_INTENSITY_MSG[] = {};
#define MENU_INTENSITY_MSG_NUM 10
static const  uint8_t SUCCESS_MSG[] = {};
#define SUCCESS_MSG_NUM 10 
static const  uint8_t FAILURE_MSG[] = {};
#define FAILURE_MSG_NUM 10
#endif /* _APP_COMMONS_H_ */