
#include <msp430g2553.h>
#include "config.h"
#include "boiler_sm.h"
//#include "main.h"

#include <timer.h>
//#include <uart.h>
#include <leds.h>
#include <buttons.h>
#include <thermistor.h>
//#include "display.h"

static int sm_toggle_flg;

#define D_THEMP_ON 8 //Delta for pumping ON
#define D_THEMP_OFF 7 // Delta for pumping OFF

heater_sm_t curr_state;

#if 0
display_stat_t display_state;

void sm_loop(void)
{
	static unsigned long old_jiffies;

	if (old_jiffies != jiffies)
	{
		themps_update();
		uart_report();
		old_jiffies = jiffies;
		display_stat(display_state++);
		if (display_state >= DISP_LAST)
			display_state = DISP_NONE;
	}
	display_pulse();
}
#endif

// #define DEMO
#ifdef DEMO

#define IDLE_MIN_TIME  0, 20
#define PUMPING_MIN_TIME  0, 5
#define IDLE_MAX_TIME    0, 30
#define PUMPING_MAX_TIME 0, 60

#else
#define IDLE_MIN_TIME  0, 60
#define PUMPING_MIN_TIME  0, 5
#define IDLE_MAX_TIME   24,0
#define PUMPING_MAX_TIME 0,600
#endif

void sm_toggle(void)
{
	sm_toggle_flg = 1;
}

#define pump_on()				\
	do {						\
		out1_turn(1);			\
		cnt1_b = cnt2_b = 0;	\
	} while (0)

#define pump_off()				\
	do {						\
		out1_turn(0);			\
		cnt1_b = cnt2_b = 0;	\
	} while (0)

static void sm_set(heater_sm_t state)
{
	switch (state)
	{
	case SM_IDLE_MIN:
		pump_off();
		timer_set_alarm(IDLE_MIN_TIME);
		break;

	case SM_IDLE:
		timer_set_alarm(IDLE_MAX_TIME);
		break;

	case SM_PUMPING_MIN:
		pump_on();
		timer_set_alarm(PUMPING_MIN_TIME);
		break;

	case SM_PUMPING:
		timer_set_alarm(PUMPING_MAX_TIME);
		break;

	case SM_CLEANING:
		pump_on();
		timer_set_alarm(PUMPING_MAX_TIME);
		break;
	}
	curr_state = state;
}

void sm_init(void)
{
	sm_toggle_flg = 0;
	sm_set(SM_IDLE_MIN);
}

void sm_sycle(void)
{
	if (master_btn())
		sm_toggle();

	if (sm_toggle_flg)
	{
		sm_toggle_flg = 0;
		switch (curr_state)
		{
		case SM_IDLE_MIN:
		case SM_IDLE:
			sm_set(SM_PUMPING_MIN);
			break;
		default:
			sm_set(SM_IDLE_MIN);
			break;
		}
	}

	switch (curr_state)
	{
	case SM_IDLE_MIN:
		if (alarm())
			sm_set(SM_IDLE);
		break;

	case SM_IDLE:
		if (themp_delta_get() >= D_THEMP_ON)
			sm_set(SM_PUMPING_MIN);
		else if (alarm())
			sm_set(SM_CLEANING);
		break;

	case SM_PUMPING_MIN:
		if (alarm())
			sm_set(SM_PUMPING);
		break;

	case SM_PUMPING:
		if ((themp_delta_get() <= D_THEMP_OFF) || alarm())
			sm_set(SM_IDLE_MIN);
		break;

	case SM_CLEANING:
		if (alarm())
			sm_set(SM_IDLE_MIN);
		break;
	}

}
