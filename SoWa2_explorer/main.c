/******************************************************************************
 *  Sun Heater controller - explorer
 * 
 * inputs
 *  Temperature:
 *   P1.0 ADC - T1 Upper sensor
 *   P1.1 ADC - T2 Lower sensor
 *   P1.3 ADC - T3 statistic
 *   P1.4 ADC - T4 statistic
 *   P1.5 ADC - T5 statistic
 *   P1.6 ADC - T6 statistic
 *   P1.7 ADC - T7 statistic
 *
 *	Flow:
 *	 P2.0 GPIO - CNT1 direct flow
 *	 P2.1 GPIO - CNT2 reverse flow
 *
 *	Button
 *	 P2.7 GPIO - BTN - TBD
 *	 P2.5 GPIO - 220V
 * 
 * outputs
 *  LEDS
 * 	 P2.6 PIN 18 - LED
 *
 * 	Relays
 *   P2.2 - OUT1 - pump motor
 *   P2.3 - OUT2 - heating
 *
 *  UART
 *   P1.2 UART_TX
 * 
 * 
 ******************************************************************************/

#include <msp430g2553.h>
#include "config.h"
#include "main.h"

#include <timer.h>
#include <uart.h>
#include <leds.h>
#include <buttons.h>
#include <thermistor.h>
#include <boiler_sm.h>
//#include "display.h"

#if 0
#define D_THEMP_ON 18 //Delta for pumping ON
#define D_THEMP_OFF 16 // Delta for pumping OFF

//#define DEMO
#ifdef DEMO

#define IDLE_MIN_TIME  0, 20
#define PUMPING_MIN_TIME  0, 5
#define IDLE_MAX_TIME    0, 30
#define PUMPING_MAX_TIME 0, 60

#else 
#define IDLE_MIN_TIME  0, 60
#define PUMPING_MIN_TIME  0, 1
#define IDLE_MAX_TIME   24,0
#define PUMPING_MAX_TIME 0,600
#endif

heater_sm_t curr_state;
display_stat_t display_state;
static void sm_set(heater_sm_t state)
{
	switch (state)
	{
		case SM_IDLE_MIN:
			led_off(LED_PUMP);
			timer_set_alarm(IDLE_MIN_TIME);
			break;
			
		case SM_IDLE:
			timer_set_alarm(IDLE_MAX_TIME);
			break;
			
		case SM_PUMPING_MIN:
			led_on(LED_PUMP);
			timer_set_alarm(PUMPING_MIN_TIME);
			break;
			
		case SM_PUMPING:
			timer_set_alarm(PUMPING_MAX_TIME);
			break;
			
		case SM_CLEANING:
			led_on(LED_PUMP);
			timer_set_alarm(PUMPING_MAX_TIME);
			break;
	}
	curr_state = state;
}

static int sm_toggle_flg;

void sm_toggle(void)
{
	sm_toggle_flg = 1;
}

void sm_init(void)
{
	sm_toggle_flg = 0;
	sm_set(SM_IDLE_MIN);
}

void sm_loop(void)
{
	static unsigned long old_jiffies;
	
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

static char *log(void)
{
	static char report[128];
	char *buf = report;
	unsigned int cnt1_A, cnt2_A;
	int i;

	_DINT();
	cnt1_A = cnt1_a;
	cnt1_a = 0;
	cnt2_A = cnt2_a;
	cnt2_a = 0;
	_EINT();

	buf = cat_ul(buf, jiffies);		//jiffies

	buf = tab_ul(buf, curr_state);	//curr_state

	buf = tab_ul(buf, btn220());	//BTN220

	buf = tab_ul(buf, cnt1_A);		//CNT1 HZ
	buf = tab_ul(buf, cnt1_b);		//CNT1 ml
	buf = tab_ul(buf, cnt2_A);		//CNT2 HZ
	buf = tab_ul(buf, cnt2_b);		//CNT2 ml

	for (i = 0; i < T_CNT; i++)
		buf = tab_ul(buf, t[i]);

	buf = cat_str(buf, "\r\n");
	*buf = 0;

	return report;
}

/* Main Function*/
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

	timer_init();
	uart_init();
	leds_init();
	buttons_init();
	thermistor_init();
//	display_init();
	sm_init();

	leds_hello();
	_EINT();
	for (;;)
	{
//		cnt_update();
		led_toggle();
		themps_update();
		out2_turn(btn220()); /* do it before report, because second call of btn220() is unstable */
		sm_sycle();
		uart_report(log());
		timer_wait();
	}


//

//	display_state = DISP_NONE;
//

//    for (;;)
//    	sm_loop();
//
}
