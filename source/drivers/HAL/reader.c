#include "include/reader.h"
#include "../MCAL/include/gpio.h"
#include "include/board.h"

void card_Enable_ISR(void);
void card_Clock_ISR(void);

volatile uint8_t buffer[ID_LENGHT];
volatile int bit_count = 0;
volatile int char_count = 0;
volatile uint8_t temp_char = 0;
volatile bool reading_active = false;

/********************** READER FUNCTIONS */
bool reader_drv_init(void) {
	// FIRST initializze gpios
	gpio_drv_mode(PIN_CARD_DATA, INPUT); // @todo change all to input pullup if all are high on idle
	gpio_drv_mode(PIN_CARD_CLOCK, INPUT);
	gpio_drv_mode(PIN_CARD_ENABLE, INPUT);

	bool int1 = gpio_drv_IRQ(PIN_CARD_ENABLE, GPIO_IRQ_MODE_FALLING_EDGE, card_Enable_ISR);
	bool int2 = gpio_drv_IRQ(PIN_CARD_CLOCK, GPIO_IRQ_MODE_RISING_EDGE, card_Clock_ISR);

	if (int1 == false || int2 == false) {
		return false;
	}
	return true;
}

bool reader_drv_event(void) {
	return false;
}

char *reader_drv_card(void) {
	return NULL;
}

void card_Enable_ISR(void) {
	bit_count = 0;
	char_count = 0;
	temp_char = 0;
	reading_active = false; // If the SS is read, flag becomes True and start writing in array
}

void card_Clock_ISR(void) {
	bool bit = !gpio_drv_read(PIN_CARD_DATA);

	if (!reading_active) {
		temp_char = (temp_char >> 1) | (bit << 4);

		// Evaluamos si la ventana actual de 5 bits coincide con el Start Sentinel
		if (temp_char == CODED_B) {
			reading_active = true;
			bit_count = 0; // Reseteamos para que el PRÓXIMO bit sea el 0 del primer char de datos
			temp_char = 0;
		}
	} else {
		// Una vez sincronizados, volvemos a la lógica de bloques de 5
		if (bit) {
			temp_char |= (1 << bit_count);
		}
		bit_count++;

		if (bit_count == 5) {
			if (char_count < ID_LENGHT) {
				buffer[char_count] = temp_char;
				char_count++;
			}
			bit_count = 0;
			temp_char = 0;
		}
	}
}
