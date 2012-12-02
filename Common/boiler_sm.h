#ifndef _BOILER_SM_H_
#define _BOILER_SM_H_

typedef enum {
	SM_IDLE_MIN = 0,
	SM_IDLE = 1,
	SM_PUMPING_MIN = 2,
	SM_PUMPING = 3,
	SM_CLEANING = 4
} heater_sm_t;

extern heater_sm_t curr_state;

void sm_init(void);
void sm_sycle(void);

#endif /*_BOILER_SM_H_*/
