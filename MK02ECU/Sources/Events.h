/* ###################################################################
**     Filename    : Events.h
**     Project     : MK02ECU
**     Processor   : MK02FN128VLF10
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-04-19, 14:00, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "fsl_device_registers.h"
#include "clockMan1.h"
#include "pin_mux.h"
#include "osa1.h"
#include "hwTim1.h"
#include "gpio1.h"

#ifdef __cplusplus
extern "C" {
#endif 


/*
** ===================================================================
**     Callback    : hwTim1_OnTimeOut
**     Description : This callback is called when the timer expires.
**     Parameters  :
**       data - User parameter for the callback function.
**     Returns : Nothing
** ===================================================================
*/
void hwTim1_OnTimeOut(void* data);

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __Events_H*/
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
