/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           functions.h
** Last modified Date:  
** Last Version:        V1.00
** Descriptions:        
** Correlated files:    
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __FUNC_H
#define __FUNC_H

#define MAX_NODE 500

// Dimensioni del campo
#define LCD_WIDTH 240
#define LCD_HEIGHT 320
#define CELL_SIZE 8
#define ROWS (LCD_HEIGHT / CELL_SIZE) // 40
#define COLUMNS (LCD_WIDTH / CELL_SIZE) // 30

// Caratteri della mappa di gioco
#define WALL '='
#define EMPTY ' '
#define POINT '.'
#define POWER_PILL 'o'
#define TELEPORT_LEFT '<'
#define TELEPORT_RIGHT '>'
#define DOOR '-'
#define PACMAN 'P' 
#define GHOST 'G'

#include <stdint.h>

typedef struct {
    int x, y;               // Grid coordinates of the node
    int g;                  // Cost from the start node to the current node
    int h;                  // Estimated cost (heuristic) to reach the end node
    int f;                  // Total cost (g + h) for A*
    int previous_index;      // Index of the previous node in the solution
} Node;

typedef struct {
    int x, y;               // Coordinates of each step in the path
} PathStep;

extern int pacmanX;
extern int pacmanY;
extern char maze[ROWS][COLUMNS];
extern int blinkyX;
extern int blinkyY;
extern volatile int blinkyMode;
extern volatile char input;
extern int timerBlinky;
extern volatile int respawnBlinky;
extern volatile int music;
extern int endGame;
extern int locBlinky;
extern int score;
extern int nLives;
extern int countdown;
extern volatile int pauseFlag;

// IRQ_CAN variables
extern volatile uint8_t valTime;
extern volatile uint8_t valLives;
extern volatile uint16_t valScore;


// Path storage
extern PathStep path[MAX_NODE];  // Steps in the calculated path
extern int counterPath;        // Number of steps in the path
extern int actualStep;      // Current step in the path
extern char previousCell;

extern void drawLife();
extern void gameOver();
extern void drawMaze();
extern void victory();
extern void restart();
extern void PowerPillsRandomizer();
extern void drawCircle(int xpos, int ypos, int r, uint16_t Color);
extern void movePacman();
extern void gameRules(char value);
extern void moveBlinky();
extern void computePath();


#endif /* end __RIT_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
