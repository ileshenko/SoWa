
#include  <msp430g2553.h>
#include <config.h>
//#include "main.h"
#include "buttons.h"

#if BTNP == 1
#define PXDIR  P1DIR
#define PXOUT P1OUT
#elif BTNP == 2
#define PxDIR  P2DIR
#define PxREN P2REN
#define PxIE P2IE
#define PxIES P2IES
#define PxIFG P2IFG
#define PxOUT P2OUT
#define PxIN P2IN
#define PxSEL P2SEL
#define PxSEL2 P2SEL2
#else
#error define BTN port
#endif

//#define BTN_TOGGLE (1<<3)

#ifdef CNT1B
unsigned int cnt1_a, cnt1_b;
#endif

#ifdef CNT2B
unsigned int cnt2_a, cnt2_b;
#endif

static char master_btn_f;

#ifdef BTN220B
static char btn220_f;
#endif

void buttons_init(void)
{
	PxSEL &= ~BTNS;									// switch to GPIO mode
	PxSEL2 &= ~BTNS;								// switch to GPIO mode

	PxDIR &= ~BTNS;									// Set as Input
	PxREN |= BTNS;									// Poll Up/Down Resistor enable
	PxOUT |= BTNS;									// Poll Up
	PxIE |= BTNS;									// Interrupt Enabled
	PxIES |= BTNS;									// Hi/Lo edge
	PxIFG &= ~BTNS;									// IFG cleared

#if 0
	P1OUT =  BTN_TOGGLE; // ????? May be cleear PxDIR instead
	P1REN |= BTN_TOGGLE; 							// Poll UP resistor
	P1IE |= BTN_TOGGLE;                             // interrupt enabled
	P1IES |= BTN_TOGGLE;                            // Hi/lo edge
	P1IFG &= ~BTN_TOGGLE;                           // IFG cleared
#endif
	master_btn_f = 0;
	btn220_f = 0;
}

int master_btn(void)
{
	int ret = master_btn_f;

	master_btn_f = 0;
	return ret || !(PxIN & BTNB);
}

#ifdef BTN220B
int btn220(void)
{
	if (btn220_f)
		btn220_f--;
	return (btn220_f > 5) || !(PxIN & BTN220B);
}
#endif

//void sm_toggle(void);

// Port X interrupt service routine
#if BTNP == 1
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#elif BTNP == 2
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#endif
{
#ifdef CNT1B
	if (PxIFG & CNT1B)
	{
		cnt1_a++;
		cnt1_b +=2;
		PxIFG &= ~CNT1B;
	}
#endif
#ifdef CNT2B
	if (PxIFG & CNT2B)
	{
		cnt2_a++;
		cnt2_b +=2;
		PxIFG &= ~CNT2B;
	}
#endif

	if (PxIFG & BTNB)
	{
		master_btn_f++;
		PxIFG &= ~BTNB;
	}

#ifdef BTN220B
	if (PxIFG & BTN220B)
	{
		if ( btn220_f < 10 )
		btn220_f++;
		PxIFG &= ~BTN220B;
	}
#endif

//	sm_toggle();
//	P1OUT ^= 0x01;
}
