#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include "font.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DIG_NUM 4

/**
 * @brief Initialize display peripheral
 */
bool display_drv_init();

// Todo should have a "set word" function or smth, also missing pisr to refresh
/**
 * @brief Write a symbol to the corresponding display digit
 * @param dig Digit to write symbol to
 * @param code Symbol code (defined in font.h)
 */
void display_drv_write_to_digit(uint32_t dig, uint8_t code);

#endif /* _DISPLAY_H_ */