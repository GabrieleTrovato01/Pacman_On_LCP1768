/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:          lib_pacman.c
** Last modified Date:  
** Last Version:       
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#include "GLCD/GLCD.h"
#include "TouchPanel/TouchPanel.h"
#include "ADC/adc.h"
#include <stdlib.h>
#include "LPC17xx.h"
#include "timer.h"
#include <stdio.h>
#include "pacman.h"
#include "../CAN/CAN.h"

// Definition of the game map
char maze[ROWS][COLUMNS] = {
		"                              ",
		"                              ",
		"                              ",
		"                              ",
		"                              ",
		" ============================ ",
		" =............==............= ",
		" =.====.=====.==.=====.====.= ",
		" =.====.=====.==.=====.====.= ",
		" =.====.=====.==.=====.====.= ",
		" =..........................= ",
		" =.====.==.========.==.====.= ",
		" =.====.==.========.==.====.= ",
		" =......==....==....==......= ",
		" ======.===== == =====.====== ",
		"      =.===== == =====.=      ",
		"      =.==          ==.=      ",
		"      =.== ===--=== ==.=      ",
		" ======.== =      = ==.====== ",
		" <   ....  =   G  =  ....   > ",
		" ======.== =      = ==.====== ",
		"      =.== ======== ==.=      ",
		"      =.==          ==.=      ",
		"      =.== ======== ==.=      ",
		" ======.== ======== ==.====== ",
		" =............==............= ",
		" =.====.=====.==.=====.====.= ",
		" =.====.=====.==.=====.====.= ",
		" =...==....... P.......==...= ",
		" ===.==.==.========.==.==.=== ",
		" ===.==.==.========.==.==.=== ",
		" =......==....==....==......= ",
		" =.====.===== == =====.====.= ",
		" =..........................= ",
		" ============================ ",
		"                              ",
		"                              ",
};


// Variable to control the comma nd movement. 
// It is used in RIT Handler and in the movePacman function
// Possible values: U (up), D (down), L (left), R (right)
extern volatile char input; 
//Position of Pacman
int pacmanX;
int pacmanY;
//Used to display variable in the screen
char scoreString[8];
//Player's current score
int score = 0;
// Variable used to add a new life every 1000 point
int previousScore = 0;
// Counter used in drawMaze to execute PowerPillsRandomizer only once
int randomizerFlag = 0;
// Number of remaining lives
int nLives = 1;
// Timer of the game
int countdown = 60;
// Counter of pills 
int totalPills = 246;
// mode blinky (default-->angry)
volatile int blinkyMode = 0;
// Position of Blinky
int blinkyX;
int blinkyY;
// flag used to respawnBlinky
volatile int respawnBlinky = 0; 
// flag to check the end of the game	
int endGame = 0;
// flag to avoid concurrency in the moveBlinky
int locBlinky = 0;

/**
 * Handles the victory state.
 * Stops all game activity and displays the victory message.
 */
extern void victory(){
	disable_timer(2);
	drawCircle(blinkyX, blinkyY, 4, Black);
	endGame = 1;
	disable_timer(1);
	reset_timer(0);
	reset_timer(2);
	reset_timer(1);
	LCD_Clear(Yellow);
	GUI_Text(0, 155,(uint8_t *)"           VICTORY!           ", Yellow, Black);
};


extern void restart(){
	
	drawCircle(blinkyX, blinkyY, 4, Black);
	previousCell = EMPTY;
	maze[blinkyY][blinkyX] = EMPTY;
	maze[pacmanY][pacmanX] = EMPTY;
	
	//initial position
	blinkyX = 14;
	blinkyY = 19;
	pacmanX = 14;
	pacmanY = 28;
	
	maze[pacmanY][pacmanX] = PACMAN;
	maze[blinkyY][blinkyX] = GHOST;
	
	disable_timer(1);
	disable_timer(2);
	disable_timer(0);
	reset_timer(0);
	reset_timer(1);
	reset_timer(2);
	countdown = 60;
	char tmp[3];
	sprintf(tmp,"%d", countdown);
	GUI_Text(47, 5, (uint8_t *)"60", Black, Black);
	GUI_Text(47, 5,(uint8_t *)tmp, White, Black);
	
	drawMaze();
	enable_timer(0);
	enable_timer(1);
	enable_timer(2);
};

/**
 * Handles the game over state.
 * Stops all game activity and displays the game over message.
 */
