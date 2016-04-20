#ifndef _HAVE_ECU_H_
#define _HAVE_ECU_H_

typedef enum {
	ECU_STATE_SYNC,
	ECU_STATE_REF,
	ECU_STATE_BDC,
	ECU_STATE_ERROR,
} ECU_SyncState_t;



void ECU_VRS0_Event();
void ECU_VRS1_Event();
void ECU_Timer_Event();

#endif // _HAVE_ECU_H_
