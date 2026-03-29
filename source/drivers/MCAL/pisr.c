#include "include/pisr.h"
#include "hardware.h"

static bool active; // evaluate if systick was enabled
static unsigned int ticks;

typedef struct {
	unsigned int period;
	pisr_callback_t callback;
} callback;

typedef struct {
	callback callbacks[PISR_CANT];
	int used_irqs;
	int max_period;
} pisrType;
static pisrType pIrqs;

bool pisrRegister(pisr_callback_t fun, unsigned int period) {
	if (pIrqs.used_irqs >= PISR_CANT) {
		return false;
	}
	if (!active) {
		// if it wasnt, activate
		SysTick->CTRL = 0x00;									 // reset everything
		SysTick->LOAD = (TICK_MS * PISR_FREQUENCY_HZ * 100) - 1; // Changed it to every 5ms bc encoder needs it faster
		SysTick->VAL = 0x00;									 // current value
		SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
		active = true;
	}
	pIrqs.callbacks[pIrqs.used_irqs].callback = fun;
	pIrqs.callbacks[pIrqs.used_irqs].period = period;
	if (period > pIrqs.max_period) {
		pIrqs.max_period = period;
	}
	pIrqs.used_irqs++;
	return true;
}

void SysTick_Handler(void) {
	ticks++;
	for (int i = 0; i < pIrqs.used_irqs; i++) {
		if (ticks % pIrqs.callbacks[i].period == 0) {
			pIrqs.callbacks[i].callback();
		}
	}
	if (ticks == pIrqs.max_period) {
		ticks = 0;
	}
}
