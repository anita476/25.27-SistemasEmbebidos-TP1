#include "include/display.h"
#include "../MCAL/include/gpio.h"
#include "../MCAL/include/pisr.h"
#include "include/board.h"
#include "include/shift_register.h"

#define DISPLAY_TICKS 5 // todo change later to a variable value, add counter in pisr, etc

static uint8_t word[DIG_NUM];
uint8_t curr;

void display_drv_PISR();
bool display_drv_init() {
	curr = 0;
	for (int i = 0; i < DIG_NUM; i++) {
		word[i] = 0x00;
	}
	pisr_drv_register(display_drv_PISR, DISPLAY_TICKS);
	shift_register_drv_init();
}

// for now, only 4 letter words
void display_drv_write_word(uint8_t new_word[DIG_NUM]) {
	for (int i = 0; i < DIG_NUM; i++) {
		word[i] = new_word[i]; // this is horrible, change later
	}
}

void display_drv_write_to_digit(uint8_t dig, uint8_t code) {
	if (dig >= DIG_NUM)
		return;
	shift_register_drv_set_segments(code);
	shift_register_drv_sel_digit(dig);
}

void display_drv_PISR() {
	curr = (curr + 1u) & (DIG_NUM - 1u);
	display_drv_write_to_digit(curr, word[curr]);
}
