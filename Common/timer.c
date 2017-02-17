/*
 * Standard timer configuration
 * DCO - 1 MHz
 * MCLK - 1 MHz
 * SMCLK - 1 MHz
 * TA - 125 KHz
 */

#include "timer.h"
#include <config.h>
#include <msp430g2553.h>

unsigned long jiffies;

void timer_init(void)
{
	/* Set DCO to 1 MHz */
	DCOCTL = CALDCO_1MHZ;				//calibrate DCO
	BCSCTL1 = CALBC1_1MHZ;				//calibrate basic clock

	/* Set SMCLK and MCLK to 1MHz */
	BCSCTL2 = SELM_0 | DIVM_0 | /*SELS |*/ DIVS_0; // MCLK = DCO/1. SMCLK = DCO/1

	/* Set Timer input and configuration */
    TACTL = TASSEL_2 + ID_3 + MC_2;           // SMCLK, 125KHz, contmode

    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    CCR0 += 100;

    jiffies = 0;
}

void timer_wait(void)
{
 	_BIS_SR(LPM0_bits + GIE);
}

//----------------------------------------
#ifdef USE_ALARM

typedef enum {
	TIMER_OFF = 0,
	TIMER_RUN = 1,
	TIMER_FINISH = 2
} timer_stat_t;

typedef struct {
	unsigned char stat;
	unsigned char hours;
	unsigned int secs;
} timer_t;

timer_t timer;

int alarm(void)
{
	return timer.stat == TIMER_FINISH;
}

void timer_set_alarm(char hours, int secs )
{
	_DINT();
	timer.stat = TIMER_RUN;
	timer.hours = hours;
	timer.secs = secs;
	_EINT();
}

#endif

//-------------------------------------------
static unsigned char divider = 0;

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void main_timer(void)
{
	CCR0 += 15625; // call me 1/8 sec later
	divider++;
	if (divider & 0x7)
		return;

	jiffies++; /* HZ == 1 */

#ifdef USE_ALARM
	if (timer.stat == TIMER_RUN)
	{
		timer.secs--;
		if (!timer.secs && timer.hours)
		{
			timer.hours--;
			timer.secs = 3600;
		}
		if (!timer.secs)
			timer.stat = TIMER_FINISH;
	}
#endif
	_BIC_SR_IRQ(LPM0_bits);
}
