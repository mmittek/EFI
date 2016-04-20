#include <stdint.h>
#include "ECU.h"
#include "gpio.h"


const uint32_t SYNC_BTDC_DEG = 103;
const int ECU_coilDwellTicks = 10;
// 1 2 4 3
uint32_t ECU_ignitionTiming[] = {90, 270, 630, 450};
uint32_t ECU_coilPins[] = {COIL1, COIL1, COIL1, COIL1};
const int ECU_cylinders = 4;



volatile uint32_t VRS0_events = 0;
volatile uint32_t VRS1_events = 0;
volatile ECU_SyncState_t syncState = ECU_STATE_ERROR;
volatile uint32_t ECU_cycleTimeTicks = 0;
volatile uint32_t ECU_ticksPerRotation = 0;

volatile int syncs = 0;

void ECU_Timer_Event() {
	int i;
	ECU_cycleTimeTicks++;

	for(i=0; i<ECU_cylinders; i++) {
		// COIL DWELL START

if(ECU_cycleTimeTicks == (ECU_ignitionTiming[i]*ECU_ticksPerRotation )/360-ECU_coilDwellTicks) {
	GPIO_DRV_SetPinOutput(ECU_coilPins[i]);

}
if(ECU_cycleTimeTicks == (ECU_ignitionTiming[i]*ECU_ticksPerRotation )/360) {
	GPIO_DRV_ClearPinOutput(ECU_coilPins[i]);

}

		/*
		if(ECU_cycleTimeUs == (ECU_ignitionTiming[i]*ECU_usPerRotation)/360-ECU_coilDwellUs) {
			GPIO_DRV_SetPinOutput(ECU_coilPins[i]);
		}
		if(ECU_cycleTimeUs == (ECU_ignitionTiming[i]*ECU_usPerRotation)/360) {
			GPIO_DRV_ClearPinOutput(ECU_coilPins[i]);
		}
		*/
	}

}

void ECU_VRS0_Event() {
	VRS0_events++;
}


void ECU_VRS1_Event() {
	VRS1_events++;

	switch(VRS0_events) {
		case 2:
			syncState = ECU_STATE_REF;
		break;

		case 10:
			syncState = ECU_STATE_BDC;
		break;

		case 12:
			syncState = ECU_STATE_SYNC;
			ECU_ticksPerRotation = ECU_cycleTimeTicks/2;
			ECU_cycleTimeTicks = 0;
		break;

		default:
			syncState = ECU_STATE_ERROR;
	}
	VRS0_events = 0;
}
