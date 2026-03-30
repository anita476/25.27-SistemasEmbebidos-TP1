#ifndef _MC74HC139_H_
#define _MC74HC139_H_
#include <stdbool.h>
/***************************************************************************/ /**
   @file     MC74HC139.h
   @brief    Selection MC74HC139 decoder driver.
   @note     Note that the outputs are all are active on low
  ******************************************************************************/
/**
 *
 * TRUTH TABLE
 *  OE tied to GND >  always enabled? @todo ask what this means
 *
 *   Output  |  A (pin_A)  |  B (pin_B)
 *  ---------+-------------+-----------
 *    Y0     |      0      |     0             > 0
 *    Y1     |      0      |     1             > 1
 *    Y2     |      1      |     0             > 2
 *    Y3     |      1      |     1             > 3
 *
 * B is the LSB, A is MSB
 *
 */
typedef enum {
	MC74HC139_Y0 = 0,
	MC74HC139_Y1 = 1,
	MC74HC139_Y2 = 2,
	MC74HC139_Y3 = 3,
	MC74HC139_NONE = 4
} MC74HC139_Out_t;

// using enum so app doesnt need to concern with pins @todo ask if/how to do switches in the same way
typedef enum {
	MC74HC139_DEV_U1A = 0,	/*  digit selector */
	MC74HC139_DEV_U1B = 1,	/*  LED selector */
	MC74HC139_DEV_COUNT = 2 // for array
} MC74HC139_Dec_t;

/**
 * @brief Initialize the decoder instance (sets A and B to LOW → Y0 selected).
 * @param MC74HC139_Dec_t Which decoder to initialize
 * @returns true is successfull, false if not
 */
bool MC74HC139_drv_init(MC74HC139_Dec_t type);

/**
 * @brief Select one of the four outputs.
 *
 * @param dev    Pointer to decoder instance
 * @param output One of the outputs (Y0 to Y3)
 */
void MC74HC139_drv_select(MC74HC139_Dec_t type, MC74HC139_Out_t output);

#endif /* _MC74HC139_H */