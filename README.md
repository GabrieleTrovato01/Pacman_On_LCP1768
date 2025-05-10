# Pacman_On_LCP1768

Collecting workspace informationFiltering to most relevant information# How the Pacman Game on LPC1768 Works

This project is a full implementation of a Pacman game running on an LPC1768 microcontroller with an ARM Cortex-M3 processor. The game features a classic maze-based gameplay where the player controls Pacman while avoiding or sometimes chasing a ghost enemy (Blinky).

## Game Mechanics

### Core Gameplay Elements
- **Countdown Timer**: Players have 60 seconds to collect as many pills as possible
- **Lives System**: Players start with 1 life and can earn additional lives
- **Score System**:
  - Regular pills: 10 points
  - Power pills: 50 points
  - Eating ghost: 100 points
  - Extra life awarded every 1000 points
- **Victory Condition**: Collect all pills within the time limit
- **Game Over Condition**: Lose all lives or time runs out

### Character Behaviors
1. **Pacman Movement**:
   - Controlled using directional inputs (Up, Down, Left, Right)
   - Cannot pass through walls
   - Can use teleport tunnels to move between sides of the maze
   - Collects pills and power pills

2. **Blinky (Ghost) AI**:
   - Uses A* pathfinding algorithm for intelligent movement
   - Has two behavior modes:
     - **Chase Mode (default)**: Directly pursues Pacman
     - **Frightened Mode**: Activated when Pacman eats a power pill, runs away to corners
   - Returns to the center of the maze when eaten in frightened mode

## Technical Implementation

### Game State Representation
- The game world is represented as a 2D character matrix `maze[ROWS][COLUMNS]` with specific symbols:
  - `=` walls
  - `.` pills
  - `o` power pills
  - `</>` teleporters
  - `P` Pacman
  - `G` ghost (Blinky)
  - ` ` (space) empty paths

### Key Algorithms
1. **A* Pathfinding** (`a_star_lib.c`):
   - Calculates optimal paths for the ghost
   - Uses Manhattan distance heuristic
   - Adapts target selection based on ghost mode

2. **Collision Detection** (`lib_pacman.c`):
   - Detects when Pacman and Blinky occupy the same cell
   - Handles appropriate response based on ghost's current mode

3. **Timer Management**:
   - Multiple hardware timers control different aspects of the game:
     - Game countdown
     - Ghost behavior timing
     - Movement timing
     - CAN communication timing

### Hardware Integration
- **LCD Display**: Renders the 240x320 pixel game screen
- **Input Handling**: Processes button presses through external interrupts
- **CAN Communication**: Transmits game statistics to other devices
- **Timers**: Manages game timing, movement intervals, and mode changes

## Key Functions

### Game Control Functions
- `gameOver()`: Handles end-of-game when player loses lives or time runs out
- `victory()`: Triggered when all pills are collected
- `restart()`: Resets character positions after losing a life
- `drawMaze()`: Renders the entire game board on the LCD display

### Character Movement Functions
- `movePacman()`: Updates Pacman's position based on input and handles collisions
- `moveBlinky()`: Moves the ghost along paths calculated by the A* algorithm
- `gameRules()`: Processes the consequences of Pacman's movements (collecting items, etc.)

### AI Functions
- `computePath()`: Runs the A* algorithm to find optimal paths for Blinky
- `computeDestination()`: Determines where Blinky should head based on current mode

This implementation demonstrates advanced embedded programming techniques, combining real-time systems, algorithm development, and interactive graphics on resource-constrained hardware.
