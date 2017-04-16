#include "bsp.h"
#include "pwr.h"
#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include "tick.h"
#include "InitDevice.h"
#include <SI_EFM8BB3_Register_Enums.h>
#include "spi.h"
SI_SBIT (LED1, SFR_P1, 3);
int x = 0;
int goWrite=0;
int screenLocation=10;
int lapIt=0;
char text[10];
int debounceCount=0;
//SI_SFR(ourSF ,SFRPAGE);
SI_INTERRUPT (TIMER0_ISR, TIMER0_IRQn)
{
	goWrite=1;
	LED1 = !LED1;
	TCON_TR0 = 0;                       // Stop Timer 0
	TH0 = 00111000;           // Reset Timer0 High register
    TL0 = 10011110;            // Reset Timer0 Low register
    TCON_TR0 = 1;
	x = x + 100;
}

SI_INTERRUPT (PMATCH_ISR, PMATCH_IRQn)
{
   if(P0_B2 == 0 && lapIt==0)
   {
	   //make this the lap
	   lapIt=1;
	   //Wait(.001);
   }
   if(P0_B3 == 0)
   {
	  //make this the clear
	   x = 0;
   }

                       // Disable Port Match interrupts to
									   // prevent multiple interrupts from
									   // occurring while the switches are
									   // pressed

}

void main(void)
{
  // Initialize the device
  enter_DefaultMode_from_RESET();
  //SFRPAGE=PG2_PAGE;
  // Safety trap. Press either push button while resetting the device to
  // stall the application here.

  IE_EA = 1;

  EIP1H = 0|1|0|0|0|0|0|0;
  //set portmatch priority to high...

  DISP_Init();
  DrawScreenText("HowDoesthisWork", 0);
  //DrawScreenText("Pens MAN", 10);
  // Mainloop
  //writeALot("hey");
  while (true)
  {
	if (lapIt){
		//Wait(.01);
		SFRPAGE = 0x00;              // EIE1 on SFRPAGE 0x00
		EIE1 &= ~0x02;
		screenLocation +=10;
		text[0]='\0';
		sprintf(text,"%d",x);
		DrawScreenText(text, screenLocation);
		lapIt=0;
		IE_EA=1;
		//SFRPAGE=PG2_PAGE;
		//SFRPAGE=LEGACY_PAGE;


	}
	if (goWrite){
		text[0]='\0';
		sprintf(text,"%d",x);
		DrawScreenText(text, 10);
		goWrite=0;
		if (debounceCount == 3){
			EIE1 |=0x02;
			debounceCount = 0;
		}
		debounceCount++;
	}

	//x+=10;
    // Halt the display tick and wait for display updates to finish
    TMR3CN0 &= ~TMR3CN0_TR3__BMASK;
    while (BSP_DISP_CS == SPI_CS_ASSERT_LVL);
    TMR3CN0 |= TMR3CN0_TR3__BMASK;


  }
}
