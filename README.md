# Pacman_On_LCP1768

Pacman Game for LPC1768 Microcontroller
This project implements a Pacman game for the LPC1768 microcontroller (ARM Cortex-M3) with an LCD display. The game features classic Pacman mechanics with a maze, collectible pills, power-ups, and a ghost enemy (Blinky) that uses the A* pathfinding algorithm for intelligent movement.

Game Overview
The game consists of navigating Pacman through a maze to collect all pills while avoiding the ghost enemy Blinky. The player has a limited number of lives and a countdown timer. The game includes features such as:

A 30x40 maze displayed on a 240x320 pixel LCD screen
Player-controlled Pacman using directional inputs
AI-controlled Blinky ghost with different behavior modes
Collectible regular pills and power pills
Teleportation portals on opposite sides of the maze
Score tracking system with bonus lives
Timer-based gameplay (60-second countdown)
CAN communication for displaying game statistics
Technical Implementation
Main Components
Game Board (lib_pacman.c):

A 2D character matrix (maze[ROWS][COLUMNS]) representing the game state
Each cell contains a character representing walls ('='), empty spaces (' '), pills ('.'), power pills ('o'), teleporters ('<', '>'), doors ('-'), Pacman ('P'), and ghost ('G')
A Pathfinding Algorithm (a_star_lib.c)*:

Implementation of the A* algorithm for ghost movement
Includes open and closed lists, node creation, and path reconstruction
Manhattan distance heuristic for pathfinding
Dynamic target selection based on ghost mode (chase or scatter)
Game Logic (lib_pacman.c):

Collision detection and resolution
Score tracking and life management
Power pill effects (ghost becomes frightened)
Victory and game over conditions
Key Functions
Game Management
drawMaze(): Renders the game board on the LCD display
gameOver(): Handles game over state when player loses all lives
victory(): Handles victory state when all pills are collected
restart(): Resets game state after losing a life
PowerPillsRandomizer(): Randomly places power pills in the maze
Player Controls
movePacman(): Updates Pacman's position based on input direction
gameRules(): Handles game logic when Pacman collects items or collides with entities
Ghost AI
computePath(): Calculates the path for Blinky using A* algorithm
moveBlinky(): Moves Blinky along the calculated path
computeDestination(): Determines Blinky's target based on current mode
In chase mode (0): targets Pacman directly
In frightened mode (1): moves away from Pacman to a corner
A* Algorithm Components
createNode(): Creates a node with position and costs for A* algorithm
addOpenList(), addClosedList(): Manages nodes in the pathfinding process
popLowestF(): Extracts the node with lowest F value from open list
isValidPosition(): Checks if a position is valid for movement
distance(): Calculates Manhattan distance between two points
Utility Functions
drawCircle(): Renders circular objects on the LCD
drawLife(): Displays remaining lives on screen
Game Mechanics
Movement System:

Pacman moves in four directions (up, down, left, right) based on input
Blinky follows path calculated by A* algorithm
Both characters obey wall constraints and can use teleporters
Scoring System:

Regular pills: 10 points
Power pills: 50 points
Eating frightened ghost: 100 points
Extra life awarded every 1000 points
Ghost Behavior:

Default "angry" mode (0): Chases Pacman directly
"Frightened" mode (1): Activated by power pills, moves away from Pacman
Ghost respawns in the center when eaten in frightened mode
Game States:

Playing: Normal gameplay with countdown timer
Victory: All pills collected
Game Over: All lives lost
Hardware Integration:

Timer-based game loop
LCD display for rendering
CAN communication for displaying game statistics
Input handling through external interrupts
This implementation demonstrates embedded systems programming concepts, game development fundamentals, and pathfinding algorithms in a resource-constrained environment.