/* Global config file */
#ifndef _CONFIG_H_
#define _CONFIH_H_

/* Timer section */
#define USE_ALARM

/* UART section */
#define USB_BAUDRATE 115200

//#define REPORT_SZ

/* LEDS section */
#define LEDP 2

#define LEDB BIT7
#define OUT1B BIT2
#define OUT2B BIT3
#define LEDS (LEDB | OUT1B | OUT2B)

/* Buttons section */
#define BTNP 2

#define CNT1B BIT0
#define CNT2B BIT1
#define BTNB BIT6
#define BTN220B BIT5

#define BTNS (CNT1B | CNT2B | BTNB | BTN220B)

/* Temperature section */
#define T_CNT 7
#define T1B BIT0
#define T1P 0
#define T2B BIT1
#define T2P 1
#define T3B BIT3
#define T3P 3
#define T4B BIT4
#define T4P 4
#define T5B BIT5
#define T5P 5
#define T6B BIT6
#define T6P 6
#define T7B BIT7
#define T7P 7
#define TS (T1B | T2B | T3B | T4B | T5B | T6B | T7B)

#define T1T ntc1523950
#define T2T ntc1523950
#define T3T ntc1033470
#define T4T ntc1033470
#define T5T ntc1033470
#define T6T ntc1033470
#define T7T ntc1033470

#define T_UP 0
#define T_DOWN 1

#endif
