/* ###################################################################
**     Filename    : Events.c
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
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "gpio1.h"
#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : hwTim1_OnTimeOut
**     Description : This callback is called when the timer expires.
**     Parameters  :
**       data - User parameter for the callback function.
**     Returns : Nothing
** ===================================================================
*/
void hwTim1_OnTimeOut(void* data)
{
  /* Write your code here ... */
	GPIO_DRV_TogglePinOutput( PTE19 );
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