extern void gameOver(){

	//disable_timer(0); (see in IRQ_Timer in Timer_1 section)
	endGame = 1;
	disable_timer(1);
	reset_timer(0);
	reset_timer(1);
	reset_timer(2);
	LCD_Clear(Red);
	GUI_Text(0, 155,(uint8_t *)"         GAME OVER!           ", Red, Black);
};

/**
 * Draws a circle on the screen at a specific position.
*/
extern void drawCircle(int xpos, int ypos, int r, uint16_t Color) {
    int i;
    int j;

    for (i = -r; i <= r; i++) {
        for (j = -r; j <= r; j++) {
            if (j * j + i * i <= r * r) {
                TP_DrawPoint(xpos + j, ypos + i, Color);
            }
        }
    }
};

/**
 * Draws the remaining lives at the bottom of the screen.
 * (In this version we use the CAN to print the remaining lives)
 */
extern void drawLife(){

//	int i;
//	for(i=0; i<nLives; i++){
//		drawCircle(20+(i*12), 300, 5, Black);
//		drawCircle(20+(i*12), 300, 5, Yellow);
//		if(i == 4){
//			break;
//		}
//	};
};

/**
 * Updates the score,
 * If Pacman eats a power pill set the scared mode of blinky and also the timer of scary blinky. 
 * Update the number of lifes,
 * and checks for victory conditions.
*/
extern void gameRules(char value){

	if(value == POINT){
		score += 10;
		totalPills--;
	}
	else if(value == POWER_PILL){
		score += 50;
		totalPills--;
		blinkyMode = 1; //scared mode
		disable_timer(2);
		reset_timer(2);
		init_timer(2, 0, 0, 3, 0.4*25000000);
		enable_timer(2);
		timerBlinky = 10;
		computePath();
	}
//	sprintf(scoreString,"%d", score);
//	GUI_Text(141, 20, (uint8_t *)scoreString, Black, Black);
//	GUI_Text(141, 20, (uint8_t *)scoreString, White, Black);
	
	if (score >= previousScore + 1000){
				nLives += 1;
				drawLife();
				previousScore += 1000;
	}
	
	if(totalPills == 0){
		victory();
	}
	else{
		enable_timer(0);
	}
};
/**
 * Used to position 6 power pills in the maze,
 * in random point of the map
*/
extern void PowerPillsRandomizer(){

	int min_x = 6;  // lower x bound
	int max_x = 36; // upper x bound
	int min_y = 2;  // lower y bound
	int max_y = 27; // upper y bound
	int PowerPills = 0;
	int rand_x = 0;
	int rand_y = 0;
	int seed ;
	
	seed = LPC_RIT->RICOUNTER;
	srand(seed); // Init seed based on current time

	while(PowerPills < 6){
		
		rand_x = min_x + rand() % (max_x - min_x + 1);
		rand_y = min_y + rand() % (max_y - min_y + 1);
		
		if(maze[rand_x][rand_y] == POINT){
			maze[rand_x][rand_y] = POWER_PILL;
			PowerPills++;
		}
	}
};

/**
* Scan the maze matrix and 
* draw the corresponding elements
* in the screen (start also to compute the path for Blinky)
*/
extern void drawMaze(){

    LCD_Clear(Black);
		if(randomizerFlag == 0) {
			PowerPillsRandomizer();
			GUI_Text(47, 5,(uint8_t *)"60", White, Black);
			GUI_Text(190, 5,(uint8_t *)"00", White, Black);
			randomizerFlag++;
		}
		else{
			GUI_Text(47, 5, (uint8_t *)"60", Black, Black);
			GUI_Text(190, 5,(uint8_t *)scoreString, White, Black);
			int i;
			for(i=0; i<valLives; i++){
				drawCircle(20+(i*12), 300, 5, Black);
				drawCircle(20+(i*12), 300, 5, Yellow);
				if(i == 4){
					break;
				}
			}
			GUI_Text(141, 20, (uint8_t *)scoreString, Black, Black);
			GUI_Text(141, 20, (uint8_t *)scoreString, White, Black);
		}
		GUI_Text(10, 5,(uint8_t *)"TIME", White, Black);
		GUI_Text(130, 5,(uint8_t *)"SCORE", White, Black);
		
		
		drawLife();
		
    int i;
    int j;
    char type;
    int xpos;
    int ypos;
    for(i=0; i<ROWS; i++){
        for(j=0; j<COLUMNS; j++){

            type = maze[i][j];
            xpos = j*CELL_SIZE;
            ypos = i*CELL_SIZE;

            switch (type){
							case DOOR:
											// Draw door
											xpos = xpos;
											ypos = ypos + 5;
											LCD_DrawLine(xpos, ypos, xpos+8, ypos, Magenta);
											break;
							case PACMAN:
											// Draw Pac-Man
											drawCircle((xpos+4), (ypos+4), 4, Yellow);
											pacmanX = j;
											pacmanY = i;
											break;
							case POINT:
											// Draw POINTs
										  drawCircle((xpos+4), (ypos+4), 1, White);
											break;
							case POWER_PILL:
											// Draw power POINTs
											drawCircle((xpos+4), (ypos+4), 3, White);
											break;
							case WALL:
											// Draw walls
											xpos = xpos + 2;
											ypos = ypos + 2;
											LCD_DrawLine(xpos, ypos, xpos+4, ypos, Blue);
											xpos = xpos + 4;
											LCD_DrawLine(xpos, ypos, xpos, ypos+4, Blue);
											ypos = ypos + 4;
											LCD_DrawLine(xpos, ypos, xpos-4, ypos, Blue);
											xpos = xpos - 4;
											LCD_DrawLine(xpos, ypos, xpos, ypos-4, Blue);
											ypos = ypos - 4;
											break;
							
							case GHOST:
										drawCircle(xpos+4, ypos+4, 4, Red);
										blinkyY = i;
										blinkyX = j;
										break;
							case EMPTY:
											break;
            }
        }
    }
		computePath();
};

