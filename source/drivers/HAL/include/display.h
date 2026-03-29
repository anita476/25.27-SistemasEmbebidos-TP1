#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "font.h"


#define DIG_NUM 4

/**
 * @brief Initialize display peripheral
 */
bool displayInit();

// Todo should have a "set word" function or smth, also missing pisr to refresh
/**
 * @brief Write a symbol to the corresponding display digit
 * @param dig Digit to write symbol to
 * @param code Symbol code (defined in font.h)
 */
void writeToDigit(uint32_t dig, uint8_t code);


#endif /* _DISPLAY_H_ */