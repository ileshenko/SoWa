
#include <config.h>
#include "uart.h"
#include "timer.h"
//#include "thermistor.h"
//#include "buttons.h"
//#include "main.h"
#include <msp430g2553.h>

//char report[128];
char *to_print;
unsigned int i;

void uart_init(void)
{
#if 0
	BCSCTL1 = CALBC1_1MHZ; 		    // Set range
  	DCOCTL = CALDCO_1MHZ;  		    // Set DCO step + modulation
#endif

	/* Port 1.2 (Rx) may be overridden by ADC if we need output only */
	P1SEL |= BIT1 + BIT2; //select RXD and TXD for UART
	P1SEL2 |= BIT1 + BIT2; //select RXD and TXD for UART

	UCA0CTL0 = 0; /* UCSYNC = 0 (ASYNC), UCMODE = 0 (UART), UCSPB = 0 (one stop) ... */
	UCA0CTL1 |= UCSSEL_3; /* SMCLK (1MHz)*/

#if USB_BAUDRATE == 9600
	/* Modulation Control */
	UCA0MCTL = UCBRF_0 + UCBRS_1;

	/* Baud Rate Control */
	UCA0BR0 = 104;
	UCA0BR1 = 0;
#elif USB_BAUDRATE == 115200
	/* Modulation Control */
	UCA0MCTL = UCBRF_0 + UCBRS_6;

	/* Baud Rate Control */
	UCA0BR0 = 8;
	UCA0BR1 = 0;
#else
#error Unknow Boudrate
#endif

	UCA0CTL1 &= ~UCSWRST; //toggle SW Reset off
}

char *cat_ul(char *buf, unsigned long val)
{
	unsigned long cutoffVal = val/10;

	if (cutoffVal)
		buf = cat_ul(buf, cutoffVal);

	*buf = '0' + val%10;

	return buf+1;
}

char *cat_str(char *buf, char *str)
{
	while (*str)
		*buf++ = *str++;
	return buf;
}

void uart_report(char *line)
{
	to_print = line;
#if 0
	char *buf = report;
	unsigned int cnt1_A;

	_DINT();
	cnt1_A = cnt1_a;
	cnt1_a = 0;
	_EINT();

	buf = cat_ul(buf, jiffies);
	buf = cat_str(buf, "\t");
	buf = cat_ul(buf, cnt1_A);
	buf = cat_str(buf, "\t");
	buf = cat_ul(buf, cnt1_b);
# if 0
	buf = cat_ul(buf, curr_state);
	buf = cat_str(buf, "\t");
	buf = cat_ul(buf, temp_up);
	buf = cat_str(buf, "\t");
	buf = cat_ul(buf, temp_bottom);
	buf = cat_str(buf, "\t");
	buf = cat_ul(buf, temp_ctl);
#endif

	buf = cat_str(buf, "\r\n");
	*buf = 0;
#endif
	i = 0;
	IE2 |= UCA0TXIE;
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR (void)
{
  UCA0TXBUF = to_print[i++];				// TX next character

  if (!to_print[i])							// TX over?
    IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
}

