/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: 
 *		to control led11 and led 10 through EINT buttons (similarly to project 03_)
 *		to control leds9 to led4 by the timer handler (1 second - circular cycling)
 * Note(s): this version supports the LANDTIGER Emulator
 * Author: 	Paolo BERNARDI - PoliTO - last modified 15/12/2020
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/

                  
#include <stdio.h>
#include "LPC17xx.h"                    /* LPC17xx definitions                */
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "ADC/adc.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "../pacman.h"
#include "CAN/CAN.h"

#ifdef SIMULATOR
//extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

extern volatile int pauseFlag;

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {	
	
	SystemInit();  												/* System Initialization (i.e., PLL)  */
	CAN_Init();
	ADC_init();														/* ADC Inizialization */
  BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
	//init_RIT(0.025*25000000);						/* RIT Initialization 50 msec       */
	init_RIT(0x004C4B40);							    /* RIT Initialization 50 msec       */
	enable_RIT();													/* enable RIT to count 50ms				 */
	
	/******************TIMER ON**********************/
	LPC_SC -> PCONP |= (1 << 22);  // TURN ON TIMER 2
	LPC_SC -> PCONP |= (1 << 23);  // TURN ON TIMER 3	
	
	LCD_Initialization();
	TP_Init();
	
	LCD_Clear(White);
	GUI_Text(0, 150,(uint8_t *)"           Welcome!           ", White, Black);
	pauseFlag = 1;
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= 0xFFFFFFFFD;			

	LPC_PINCON->PINSEL1 |= (1<<21);				
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);					/* Set P0.26 with Output Mode */
		
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }
}