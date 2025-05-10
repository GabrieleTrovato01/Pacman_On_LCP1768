/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC17xx with MCB1700
 * Note(s): see also http://www.port.de/engl/canprod/sv_req_form.html
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <LPC17xx.h>                  /* LPC17xx definitions */
#include "CAN.h"                      /* LPC17xx CAN adaption layer */
#include "../GLCD/GLCD.h"
#include <stdio.h>
#include "../pacman.h"

extern uint8_t icr ; 										//icr and result must be global in order to work with both real and simulated landtiger.
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */                                

volatile uint8_t valTime = 0;
volatile uint8_t valLives = 0;
volatile uint16_t valScore = 0;
char tmp_time[3];
char tmp_score[8];

/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
void CAN_IRQHandler (void)  {

  /* check CAN controller 1 */
	icr = 0;
  icr = (LPC_CAN1->ICR | icr) & 0xFF;               /* clear interrupts */
	
  if (icr & (1 << 0)) {                          		/* CAN Controller #1 meassage is received */
		CAN_rdMsg (1, &CAN_RxMsg);	                		/* Read the message */
    LPC_CAN1->CMR = (1 << 2);                    		/* Release receive buffer */
		
		// do nothing here
  }
	if (icr & (1 << 1)) {                         /* CAN Controller #1 meassage is transmitted */
		// do nothing in this example
		
//		valTime = CAN_TxMsg.data[0];
//		valLives = CAN_TxMsg.data[1];
//		valScore = (CAN_TxMsg.data[2] << 8);
//		valScore = valScore | CAN_TxMsg.data[3];
//		
//		//Time
//		sprintf(tmp_time,"%d", valTime);
//		GUI_Text(47, 5, (uint8_t *)"60", Black, Black);
//		GUI_Text(47, 5,(uint8_t *)tmp_time, White, Black);
//		
//		//Lives
//		int i;
//		for(i=0; i<valLives; i++){
//			drawCircle(20+(i*12), 300, 5, Black);
//			drawCircle(20+(i*12), 300, 5, Yellow);
//			if(i == 4){
//				break;
//			}
//		}
//		
//		//Score
//		sprintf(tmp_score,"%d", valScore);
//    GUI_Text(141, 20, (uint8_t *)tmp_score, Black, Black);
//		GUI_Text(141, 20, (uint8_t *)tmp_score, Black, Black);
//		GUI_Text(141, 20, (uint8_t *)tmp_score, White, Black);
	}
		
	/* check CAN controller 2 */
	icr = 0;
	icr = (LPC_CAN2->ICR | icr) & 0xFF;             /* clear interrupts */

	if (icr & (1 << 0)) {                          	/* CAN Controller #2 meassage is received */
		CAN_rdMsg (2, &CAN_RxMsg);	                		/* Read the message */
    LPC_CAN2->CMR = (1 << 2);                    		/* Release receive buffer */
		
		valTime = CAN_RxMsg.data[0];
		valLives = CAN_RxMsg.data[1];
		valScore = (CAN_RxMsg.data[2] << 8);
		valScore = valScore | CAN_RxMsg.data[3];
		
		//Time
		sprintf(tmp_time,"%d", valTime);
		GUI_Text(15, 20, (uint8_t *)"60", Black, Black);
		GUI_Text(15, 20,(uint8_t *)tmp_time, White, Black);
		
		//Lives
		int i;
		for(i=0; i<valLives; i++){
			drawCircle(20+(i*12), 300, 5, Black);
			drawCircle(20+(i*12), 300, 5, Yellow);
			if(i == 4){
				break;
			}
		}
		
		//Score
		sprintf(tmp_score,"%d", valScore);
		GUI_Text(141, 20, (uint8_t *)tmp_score, Black, Black);
		GUI_Text(141, 20, (uint8_t *)tmp_score, Black, Black);
		GUI_Text(141, 20, (uint8_t *)tmp_score, White, Black);
	};
		
	if (icr & (1 << 1)) {                         /* CAN Controller #2 meassage is transmitted */
		// do nothing in this example
	}
}
