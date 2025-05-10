/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "TouchPanel/TouchPanel.h"
#include "RIT.h"
#include "timer/timer.h"
#include "../pacman.h"
#include "../music/music.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
#define UPTICKS 1

volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;
//command to move pacman
volatile char input;
//flag to check if button int 0 has been pressed
volatile int pauseFlag = 0;
// flag to check if the music is still running
volatile int music = 1;

NOTE intro[] = {
		{pause, time_semiminima},
		
    {NOTE_B2, time_semicroma}, 
    {NOTE_B3, time_semicroma},
    {NOTE_FS3, time_semicroma}, 
    {NOTE_DS3, time_semicroma},
    {NOTE_B3, time_semicroma}, 
    {NOTE_FS3, time_croma * 1.5},
    {NOTE_DS3, time_croma}, 
    {NOTE_DS3, time_biscroma},

		{NOTE_C3, time_semicroma}, 
    {NOTE_C4, time_semicroma}, 
    {NOTE_G3, time_semicroma}, 
    {NOTE_E3, time_semicroma},
    {NOTE_C4, time_semicroma}, 
    {NOTE_G3, time_croma * 1.5}, 
    {NOTE_E3, time_croma}, 
		
    {NOTE_B2, time_semicroma}, 
    {NOTE_B3, time_semicroma},
    {NOTE_FS3, time_semicroma}, 
    {NOTE_DS3, time_semicroma},
    {NOTE_B3, time_semicroma}, 
    {NOTE_FS3, time_croma * 1.5},
    {NOTE_DS3, time_croma}, 
    {NOTE_DS3, time_biscroma},
		
    {NOTE_E3, time_biscroma}, 
    {NOTE_F3, time_biscroma},
    {NOTE_F3, time_biscroma}, 
    {NOTE_FS3, time_biscroma},
    {NOTE_G3, time_biscroma}, 
    {NOTE_G3, time_biscroma},
    {NOTE_GS3, time_biscroma}, 
    {NOTE_A3, time_semicroma},
    {NOTE_B3, time_croma}
};

void RIT_IRQHandler (void)
{			
	/* Static as its value persists between calls to the function. It is not reinitialized each time the function is executed.*/
	static uint8_t position=0;
	
	static int down = 0;
	static int left = 0;
	static int right = 0;
	static int up = 0;	
	
		if(music == 1){
		static int currentNote = 0; // Tracks the current note index
    static int ticks = 0;       // Tracks timing between notes

    if (!isNotePlaying()) // Check if no note is currently playing
    {
        ++ticks; // Increment tick randomizerFlag
        if (ticks == UPTICKS) // Check if it's time to play the next note
        {
            ticks = 0; // Reset ticks
            playNote(intro[currentNote++]); // Play the current note and move to the next
        }
    }

    if (currentNote == (sizeof(intro) / sizeof(intro[0]))) // Check if the song is finished
    {
			music = 0;   
			disable_timer(3);
			disable_timer(0);
			reset_timer(0);
			reset_timer(3);
			init_timer(0, 0, 0, 3, 0.25*25000000);
			init_timer(1, 0, 0, 3, 1*25000000);
			init_timer(2, 0, 0, 3, 0.25*25000000);
			init_timer(3, 0, 0, 0, 60*25000000);
			init_timer(3, 0, 1, 3, 0.3*25000000);
    }
	}
	/*************************JOYSTICK_DOWN***************************/
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
			/* Joytick DOWN pressed */
			down++;
			switch(down){
				case 1:
					//your code
					input = 'D';
					break;
				default:
					break;
			}
		}
		else{
				down=0;
		}

	/*************************JOYSTICK_LEFT***************************/
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
			/* Joytick LEFT pressed */
			left++;
			switch(left){
				case 1:
					//your code
					input = 'L';
					break;
				default:
					break;
			}
		}
		else{
				left=0;
		}

	/*************************JOYSTICK_RIGHT***************************/
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
			/* Joytick RIGHT pressed */
			right++;
			switch(right){
				case 1:
					//your code
					input = 'R';
					break;
				default:
					break;
			}
		}
		else{
				right=0;
		}
		
	/*************************JOYSTICK_UP***************************/
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
			/* Joytick UP pressed */
			up++;
			switch(up){
				case 1:
					//your code
					//enable_timer(0);
					input = 'U';
					break;
				default:
					break;
			}
		}
		else{
				up=0;
		}

	/*************************INT0***************************/
	if(down_0 !=0){
		down_0++;
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
			switch(down_0){
				case 2:
					if(pauseFlag == 0 && music == 0 && endGame == 0){
						disable_timer(0);
						disable_timer(1);
						disable_timer(2);
						disable_timer(3);
						GUI_Text(0, 150,(uint8_t *)"            PAUSE!            ", Black, White);
						pauseFlag = 1;
					}
					else if(pauseFlag == 1 && music == 0 && endGame == 0){
						drawMaze();
						enable_timer(1);
						enable_timer(2);
						enable_timer(0);
						enable_timer(3);
						pauseFlag = 0;
					}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	} // end INT0

	/*************************KEY1***************************/
	if(down_1 !=0){
		down_1++;
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
			switch(down_1){
				case 2:
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	} // end KEY1

	/*************************KEY2***************************/
	if(down_2 !=0){
		down_2++;
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
			switch(down_2){
				case 2:
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_2=0;		
			NVIC_EnableIRQ(EINT2_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	} // end KEY2
	
	/*************************ADC***************************/	
	//ADC_start_conversion();	
	//LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */

	reset_RIT();
	LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */

	return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
