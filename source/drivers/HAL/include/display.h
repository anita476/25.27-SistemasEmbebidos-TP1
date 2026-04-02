#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include "font.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DIG_NUM 4
#define MAX_WORD_LEN 254

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
void display_drv_write_to_digit(uint8_t dig, uint8_t code);

/**
 * @brief Write word to display in digit starting with digit 0
 * @param new_word New word to write
 * @param size Amount of characters in word
 * @note When word is larger than DIG_NUM, the display scrolls the word horizontally
 */

void display_drv_write_word(uint8_t *new_word, uint8_t size);

#endif /* _DISPLAY_H_ */
