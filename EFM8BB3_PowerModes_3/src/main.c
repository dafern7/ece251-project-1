//-----------------------------------------------------------------------------
// main.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
//
// Program Description:
//
// This example demonstrates the available power modes on the EFM8BB3 STK using
// the LCD and push buttons.
//
//-----------------------------------------------------------------------------
// How To Test: EFM8BB3 STK
//-----------------------------------------------------------------------------
// 1) Place the SW104 switch in "AEM" mode.
// 2) Connect the EFM8BB3 STK board to a PC using a mini USB cable.
// 3) Compile and download code to the EFM8BB3 STK board.
//    In Simplicity Studio IDE, select Run -> Debug from the menu bar,
//    click the Debug button in the quick menu, or press F11.
// 4) Run the code.
//    In Simplicity Studio IDE, select Run -> Resume from the menu bar,
//    click the Resume button in the quick menu, or press F8.
// 5) The power modes example should start.
// 6) Press PB1 to cycle through the 6 different power modes shown on
//    the LCD.
// 7) Press PB0 to enter the selected power mode. The LED turns off.
//    For all modes except Shutdown, the display will show "*Active*" while
//    in the mode. For Shutdown mode the display is turned off.
// 8) Press PB0 (all modes except Stop and Shutdown) to return to the menu,
//    or press Reset (all modes) to restart the example. The LED turns on.
//
// Target:         EFM8BB3
// Tool chain:     Generic
//
// Release 0.1 (MD;AT)
//    - Initial Revision
//    - 1 APR 2016
//
//-----------------------------------------------------------------------------
// Resources:
//-----------------------------------------------------------------------------
// SYSCLK - 24.5 MHz HFOSC0 / 1
// SPI0   - 1 MHz
// Timer2 - 2 MHz (SPI CS delay)
// Timer3 - 1 kHz (1 ms tick)
// P2.6 - Display CS (Active High)
// P0.6 - SCK
// P1.0 - MOSI
// P1.4 - LED G
// P0.2 - Push Button 0
// P0.3 - Push Button 1
// P1.7 - Joystick (THIS IS NOW USED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!)
// P3.4 - Display enable
//
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "bsp.h"
#include "pwr.h"

#include "draw.h"
#include "tick.h"
#include "InitDevice.h"




//-----------------------------------------------------------------------------
// Local Definitions
//-----------------------------------------------------------------------------

// Power mode enumeration
#define NORMAL_MODE 0
#define IDLE_MODE 1
#define STOP_MODE 2
#define SUSPEND_MODE 3
#define SNOOZE_MODE 4
#define SHUTDOWN_MODE 5
#define LAST_POWER_MODE SHUTDOWN_MODE
#define BC_DISCONNECTED 0
SI_SBIT (BC_EN, SFR_P2, 2);            // Board Controller Enable

SI_VARIABLE_SEGMENT_POINTER(powerModeText[], char, SI_SEG_CODE) =
{
  "0. Normal Mode",
  "1. Idle Mode",
  "2. Stop Mode",
  "3. Suspend Mode",
  "4. Snooze Mode",
  "5. Shutdown Mode",
};

// Power mode status
#define STATUS_MODE_ACTIVE 0
#define STATUS_MODE_WAKEUP 1

SI_VARIABLE_SEGMENT_POINTER(powerStatusText[], char, SI_SEG_CODE) =
{
  "*Active*",
  "*Wakeup*",
};

// Function that demonstrates the available power modes
//void APP_enterPowerMode(uint8_t mode);


//-----------------------------------------------------------------------------
// User Interface Functions
//-----------------------------------------------------------------------------

// Push button assignments
#define UI_BTN_MENU   (1 << 0)
#define UI_BTN_SELECT (1 << 1)

// Read and return push button status
uint8_t UI_getButtons(void)
{
  uint8_t status = 0;
  if (BSP_PB1 == BSP_PB_PRESSED) {
    status = UI_BTN_MENU;
  } else if (BSP_PB0 == BSP_PB_PRESSED) {
    status = UI_BTN_SELECT;
  }
  return status;
}

