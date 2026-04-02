#include "include/display.h"
#include "../MCAL/include/gpio.h"
#include "../MCAL/include/pisr.h"
#include "include/board.h"
#include "include/shift_register.h"

#define DISPLAY_TICKS 5
#define SCROLL_TICKS 30

// padding is so that we start blank, then the scroll, and then theres 4 blank digits until restarting the scroll, it
// looks better
static uint8_t word_buf[DIG_NUM + MAX_WORD_LEN + DIG_NUM];
static uint8_t word_len;
static uint8_t scroll_offset; // current start index
static uint8_t scroll_total;
static uint8_t scroll_tick; // counts PISR calls between scroll steps, max is SCROLL_TICKS
static bool scrolling;

static uint8_t visible[DIG_NUM]; //  bytes currently on screen -> pisr multiplexes over these
static uint8_t curr;

void display_drv_PISR();
static void rebuild_visible();

bool display_drv_init() {
	curr = 0;
	scroll_offset = 0;
	scroll_tick = 0;
	scrolling = false;
	word_len = 0;

	for (int i = 0; i < DIG_NUM; i++)
		visible[i] = 0x00;

	pisr_drv_register(display_drv_PISR, DISPLAY_TICKS);
	shift_register_drv_init();
	return true;
}

void display_drv_write_word(uint8_t *new_word, uint8_t size) {
	if (size > MAX_WORD_LEN)
		size = MAX_WORD_LEN;
	word_len = size;

	// build the buffer
	for (int i = 0; i < DIG_NUM; i++)
		word_buf[i] = 0x00;
	for (int i = 0; i < size; i++)
		word_buf[DIG_NUM + i] = new_word[i];
	for (int i = 0; i < DIG_NUM; i++)
		word_buf[DIG_NUM + size + i] = 0x00;

	scroll_offset = 0;
	scroll_tick = 0;

	if (size > DIG_NUM) {
		scroll_total = DIG_NUM + size; // number of steps until word fully loads
		scrolling = true;
	} else {
		scroll_offset = DIG_NUM;
		scrolling = false;
		scroll_total = 0;
	}

	rebuild_visible();
}

static void rebuild_visible() {
	for (int i = 0; i < DIG_NUM; i++)
		visible[i] = word_buf[scroll_offset + i];
}

void display_drv_write_to_digit(uint8_t dig, uint8_t code) {
	if (dig >= DIG_NUM)
		return;
	shift_register_drv_set_digit_segments(code, dig);
}

void display_drv_PISR() {
	// advance scroll
	if (scrolling) {
		scroll_tick++;
		if (scroll_tick >= SCROLL_TICKS) {
			scroll_tick = 0;
			scroll_offset++;

			if (scroll_offset > scroll_total) {
				scroll_offset = 0; // loop back to start
			}

			rebuild_visible();
		}
	}

	// one digit per PISR call
	curr = (curr + 1u) & (DIG_NUM - 1u);
	display_drv_write_to_digit(curr, visible[curr]);
}