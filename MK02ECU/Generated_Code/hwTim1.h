/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : hwTim1.h
**     Project     : MK02ECU
**     Processor   : MK02FN128VLF10
**     Component   : fsl_hwtimer
**     Version     : Component 1.3.0, Driver 01.00, CPU db: 3.00.000
**     Repository  : KSDK 1.3.0
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-04-19, 14:03, # CodeGen: 1
**     Contents    :
**         HWTIMER_SYS_Init             - _hwtimer_error_code_t HWTIMER_SYS_Init(hwtimer_t * hwtimer,const...
**         HWTIMER_SYS_Deinit           - _hwtimer_error_code_t HWTIMER_SYS_Deinit(hwtimer_t * hwtimer);
**         HWTIMER_SYS_SetPeriod        - _hwtimer_error_code_t HWTIMER_SYS_SetPeriod(hwtimer_t * hwtimer,uint32_t...
**         HWTIMER_SYS_GetPeriod        - uint32_t HWTIMER_SYS_GetPeriod(hwtimer_t * hwtimer);
**         HWTIMER_SYS_Start            - _hwtimer_error_code_t HWTIMER_SYS_Start(hwtimer_t * hwtimer);
**         HWTIMER_SYS_Stop             - _hwtimer_error_code_t HWTIMER_SYS_Stop(hwtimer_t * hwtimer);
**         HWTIMER_SYS_GetModulo        - uint32_t HWTIMER_SYS_GetModulo(hwtimer_t * hwtimer);
**         HWTIMER_SYS_GetTime          - _hwtimer_error_code_t HWTIMER_SYS_GetTime(hwtimer_t * hwtimer,hwtimer_time_t...
**         HWTIMER_SYS_GetTicks         - uint32_t HWTIMER_SYS_GetTicks(hwtimer_t * hwtimer);
**         HWTIMER_SYS_RegisterCallback - _hwtimer_error_code_t HWTIMER_SYS_RegisterCallback(hwtimer_t *...
**         HWTIMER_SYS_BlockCallback    - _hwtimer_error_code_t HWTIMER_SYS_BlockCallback(hwtimer_t * hwtimer);
**         HWTIMER_SYS_UnblockCallback  - _hwtimer_error_code_t HWTIMER_SYS_UnblockCallback(hwtimer_t * hwtimer);
**         HWTIMER_SYS_CancelCallback   - _hwtimer_error_code_t HWTIMER_SYS_CancelCallback(hwtimer_t * hwtimer);
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file hwTim1.h
** @version 01.00
*/         
/*!
**  @addtogroup hwTim1_module hwTim1 module documentation
**  @{
*/         
#ifndef __hwTim1_H
#define __hwTim1_H

/* MODULE hwTim1. */
/* Include inherited beans */
#include "clockMan1.h"

#include "Cpu.h"
#include "fsl_hwtimer.h"
#include "fsl_hwtimer_pit.h"

/** Hw timer structure handle structure. Generated due to enabled auto initialization */
extern hwtimer_t hwTim1_Handle;
    
#define FSL_HWTIM1_LL_CALLBACK_DATA    NULL
    
#define FSL_HWTIM1_LL_DEVIF            kPitDevif     
#define FSL_HWTIM1_LL_ID               0U
  
/* hwTim1_InitConfig0 */ 
#define FSL_HWTIM1_PERIOD_CNF0         1000000U
/*! @brief Interrupt service routines used by the component driver */
void PIT0_IRQHandler(void);
    
/* END hwTim1. */

#endif 
/* ifndef __hwTim1_H */
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