/** 
* Update the PacMan position depending on the input received
* Avoid collision with walls and handles the teleport in tha map
* And also handles what happen when PacMan and Blinky are in the same cell.
*/
extern void movePacman(){
	//x and y are inverted in functions
	int x_current = pacmanX*CELL_SIZE;
	int y_current = pacmanY*CELL_SIZE;
	int x_updated;
	int y_updated;
	char value;
	
	switch(input){
	
		case 'U':
				if(maze[pacmanY-1][pacmanX] != WALL){
					
					disable_timer(0);
					x_updated = pacmanX * CELL_SIZE;
					y_updated = (pacmanY - 1) * CELL_SIZE;

					drawCircle(x_current+4, y_current+4, 4, Black);
					drawCircle((x_updated + 4), (y_updated + 4), 4, Yellow);
					value = maze[pacmanY-1][pacmanX];
					gameRules(value);
					
					maze[pacmanY][pacmanX] = EMPTY;
					maze[pacmanY - 1][pacmanX] = PACMAN;
					pacmanY--;
					//enable_timer(0);
				}
				break;
		case 'D':
				if(maze[pacmanY+1][pacmanX] != WALL)
				{
					disable_timer(0);
					x_updated = pacmanX * CELL_SIZE;
					y_updated = (pacmanY + 1) * CELL_SIZE;
					
					drawCircle(x_current+4, y_current+4, 4, Black);
					drawCircle((x_updated + 4), (y_updated + 4), 4, Yellow);
					
					value = maze[pacmanY+1][pacmanX];
					gameRules(value);
					
					maze[pacmanY][pacmanX] = EMPTY;
					maze[pacmanY + 1][pacmanX] = PACMAN;
					pacmanY++;
					//enable_timer(0);
				}
				break;
		case 'L':
            if (maze[pacmanY][pacmanX - 1] == TELEPORT_LEFT) {
                disable_timer(0);
							
                maze[pacmanY][pacmanX] = EMPTY;
                pacmanX = COLUMNS - 3; // Teleport to the right side
                x_updated = pacmanX * CELL_SIZE;
                y_updated = pacmanY * CELL_SIZE;
								// Update game map
                maze[pacmanY][pacmanX] = PACMAN;
								// Update display
								drawCircle(x_current+4, y_current+4, 4, Black);
                drawCircle(x_updated+4, y_updated+4, 4, Yellow);
								
                enable_timer(0);
            }
						else if(maze[pacmanY][pacmanX - 1] != WALL){
								disable_timer(0);
						
                x_updated = (pacmanX-1) * CELL_SIZE;
                y_updated = pacmanY * CELL_SIZE;

								// Update display
                drawCircle(x_current+4, y_current+4, 4, Black);
                drawCircle(x_updated+4, y_updated+4, 4, Yellow);
							
								value = maze[pacmanY][pacmanX-1];
								gameRules(value);
							  // Update game map
								maze[pacmanY][pacmanX] = EMPTY;
                maze[pacmanY][pacmanX - 1] = PACMAN;
                pacmanX--;
								
                //enable_timer(0);
            }
						break;
		case 'R':
				if (maze[pacmanY][pacmanX + 1] == TELEPORT_RIGHT) {
                disable_timer(0);
							
                maze[pacmanY][pacmanX] = EMPTY;
                pacmanX = 2; // Teleport to the right side
                x_updated = pacmanX * CELL_SIZE;
                y_updated = pacmanY * CELL_SIZE;
								// Update game map
                maze[pacmanY][pacmanX] = PACMAN;
								// Update display
								drawCircle(x_current+4, y_current+4, 4, Black);
                drawCircle(x_updated+4, y_updated+4, 4, Yellow);
								
                enable_timer(0);
					}
					else if(maze[pacmanY][pacmanX + 1] != WALL){
							disable_timer(0);
					

							x_updated = (pacmanX+1) * CELL_SIZE;
							y_updated = pacmanY * CELL_SIZE;
							

							// Update display
							drawCircle(x_current+4, y_current+4, 4, Black);
							drawCircle(x_updated+4, y_updated+4, 4, Yellow);
							value = maze[pacmanY][pacmanX+1];
						  gameRules(value);
						  // Update game map
							maze[pacmanY][pacmanX] = EMPTY;
							maze[pacmanY][pacmanX + 1] = PACMAN;
							pacmanX++;
							
							//enable_timer(0);
					}
				break;
		default:
				break;
			
	}
	
	if(pacmanX == blinkyX && pacmanY == blinkyY){

		if(blinkyMode == 0){//angry
			nLives--;
			if(nLives == 0){
				locBlinky = 1;
				disable_timer(0);
				disable_timer(2);
				gameOver();
			}
			else{
				drawLife();
				restart();
			}
		}
		else if(blinkyMode == 1){//frightned
			disable_timer(2);
			reset_timer(2);
			
			blinkyX = 14;
			blinkyY = 19;
			maze[blinkyY][blinkyX] = GHOST;
			previousCell = EMPTY;
			score += 100;
			//computePath();
			respawnBlinky = 1;
		}
	}
	
};

