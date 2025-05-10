/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../pacman.h"
#include "../CAN/CAN.h"
#include <stdio.h>


/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern unsigned char led_value;					/* defined in funct_led								*/
extern int countdown;
// counter to check if the frightened mode has ended
int timerBlinky = 10;
// string used to print time in the GUItext
char time_string[3];
//counter to avoid to compute the path every tick for blinky
int numMoves = 0;
//wait three second before respawning blinky
int respawn = 3;

uint16_t SinTable[45] =                    
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};
/*************************DAC***************************/
/*
SinTable: array with 45 elements

void TIMER0_IRQHandler (void)
{
	static int ticks=0;
	//DAC management
  LPC_DAC->DACR = (SinTable[ticks]<<6); //write the value in the conversion
	ticks++;
	if(ticks==45) ticks=0;

  LPC_TIM0->IR = 1;			// clear interrupt flag
  return;
}
*/

void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1) // MR0
	{ 
		// your code
		if(music == 1){
			static int sineticks=0;
			static int currentValue; 
			currentValue = SinTable[sineticks];
			currentValue -= 410;
			currentValue /= 1;
			currentValue += 410;
			LPC_DAC->DACR = currentValue <<6;
			sineticks++;
			if(sineticks==45){
				sineticks=0;
			}
		}
		else{
			movePacman();
		}
		LPC_TIM0->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM0->IR & 2){ // MR1
		// your code	
		LPC_TIM0->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		// your code	
		LPC_TIM0->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		// your code	
		LPC_TIM0->IR = 8;			// clear interrupt flag 
	}
  return;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	
	if(LPC_TIM1->IR & 1){ // MR0
		//your code
			if(blinkyMode == 1){//frightned
				timerBlinky--;
				if(timerBlinky == 0){
					blinkyMode = 0; //set again to angry
					timerBlinky = 10;
				}
			}
			if(respawnBlinky == 1){//blinky has been eaten by Pacman
				respawn--;
				if(respawn == 0){
					blinkyMode = 0;
					computePath();
					respawn = 3;
					respawnBlinky = 0;
					timerBlinky = 10;
					init_timer(2, 0, 0, 3, 0.25*25000000);
					enable_timer(2);
				}
			}
			countdown--;
//			sprintf(time_string,"%d", countdown);
//			GUI_Text(15, 20, (uint8_t *)"60", Black, Black);
//			GUI_Text(15, 20,(uint8_t *)time_string, White, Black);
			if(countdown==0){
				locBlinky = 1;
				disable_timer(2);
				disable_timer(0);
				gameOver();
			}
			LPC_TIM1->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM1->IR & 2){ // MR1
		//your code
		LPC_TIM1->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 4){ // MR2
		// your code	
		LPC_TIM1->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 8){ // MR3
		// your code	
		LPC_TIM1->IR = 8;			// clear interrupt flag 
	} 

	return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER2_IRQHandler (void)
{
	if(LPC_TIM2->IR & 1) // MR0
	{ 
		//your code
		moveBlinky();
		numMoves++;
		if (numMoves == 2) {
			computePath();
			numMoves = 0;
		}
		LPC_TIM2->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM2->IR & 2){ // MR1
		//your code
		LPC_TIM2->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 4){ // MR2
		// your code	
		LPC_TIM2->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 8){ // MR3
		// your code	
		LPC_TIM2->IR = 8;			// clear interrupt flag 
	} 

	return;
}


/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER3_IRQHandler (void)
{
	static uint16_t precScore = -1;
	static uint8_t precTime = -1;
	static uint8_t precLives = -1;
	
  if(LPC_TIM3->IR & 1) // MR0
	{ 
		//your code
		if(music == 1){
			disable_timer(0);
		}
		LPC_TIM3->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM3->IR & 2){ // MR1
		//your code
		if(score != precScore || nLives != precLives || countdown != precTime){
			precLives = nLives;
			precScore = score;
			precTime = countdown;
			CAN_TxMsg.data[0] = precTime;
			CAN_TxMsg.data[1] = precLives;
			CAN_TxMsg.data[2] = (precScore >> 8) & 0xFF;
			CAN_TxMsg.data[3] = precScore & 0xFF;
			CAN_TxMsg.len = 4;
			CAN_TxMsg.id = 2;
			CAN_TxMsg.format = STANDARD_FORMAT;
			CAN_TxMsg.type = DATA_FRAME;
			CAN_wrMsg (1, &CAN_TxMsg);   
		}
		LPC_TIM3->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 4){ // MR2
		// your code	
		LPC_TIM3->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 8){ // MR3
		// your code	
		LPC_TIM3->IR = 8;			// clear interrupt flag 
	} 

	return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/