#ifndef _BUTTONS_H_
#define _BUTTONS_H_

void buttons_init(void);
int master_btn(void);

//extern char btn_f;

#ifdef CNT1B
extern unsigned int cnt1_a, cnt1_b;
#endif
#ifdef CNT2B
extern unsigned int cnt2_a, cnt2_b;
#endif
#ifdef BTN220B
//extern char btn220_f;
int btn220(void);
#endif

#endif