/*
* Function to move Blinky along the already computed path
*/
void moveBlinky() {
    // If the counterPath is 1, exit the function (no movement for Blinky)
		if(counterPath == 1){
			return;
		}
		// If locBlinky is already 1, do nothing and return
		if(locBlinky == 1){
			return;
		}
		// Set locBlinky to 1 to indicate Blinky is moving
		locBlinky = 1;
		 // Get the current position of Blinky
    int x_current = blinkyX * CELL_SIZE;
    int y_current = blinkyY * CELL_SIZE;
		
		// Store the previous cell type where Blinky was
		maze[blinkyY][blinkyX] = previousCell;

    // Coordinates of the next step from the path
    int x_next = path[actualStep].x * CELL_SIZE;
    int y_next = path[actualStep].y * CELL_SIZE;

    // Erase Blinky's current position
    switch (previousCell) {
        case POINT:
            x_current += 4;
            y_current += 4;
            drawCircle(x_current, y_current, 4, Black);
            LCD_DrawLine(x_current, y_current, x_current, y_current+1, White);
            x_current += 1;
            LCD_DrawLine(x_current, y_current, x_current, y_current+1, White);
            maze[blinkyY][blinkyX] = POINT;
            break;

        case POWER_PILL:
            drawCircle(x_current+4, y_current+4, 4, Black);
            drawCircle(x_current+4, y_current+4, 3, White);
            maze[blinkyY][blinkyX] = POWER_PILL;
            break;

        case DOOR:
            drawCircle(x_current+4, y_current+4, 4, Black);
            y_current += 5;
            LCD_DrawLine(x_current, y_current, x_current+8, y_current, Magenta);
            break;

        default:
            drawCircle(x_current+4, y_current+4, 4, Black);
            break;
    }
		
    // Move Blinky to the next step
    blinkyX = path[actualStep].x;
    blinkyY = path[actualStep].y;
		
		// Store the current cell as the previous one for the next step
		previousCell = maze[blinkyY][blinkyX];
		maze[blinkyY][blinkyX] = GHOST;

    // Draw Blinky in the new position
		if(blinkyMode == 0){
			drawCircle(x_next+4, y_next+4, 4, Red);
		}
		else{
			drawCircle(x_next+4, y_next+4, 4, Blue);
		}
		
   // Decrease the step count to move Blinky to the next path point in the path
    actualStep--;
		
		locBlinky = 0;
};









