/*
 * blinky.asm
 *
 *  Created on: Apr 14, 2017
 *      Author: Richard
 */

$NOMOD51
;-----------------------------------------------------------------------------
; SB1_blinky.ASM
;-----------------------------------------------------------------------------
;  TARGET MCU  :  EFM8SB1
;  DESCRIPTION :  This program illustrates how to disable the watchdog timer,
;                 configure the Crossbar, configure a port and write to a port
;                 I/O pin.
;
; 	NOTES:
;
;-----------------------------------------------------------------------------

$include (SI_EFM8BB3_Defs.inc)            ; Include register definition file.

;-----------------------------------------------------------------------------
; EQUATES
;-----------------------------------------------------------------------------

GREEN_LED   equ     P1.4                    ; Green LED: '1' is ON
RED_LED		equ		P1.6
BLUE_LED	equ		P1.5
DISP_EN     equ     P2.7                    ; Display Enable:
                                           ;   '0' is Board Contoller driven
                                           ;   '1' is EFM8 driven
;-----------------------------------------------------------------------------
; RESET and INTERRUPT VECTORS
;-----------------------------------------------------------------------------

            ; Reset Vector
   /*       cseg AT 0
            ljmp Main                     ; Locate a jump to the start of
   */                                       ; code at the reset vector.

;-----------------------------------------------------------------------------
; CODE SEGMENT
;-----------------------------------------------------------------------------


Blink       segment  CODE

            rseg     Blink                ; Switch to this code segment.
            using    0                    ; Specify register bank for the
                                          ; following program code.

Main:
            ; Disable the WDT.
            anl   PCA0MD, #NOT(040h)      ; clear Watchdog Enable bit

            ; Enable the Port I/O Crossbar
            orl   P1SKIP, #04h            ; skip LED pin in crossbar assignments
            orl   P1SKIP, #05h
            orl   P1SKIP, #06h
            mov   XBR2, #40h
            orl   P1MDOUT, #04h           ; make LED pin output push-pull
            orl   P1MDOUT, #05h
            orl   P1MDOUT, #06h
            orl   P2MDOUT, #80h           ; make DISP_EN pin output push-pull

            ; Initialize LED to OFF
            clr   GREEN_LED
            clr   RED_LED
            clr   BLUE_LED
            cpl   GREEN_LED
            cpl   RED_LED
            cpl   BLUE_LED


            ; Set Display Enable pin to Board Controller driven
            clr   DISP_EN
            ;jmp   green

            ; Simple delay loop.
Loop2:      mov   R7, #03h
Loop1:      mov   R6, #00h
Loop0:      mov   R5, #00h
            djnz  R5, $
            djnz  R6, Loop0
            djnz  R7, Loop1
            cpl   RED_LED               ; Toggle LED
Loop5:      mov   R7, #03h
Loop4:      mov   R6, #00h
Loop3:      mov   R5, #00h
            djnz  R5, $
            djnz  R6, Loop3
            djnz  R7, Loop4
            cpl   RED_LED               ; Toggle LED
Loop8:      mov   R7, #03h
Loop7:      mov   R6, #00h
Loop6:      mov   R5, #00h
            djnz  R5, $
            djnz  R6, Loop6
            djnz  R7, Loop7
            cpl   RED_LED               ; Toggle LED
Loop11:      mov   R7, #03h
Loop10:      mov   R6, #00h
Loop9:      mov   R5, #00h
            djnz  R5, $
            djnz  R6, Loop9
            djnz  R7, Loop10
            cpl   RED_LED               ; Toggle LED
Loop14:      mov   R7, #03h
Loop13:      mov   R6, #00h
Loop12:      mov   R5, #00h
            djnz  R5, $
            djnz  R6, Loop12
            djnz  R7, Loop13
            cpl   GREEN_LED
            cpl   RED_LED
Loop17:      mov   R7, #03h
Loop16:      mov   R6, #00h
Loop15:      mov   R5, #00h
            djnz  R5, $
            djnz  R6, Loop15
            djnz  R7, Loop16
            cpl   GREEN_LED
            cpl   RED_LED
Loop20:      mov   R7, #03h
Loop19:      mov   R6, #00h
Loop18:      mov   R5, #00h
            djnz  R5, $
            djnz  R6, Loop18
            djnz  R7, Loop19
            cpl   GREEN_LED
Loop23:      mov   R7, #03h
Loop22:      mov   R6, #00h
Loop21:      mov   R5, #00h
            djnz  R5, $
            djnz  R6, Loop21
            djnz  R7, Loop22
            cpl   GREEN_LED





;-----------------------------------------------------------------------------
; End of file.

END
