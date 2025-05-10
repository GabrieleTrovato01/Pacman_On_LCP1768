#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pacman.h"
//#include "../GLCD/GLCD.h"

// Lists for nodes to explore (openList) and already explored (closedList)
Node openList[MAX_NODE];    // Nodes to explore 
Node closedList[MAX_NODE];  // Nodes already explored
int openListCounter = 0;         // Number of nodes in openList
int closedListCounter = 0;       // Number of nodes in closedList

// Direction vectors for exploring neighbors (right, down, left, up)
int dx[] = {0, 1, 0, -1};
int dy[] = {1, 0, -1, 0};

// Path storage
PathStep path[MAX_NODE];  // Steps in the calculated path
int counterPath = 0;        // Number of steps in the path
int actualStep = 0;      // Current step in the path
char previousCell = EMPTY;
int destX;
int destY;
int execFind = 0;

// Function to calculate Manhattan distance between two points
int distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Adds a node to the openList
void addOpenList(Node node) {  
		if(openListCounter >= MAX_NODE){
				return;
			}
		openList[openListCounter] = node;
    openListCounter++;
}

// Adds a node to the closedList
void addClosedList(Node node) {
		if(closedListCounter >= MAX_NODE){
			return;
		}
    closedList[closedListCounter] = node;
    closedListCounter++;
}

// Extracts the node with the lowest F value from the openList
Node popLowestF() {
    int min_index = 0;
    int i, j;

    // Find the node with the lowest F value
    for (i = 1; i < openListCounter; i++) {
        if (openList[i].f < openList[min_index].f) {
            min_index = i;
        }
    }

    Node node = openList[min_index];

    // Remove the node from openList by shifting the remaining elements
    for (j = min_index; j < openListCounter - 1; j++) {
        openList[j] = openList[j + 1];
    }
    openListCounter--;

    return node;
}

// Function to create and initialize a new node
Node createNode(int x, int y, int g, int previous_index) {
		Node node;
		node.x = x;
		node.y = y;
		node.g = g;
		node.h = distance(x, y, destX, destY);
		node.f = g + node.h;
		node.previous_index = previous_index;
    
		return node;
}

// Function to check if a position is valid and not in closedList
int isValidPosition(int x, int y){
	int j;
	
	if(x<0 || x >= COLUMNS || y <0 || y >= ROWS || maze[y][x] == '='){
		return 0;
	}
	
	for(j=0; j < closedListCounter; j++){
		if(closedList[j].x == x && closedList[j].y == y){
			return 0;
		}
	}
	return 1;
}
// Function to calculate the destination of the path
void computeDestination(){
	
	//PathStep dest;
	
	switch(blinkyMode){
	
		case 0:
			destX = pacmanX;
			destY = pacmanY;
			break;
		
		case 1:
			// pacman upper left
			if(pacmanX <= COLUMNS/2 && pacmanY <= ROWS/2){
				destX = 27;
				destY = 33;
			}
			// pacman lower left
			else if(pacmanX <= COLUMNS/2 && pacmanY >= ROWS/2){
				destX = 27;
				destY = 6;
			}
			// pacman upper right
			else if(pacmanX >= COLUMNS/2 && pacmanY <= ROWS/2){
				destX = 2;
				destY = 33;
			}
			// pacman lower right
			else if(pacmanX >= COLUMNS/2 && pacmanY >= ROWS/2){
				destX = 2;
				destY = 6;
			}
			break;
		
		default:
			break;
		
	}
	//return dest;
}

void computePath() {
		
		if(execFind){
			return;
		}
		execFind = 1;
		
    int i;
		computeDestination();
	
    // Reset global variables
    counterPath = 0;
    openListCounter = 0;
    closedListCounter = 0;
    actualStep = 0;

    // Add starting node to openList
    addOpenList(createNode(blinkyX, blinkyY, 0, -1));

    // While there are nodes to explore in the openList
    while (openListCounter > 0) {
        // Extract the node with the lowest F value
        Node current = popLowestF();

        // If the target node is reached, reconstruct the path
        if (current.x == destX && current.y == destY) {
            // Add target node to closedList
            addClosedList(current);
            
            // Start from the current node's closed list index
            int currentIndex = closedListCounter - 1;  // Index of the last added node

            while (currentIndex != -1) {
                // Create a temporary PathStep
								PathStep step;
								step.x = closedList[currentIndex].x;
								step.y = closedList[currentIndex].y;

								// Insert the step into the path
								path[counterPath++] = step;

								// Move to the previous node
								currentIndex = closedList[currentIndex].previous_index;
            }

            actualStep = counterPath-1;
						execFind = 0;
            return;
        }

        // Add the current node to the closedList
        addClosedList(current);

        // Explore neighbors
        for (i = 0; i < 4; i++) {
            int new_x = current.x + dx[i];
            int new_y = current.y + dy[i];

            // Check if the neighbor position is valid
            if (isValidPosition(new_x, new_y)) {
                // Create and add the neighbor node to openList
                addOpenList(createNode(new_x, new_y, current.g+1, closedListCounter-1));
            }
        }
    }
		execFind = 0;
}