// Manage the UI menu and return the user selection
uint8_t UI_getPowerMode(uint8_t mode)
{
  // Initialize the display and draw the UI
  DISP_Init();
  DrawSplashScreen();
  DrawScreenText(powerModeText[mode], TEXT_ROW0_Y);

  // Menu selection loop
  while (true)
  {
    // Wait for button press leading edge
    while (UI_getButtons() != 0);
    while (UI_getButtons() == 0);

    // Cycle menu through available modes
    if (UI_getButtons() & UI_BTN_MENU)
    {
      mode++;
      if (mode > LAST_POWER_MODE)
      {
        mode = NORMAL_MODE;
      }
      DrawScreenText(powerModeText[mode], TEXT_ROW0_Y);
    }
    // Selection made, break out of menu loop
    else if (UI_getButtons() & UI_BTN_SELECT)
    {
      break;
    }
  }

  // Wait for all buttons to be released
  while (UI_getButtons() != 0);

  // Display mode status, turn off the LED and return the selection
  DrawScreenText(powerStatusText[STATUS_MODE_ACTIVE], TEXT_ROW1_Y);
  BSP_LED_G = BSP_LED_OFF;
  return mode;
}

// Disables the LCD display
void UI_disableDisplay(void)
{
  // Clear the display screen
  DISP_ClearAll();
  // Wait for BSP to complete all display updates
  while (BSP_DISP_CS == SPI_CS_ASSERT_LVL);
  // Disable the display
  BSP_DISP_EN = DISP_EN_BC;
}

// Port Match ISR - Triggered on leading edge of UI_BTN_SELECT.

int previousflag;
int button_flag = 0;
int flag2 = 0;
extern void changedisplay(void); //prototype
 SI_INTERRUPT (PMATCH_ISR, PMATCH_IRQn)
{
  // Turn on LED whenever UI_BTN_SELECT is pressed
  //BSP_LED_G = BSP_LED_ON;
	 button_flag = -1*button_flag;
	 flag2 = -1 * flag2;
}












#define JOYSTICK_MV_C          3       ///< center position in mV
#define JOYSTICK_MV_N          2831    ///< north position in mV
#define JOYSTICK_MV_E          2533    ///< east position in mV
#define JOYSTICK_MV_S          1650    ///< south position in mV
#define JOYSTICK_MV_W          1980    ///< west position in mV

#define JOYSTICK_MV_ERROR      150

#define JOYSTICK_NONE          0       ///< not pressed
#define JOYSTICK_C             1       ///< center
#define JOYSTICK_N             2       ///< north
#define JOYSTICK_E             3       ///< east
#define JOYSTICK_S             4       ///< south
#define JOYSTICK_W             5       ///< west


 uint8_t JOYSTICK_convert_mv_to_direction(uint32_t mV);












