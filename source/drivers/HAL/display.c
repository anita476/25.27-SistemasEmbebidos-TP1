#include "include/display.h"
#include "../MCAL/include/gpio.h"
#include "../MCAL/include/pisr.h"
#include "include/MC74HC139.h"
#include "include/board.h"

// @todo with 4ms its still flickering, with 3 no but brightness setting would be coarser..
#define DISPLAY_TICKS 5 // todo change later to a variable value, add counter in pisr, etc
static uint32_t digits[DIG_NUM] = {MC74HC139_Y0, MC74HC139_Y1, MC74HC139_Y2, MC74HC139_Y3};
static uint8_t word[DIG_NUM];
uint8_t curr;

void display_drv_PISR();
bool display_drv_init() {
	// Initialize all digits
	for (int i = 0; i < DISPLAY_PINS; i++) {
		gpio_drv_mode(display_seg_pins[i], OUTPUT);
		gpio_drv_write(display_seg_pins[i], !DISPLAY_SEG_ACTIVE);
	}
	pisr_drv_register(display_drv_PISR, DISPLAY_TICKS);
	return MC74HC139_drv_init(MC74HC139_DEV_U1A);
}

// for now, only 4 letter words
void display_drv_write_word(uint8_t new_word[DIG_NUM]) {
	for (int i = 0; i < DIG_NUM; i++) {
		word[i] = new_word[i]; // this is horrible, change later
	}
}

void display_drv_write_to_digit(uint32_t dig, uint8_t code) {
	if (dig >= DIG_NUM)
		return;
	// for (int i = 0; i < DISPLAY_PINS; i++) {
	// gpio_drv_write(display_seg_pins[i], !DISPLAY_SEG_ACTIVE);
	//}
	MC74HC139_drv_select(MC74HC139_DEV_U1A, digits[dig]);
	for (int i = 0; i < DISPLAY_PINS; i++) {
		gpio_drv_write(display_seg_pins[i], (code >> i) & 1);
	}
}

void display_drv_PISR() {
	curr = (curr + 1u) & (DIG_NUM - 1u);
	display_drv_write_to_digit(curr, word[curr]);
}
