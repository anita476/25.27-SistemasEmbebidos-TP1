#include "include/display.h"
#include "../MCAL/include/gpio.h"
#include "include/MC74HC139.h"
#include "include/board.h"

static uint32_t digits[DIG_NUM] = {MC74HC139_Y0, MC74HC139_Y1, MC74HC139_Y2, MC74HC139_Y3};

bool displayInit() {
	// Initialize all digits
	for (int i = 0; i < DISPLAY_PINS; i++) {
		gpioMode(display_seg_pins[i], OUTPUT);
		gpioWrite(display_seg_pins[i], !DISPLAY_SEG_ACTIVE);
	}
	return MC74HC139Init(MC74HC139_DEV_U1A);
}

// Todo should have a "set word" function or smth, also missing pisr to refresh
/**
 * @brief Write a symbol to the corresponding display digit
 * @param dig Digit to write symbol to
 * @param code Symbol code (defined in font.h)
 */
void writeToDigit(uint32_t dig, uint8_t code) {
	if (dig >= DIG_NUM)
		return;
	for (int i = 0; i < DISPLAY_PINS; i++) {
		gpioWrite(display_seg_pins[i], !DISPLAY_SEG_ACTIVE);
	}
	MC74HC139Select(MC74HC139_DEV_U1A, digits[dig]);
	for (int i = 0; i < DISPLAY_PINS; i++) {
		gpioWrite(display_seg_pins[i], (code >> i) & 1);
	}
}