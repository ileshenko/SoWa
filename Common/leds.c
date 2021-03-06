#include <msp430g2553.h>
#include <config.h>
#include "leds.h"
#include "main.h"

#if LEDP == 1
#define PxDIR  P1DIR
#define PxOUT P1OUT
#elif LEDP == 2
#define PxDIR  P2DIR
#define PxOUT P2OUT
#define PxSEL P2SEL
#else
#error define LED port
#endif

void leds_init(void)
{
	PxDIR |= LEDS;			// OUT
	PxSEL &= ~LEDS;			// GPIO mode

	PxOUT &= ~LEDS;			// Switch Off
}

void led_toggle(void)
{
	PxOUT ^= LEDB;
}

#ifdef OUT1B
void out1_turn(int on)
{
	if (on)
		PxOUT |= OUT1B;
	else
		PxOUT &= ~OUT1B;
}
#endif

#ifdef OUT2B
void out2_turn(int on)
{
	if (on)
		PxOUT |= OUT2B;
	else
		PxOUT &= ~OUT2B;
}
#endif

void leds_error_indicate(void)
{
	int i;
	volatile int j;

	for (i = 0; i < 9; i++)
	{
		led_toggle();
		j = 0x7fff;
		while(--j);
	}
}

void leds_hello(void)
{
	int i;
	volatile int j;

	for (i = 0; i < 20; i++)
	{
		led_toggle();
		j = 0x3fff;
		while(--j);
	}
}