char var[8]= {0,0,0,0,0,0,0,0};
int tester;
char array2[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char array1[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int flagX;
int flagY;
int startrender;
int endrender;
char testarr[5];
char temps[16];
char wholebuf[30][16];
uint8_t dir;
uint8_t dir_flag = 0;
int linewidth=0;
int linelength=0;
int m;
int l;
int lineCount = 0;
char temp[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int k = 0;
int j;
int counter = 0;
int c = 0;
int i = 0;
char testChar;
#define UART_BUFFERSIZE        64
int write_flag=0;


SI_INTERRUPT (ADC0EOC_ISR, ADC0EOC_IRQn)
{
  uint32_t mV;
  P1_B4 = !P1_B4;

  ADC0CN0_ADINT = 0; // Clear ADC0 conv. complete flag

  mV = (ADC0 * (uint32_t) 3300) / 1023;

  dir = JOYSTICK_convert_mv_to_direction(mV);

}



//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
uint8_t UART_Buffer[UART_BUFFERSIZE];
uint8_t UART_Buffer_Size = 0;
uint8_t UART_Input_First = 0;
uint8_t UART_Output_First = 0;
uint8_t TX_Ready =1;
uint8_t Byte = 0;

//-----------------------------------------------------------------------------
// UART0_ISR
//-----------------------------------------------------------------------------
//
// UART0 ISR Content goes here. Remember to clear flag bits:
// SCON0::RI (Receive Interrupt Flag)
// SCON0::TI (Transmit Interrupt Flag)
//
// This routine is invoked whenever a character is entered or displayed on the
// Hyperterminal.
//
//-----------------------------------------------------------------------------


SI_INTERRUPT (UART0_ISR, UART0_IRQn)
{
	   if (SCON0_RI == 1)
	   {
	      if( UART_Buffer_Size == 0)  {    // If new word is entered
	         UART_Input_First = 0;    }

	      SCON0_RI = 0;                          // Clear interrupt flag

	      Byte = SBUF0;                    // Read a character from UART



	      /*//Individual strings approach
	      if ((linelength == 0) && (linewidth < 16)){
	    	  array1[linewidth] = Byte;
	    	  linewidth++;
	      }else{
	    	  linelength++;
	    	  linewidth = 0;
	      }

	      if((linelength == 1) && (linewidth<16)){
	    	  array2[linewidth] = Byte;
	    	  linewidth++;
	      }else{
	    	  linelength++;
	          linewidth = 0;
	      }
          */



	      if (linewidth<16){
        	 wholebuf[linelength][linewidth] = Byte;
        	// array1[linewidth] = Byte;
        	 linewidth++;
        	// wholebuf[0][0] = Byte;

         }
         else{
        	 linelength++;
        	 linewidth = 0;
        	 wholebuf[linelength][linewidth] = Byte;
        	 linewidth++;
         }



	      if (UART_Buffer_Size < UART_BUFFERSIZE)
	      {
	         UART_Buffer[UART_Input_First] = Byte; // Store in array

	         UART_Buffer_Size++;           // Update array's size

	         UART_Input_First++;           // Update counter
	      }

	      write_flag = 1;

	   }
	   //Byte = 0;
}

//-----------------------------------------------------------------------------
// Example main()
//-----------------------------------------------------------------------------
void main(void)
{
    uint8_t modeSelected = NORMAL_MODE;
    BC_EN = BC_DISCONNECTED;
    linelength = 0;
    linewidth = 0;

  // Initialize the device
  enter_DefaultMode_from_RESET();

  // Safety trap. Press either push button while resetting the device to
  // stall the application here.
  //while (UI_getButtons());

  // Start with the LED on
  BSP_LED_G = BSP_LED_ON;

  // Enable all interrupts
  IE_EA = 1;
  for (j = 0; i<128; i++){
	  DrawScreenText("                             ", i); //clear whole LCD
  }

  //DrawScreenText("NORTH WAS PRESSED", 50);
  // Mainloop

  testarr[0] = 0;
  testarr[1] = 0;
  testarr[2] = 0;
  testarr[3] = 0;
  testarr[4] = 0;
  for(l=0 ;  l<30 ; l++){
	  for (c = 0; c<16; c++){
		  wholebuf[l][c] = 0;
	      temp[c] = 0;
	      temps[c] = 0;
	      button_flag = -1;
	      flag2 = -1;
	  }
  }
  previousflag = flag2;

  //wholebuf[0][3] = 66;
  while (true)
  {


          /*
    	  for(flagY = 0; flagY<15; flagY++){
        	  for(flagX = 0; flagX < 2; flagX++){
        		  sprintf(&temps, "0x%x 0x%x 0x%x", wholebuf[linelength-14+flagY][0], wholebuf[linelength-14+flagY][1],
        				  wholebuf[linelength-14+flagY][2]);
        		  DrawScreenText(temps, flagY*8);
        	  }
        	  */


        	      /*for(flagX = 0; flagX<16; flagX++){
          			  sprintf(temps, "0x%x",wholebuf[linelength-14+flagY][flagX]);
          			  temp[flagX*2] = temp[0];
          			  temp[flagX*2+1] = temp[1];
          			  //DrawScreenText(temp, flagY*8);
          			  DrawScreenText(temps,flagY*8);
          		  }*/




	  //DrawScreenText("this is a test", 56);
	  //temps = 93;
	  //DrawScreenText(temps, 40);
	  if (dir == JOYSTICK_N){
		  P1_B4 = !P1_B4;
		  DrawScreenText("NORTH WAS PRESSED", 50);
	  }
	  /*

	   else if (dir == JOYSTICK_W){
		  DrawScreenText("WEST WAS PRESSED", 50);
	  }
	  else if (dir == JOYSTICK_S){
	  		  DrawScreenText("SOUTH WAS PRESSED", 50);
	  	  }
	  else if (dir == JOYSTICK_E){
	  		  DrawScreenText("EAST WAS PRESSED", 50);
	  	  }*/

    // Get the user's selection
    //modeSelected = UI_getPowerMode(modeSelected);
	  //DrawScreenText(testarr, 48);

	  if(flag2 != previousflag){
    	  changedisplay();
    	  previousflag = flag2;
      }
	  if(write_flag == 1){
		  write_flag = 0;
	  for(k = 0; k<15; k++){
		 for(i = 0; i<16; i++){
			 if(button_flag == 1){
				  sprintf(&temps[i],"%x",wholebuf[linelength-14+k][i]);
				  DrawScreenText(temps, k*8);
			  }else{
				  temp[i] = wholebuf[linelength-14+k][i];
				  DrawScreenText(temp, k*8);
	          }
	  	  }
	  }
	  }








	       }//END OF WHILE LOOP


} //END OF MAIN()



uint8_t JOYSTICK_convert_mv_to_direction(uint32_t mV)
 {
	 uint8_t joystickDirection;

     // determine which direction pad was pressed
     if ((mV <= JOYSTICK_MV_C + JOYSTICK_MV_ERROR))
     {
       joystickDirection = JOYSTICK_C;
     }
     else if ((mV >= JOYSTICK_MV_N - JOYSTICK_MV_ERROR) && \
              (mV <= JOYSTICK_MV_N + JOYSTICK_MV_ERROR))
     {
       joystickDirection = JOYSTICK_N;
     }
     else if ((mV >= JOYSTICK_MV_E - JOYSTICK_MV_ERROR) && \
              (mV <= JOYSTICK_MV_E + JOYSTICK_MV_ERROR))
     {
       joystickDirection = JOYSTICK_E;
     }
     else if ((mV >= JOYSTICK_MV_S - JOYSTICK_MV_ERROR) && \
              (mV <= JOYSTICK_MV_S + JOYSTICK_MV_ERROR))
     {
       joystickDirection = JOYSTICK_S;
     }
     else if ((mV >= JOYSTICK_MV_W - JOYSTICK_MV_ERROR) && \
              (mV <= JOYSTICK_MV_W + JOYSTICK_MV_ERROR))
     {
       joystickDirection = JOYSTICK_W;
     }
     else
       {
         joystickDirection = JOYSTICK_NONE;
       }

       return joystickDirection;
}


void changedisplay(void){
	if(flag2 == 1){
		for(k = 0; k<15; k++){
			  	  for(i = 0; i<16; i++){

					  	  sprintf(&temps[i],"%x",wholebuf[linelength-14+k][i]);
					  	  DrawScreenText(temps, k*8);

			  }
		}
	}
	else if(flag2 ==-1){
		for(k = 0; k<15; k++){
		 	  for(i = 0; i<16; i++){
		 		  temp[i] = wholebuf[linelength-14+k][i];
		 		  DrawScreenText(temp, k*8);
			  }
		}

	}






	//flag2 = flag2 * -1;

}




    // Turn off the display for Shutdown Mode
   // if (modeSelected == SHUTDOWN_MODE)
    //{
      //UI_disableDisplay();
    //}

    // Halt the display tick and wait for display updates to finish
    //TMR3CN0 &= ~TMR3CN0_TR3__BMASK;
    //while (BSP_DISP_CS == SPI_CS_ASSERT_LVL);

    // Change all input pins to analog to disable internal pullups
    //P0MDIN = 0x44;
    //P1MDIN = 0x11;
    //P2MDIN = 0x40;
    //P3MDIN = 0x10;

    // Enter the selected power mode
    //APP_enterPowerMode(modeSelected);






    // Restore all pins to digital mode for normal operation
    //P3MDIN = 0x9F;
    //P2MDIN = 0x7F;
    //P1MDIN = 0xFF;
    //P0MDIN = 0xFF;

    // Restart the display update tick
   // TMR3CN0 |= TMR3CN0_TR3__BMASK;




//-----------------------------------------------------------------------------
// Demonstrate the available power modes
//-----------------------------------------------------------------------------
void APP_enterPowerMode(uint8_t mode)
{
  // Clear the PMU wake flags
  uint8_t wake_flags = PWR_readAndClearWakeFlags();

  // Start the selected power mode
  switch (mode)
  {
    case NORMAL_MODE:
      // Wait for button press on PB0
      while (BSP_PB0 != BSP_PB_PRESSED);
      break;

    case IDLE_MODE:
      // Interrupts must be enabled to return from Idle mode
      PWR_enterIdle();
      break;

    case STOP_MODE:
      // Disable VMON0 to see datasheet current (VMON0 adds ~15 uA)
      PWR_enterStop();
      break;

    case SUSPEND_MODE:
      // Disable VMON0 to see datasheet current (VMON0 adds ~15 uA)
      // Enter Suspend mode with port match previously enabled as the wake source
      PWR_enterSuspend();
      wake_flags = PWR_readAndClearWakeFlags();
      break;

    case SNOOZE_MODE:
      // Disable VMON0 to see datasheet current (VMON0 adds ~15 uA)
      // Enter Snooze mode with port match previously enabled as the wake source
      PWR_enterSnooze();
      wake_flags = PWR_readAndClearWakeFlags();
      break;

    case SHUTDOWN_MODE:
      // Disable VMON0 to see datasheet current (VMON0 adds ~15 uA)
      PWR_enterShutdown();
      break;
  }
}
