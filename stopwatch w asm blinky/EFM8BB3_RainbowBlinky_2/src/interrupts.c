/*
 * interrupts.c
 *
 *  Created on: Apr 14, 2017
 *      Author: Richard
 */

//=========================================================
// src/Interrupts.c: generated by Hardware Configurator
//
// This file will be regenerated when saving a document.
// leave the sections inside the "$[...]" comment tags alone
// or they will be overwritten!
//=========================================================

// USER INCLUDES
#include <SI_EFM8BB3_Register_Enums.h>
//SI_SBIT (LED1, SFR_P1, 3);
//-----------------------------------------------------------------------------
// TIMER2_ISR
//-----------------------------------------------------------------------------
//
// TIMER2 ISR Content goes here. Remember to clear flag bits:
// TMR2CN0::TF2H (Timer # High Byte Overflow Flag)
// TMR2CN0::TF2L (Timer # Low Byte Overflow Flag)
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// PMATCH_ISR
//-----------------------------------------------------------------------------
//
// PMATCH ISR Content goes here. Remember to clear flag bits:

//
//-----------------------------------------------------------------------------
/*SI_INTERRUPT (PMATCH_ISR, PMATCH_IRQn)
{

   }


SI_INTERRUPT (TIMER0_ISR, TIMER0_IRQn)
{
	LED1 = !LED1;
	TCON_TR0 = 0;                       // Stop Timer 0
	TH0 = 00111000;           // Reset Timer0 High register
    TL0 = 10011110;            // Reset Timer0 Low register
	TCON_TR0 = 1;
}
*/
