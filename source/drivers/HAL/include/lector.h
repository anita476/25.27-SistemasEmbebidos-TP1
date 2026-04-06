#ifndef _LECTOR_H_
#define _LECTOR_H_
#include <stdbool.h>
#include <stdint.h>


void card_Enable_ISR(void);
void card_Clock_ISR(void);

// Macros

#define ID_LENGHT 8U

// ISO/IEC 7811-6 Coded Characters

#define CODED_0 0b10000
#define CODED_1 0b00001
#define CODED_2 0b00010
#define CODED_3 0b10011

#define CODED_4 0b00100
#define CODED_5 0b10101
#define CODED_6 0b10110
#define CODED_7 0b00111

#define CODED_8 0b01000
#define CODED_9 0b11001
#define CODED_A 0b11010      // Symbolizes :
#define CODED_B 0b01011      // Symbolizes ;

#define CODED_C 0b11100      // Symbolizes <
#define CODED_D 0b01101      // Symbolizes =
#define CODED_E 0b01110      // Symbolizes >
#define CODED_F 0b11111      // Symbolizes ?

const uint8_t Codes[] = {
    CODED_0, CODED_1, CODED_2, CODED_3, 
    CODED_4, CODED_5, CODED_6, CODED_7, 
    CODED_8, CODED_9, CODED_A, CODED_B, 
    CODED_C, CODED_D, CODED_E, CODED_F
};