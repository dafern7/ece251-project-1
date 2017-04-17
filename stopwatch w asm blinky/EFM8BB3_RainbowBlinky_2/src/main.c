#include "bsp.h"
#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include "tick.h"
#include "InitDevice.h"
#include <SI_EFM8BB3_Register_Enums.h>
#include "spi.h"
SI_SBIT(LED1, SFR_P1, 3);



int lap=0;
int x = 0;
int count=0;
//interrupts &timer
SI_INTERRUPT (TIMER0_ISR, TIMER0_IRQn)
{
	count=1;
	LED1 = !LED1;
	TCON_TR0 = 0;             // timercontrol
	TH0 = 00111100;           // change time speed
    TL0 = 11001000;
    TCON_TR0 = 1;
	x = x + 1;
}




SI_INTERRUPT (PMATCH_ISR, PMATCH_IRQn)
{
   if(P0_B2 == 0 && lap==0)
   {
	   lap=1;
   }


}

int screenLocation=10;
char text[10];
int multilap=0;
void main(void)
{

  enter_DefaultMode_from_RESET();
  IE_EA = 1;
  DISP_Init();
  DrawScreenText("HowDoesthisWork", 0);

  while (true)
  {
	if (lap){

		SFRPAGE = 0x00;              // EIE1 interrupt on SFRPAGE 0x00
		EIE1 &= ~0x02;
		screenLocation +=10;
		text[0]='\0';
		sprintf(text,"%d",x);		//print to string
		DrawScreenText(text, screenLocation);
		lap=0;
		IE_EA=1;

	}


	if (count){
		text[0]='\0';
		sprintf(text,"%d",x);
		DrawScreenText(text, 10);
		count=0;

		if (multilap == 2){		//enables multiple laps
			EIE1 |=0x02;
			multilap = 0;
		}
		multilap++;
	}


    TMR3CN0 &= ~TMR3CN0_TR3__BMASK;
    while (BSP_DISP_CS == SPI_CS_ASSERT_LVL);
    TMR3CN0 |= TMR3CN0_TR3__BMASK;


  }
}
