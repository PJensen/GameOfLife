/*
** Copyright 1998-2003, Peter J. Jensen
** All Rights Reserved.
** 
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of Peter J. Jensen
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of Hairylogic, Inc.
** 
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**        ______
**       /_____/\
**      /_____\\ \      FILE:    gol.c
**     /_____\ \\ /     AUTHOR:  Pete Jensen
**    /_____/ \/ / /    DATE:    October 16th, 2006
**   /_____/ /   \//\   VERSION: 2.0.2
**   \_____\//\   / /   
**    \_____/ / /\ /    
**     \_____/ \\ \     http://euler.acadiau.ca/~064881j
**      \_____\ \\ 
**       \_____\/
**
**       Additional Comments / Info:
**
** INFO: http://en.wikipedia.org/wiki/Conway's_Game_of_Life
**
** NOTE: This is somewhat hacky-code; also as ONLY an expiriment decided to 
**    stray from my usual coding conventions and use a slightly different 
**    (possibly) more readable style of coding.
*/

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <conio.h>

#include "graph.h"

#ifdef __MOUSE_ENABLED
#include "mouse.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// Screen Defines
#define SCREEN_WIDTH                   320
#define SCREEN_HEIGHT                  200

//////////////////////////////////////////////////////////////////////////////
// Cell Growth Area.
#define WIDTH                          100  
#define HEIGHT                         100

//////////////////////////////////////////////////////////////////////////////
// Four bounding boxes, with shaded colors ranging from values, 20 to 23.
#define BOUNDING_BOX_ONE               (320/2) - 51, (320/2) + 50, 24, 125, 20
#define BOUNDING_BOX_TWO               (320/2) - 52, (320/2) + 51, 23, 126, 21
#define BOUNDING_BOX_THREE             (320/2) - 53, (320/2) + 52, 22, 127, 22
#define BOUNDING_BOX_FOUR              (320/2) - 54, (320/2) + 53, 21, 128, 23

//////////////////////////////////////////////////////////////////////////////
// The predefined states of the cells.
#define STATE_DEAD                     0x00
#define STATE_ALIVE                    0x01

//////////////////////////////////////////////////////////////////////////////
// The background color of the working cell area.
#define BG_COLOR                       16
#define CELL_COLOR                     9

//////////////////////////////////////////////////////////////////////////////
// Predefined conditions on cellular evolution. (The Rules)
#define CELL_SURVIVAL_CONDITION           (n == 2 || n == 3)
#define CELL_OVERCROWDING_CONDITION       (n > 3)
#define CELL_ISOLATION_CONDITION          (n == 1 || n == 0)
#define EMPTY_CELL_GENERATION_CONDITION   (n == 3)

//////////////////////////////////////////////////////////////////////////////
// Max number of cells.
#define MAX_CELLS                      WIDTH * HEIGHT

//////////////////////////////////////////////////////////////////////////////
// The Default starting number of cells, unless specified @ cmd line.
#define INIT_CELLS                     600

//////////////////////////////////////////////////////////////////////////////
// The time to delay between each iteration, ms (milli-seconds) IFF mouse IS
// enabled, and if the mouse is NOT enabled (automatic).
#ifdef __MOUSE_ENABLED
#define DELAY_TIME                     0 //ms
#else
#define DELAY_TIME                     10 //ms
#endif

// Prototypes
int getNeighbors(int x, int y);
void displayMessage(void);
void drawBoundingBox(int left, int right, int top, int bottom, int color);
void displayCells(void);
void processCells(void);
void initializeCells(int initCells);
void drawShadedBorder(void);

//////////////////////////////////////////////////////////////////////////////
// Cell Data Structure, Living & Non-Living.
struct _Cell {
    unsigned char type;       // Unused.
    unsigned char state;      // STATE_DEAD (or) STATE_ALIVE
    unsigned char neighbors;  // @See function getNeighbors

} Cells[WIDTH][HEIGHT];       // H4cKy GBL storage.


//////////////////////////////////////////////////////////////////////////////
//                         Program Entry Point
int main(int argc, char **argv)
{
    int argNumCells;

    (void) randomize();
    
    if (argc == 2) {
        // Grab argument, convert to integer; init cells with passed arg.
        argNumCells = (int)atoi((char*)argv[1]);
        (void) initializeCells(argNumCells);
        
        // Display message, showing # of init'ed cells.
        printf("[Game of Life:] Initialized %d Cells.", argNumCells);
        
        // Wait a second.
        delay(1000);
    }
    // No arguments were passed? Initialize Default # of cells.
    else {  
        (void) initializeCells(INIT_CELLS);
        delay(1000);
    }

    // Initialize 0x13H Graphics Mode, 320x200x256
    (void) initgraph();

    // Display program message, copyright etc.
    (void) displayMessage();

    // Draw a border around the cell area.
    (void) drawShadedBorder();

#ifdef __MOUSE_ENABLED    
    (void) ShowMouse();
#endif
    
    
    // Enter main loop
    while ( !kbhit() ) 
    {    

//////////////////////////////////////////////////////////////////////////////
// From my general feelings I am guessing that opening a bracket in an #ifdef
// expecting to close that bracket if there is a following #ifdef a few lines
// down is somewhat of a different idea.  This technique allows me to let the
// user click per iteration that they want to see.
#ifdef __MOUSE_ENABLED
        (void)ReadMouse();

        if (MOUSE_BUTTON_1_PRESSED) {  
#endif
// Display & Process cells, then wait delay time, Reapeat until
// keystroke is enacted.
            (void) displayCells();
            (void) processCells();

#ifdef __MOUSE_ENABLED
            (void) processCells();
        }                           // <-- END funky idea.
#endif

        // @see #define DELAY_TIME
        (void) delay(DELAY_TIME);
    }

#ifdef __MOUSE_ENABLED
    (void) HideMouse();
#endif

    // End graphics mode.
    (void) endgraph();
    
    // Exit politely.
    return EXIT_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// Name:                         getNeighbors
// Purpose:
//    This function returns the number of neighbors that a specific cell 
//    specified by (x, y) parameters has.  This function is used mainly in 
//    the processing state.
// Parameters:
//    x - The x coordinate for cell to find neighbors of.
//    y - The y coordinate for cell to find neighbors of.
// Returns:
//    neighborCount - The number of neighbors that this (x,y) cell has.
int getNeighbors(int x, int y)
{
    unsigned int cellX = x;
    unsigned int cellY = y;
    unsigned int neighborCount = 0;

    // LEFT NEIGHBOT
    if (get_pixelcolor(cellX - 1, cellY) == CELL_COLOR)
        neighborCount++;

    // TOP LEFT NEIGHBOT
    if (get_pixelcolor(cellX - 1, cellY - 1) == CELL_COLOR)
        neighborCount++;

    // BOTTOM LEFT NEIGHBOT
    if (get_pixelcolor(cellX - 1, cellY + 1) == CELL_COLOR)
        neighborCount++;
                
    // RIGHT NEIGHBOR
    if (get_pixelcolor(cellX + 1, cellY) == CELL_COLOR)
        neighborCount++;

    // TOP RIGHT NEIGHBOR
    if (get_pixelcolor(cellX + 1, cellY - 1) == CELL_COLOR)
        neighborCount++;

    // BOTTOM RIGHT NEIGHBOR
    if (get_pixelcolor(cellX + 1, cellY + 1) == CELL_COLOR)
        neighborCount++;
       
    // BOTTOM NEIGHBOR
    if (get_pixelcolor(cellX, cellY - 1) == CELL_COLOR)
        neighborCount++;

    // TOP NEIGHBOR
    if (get_pixelcolor(cellX, cellY + 1) == CELL_COLOR)
        neighborCount++;
    
    Cells[x][y].neighbors = neighborCount;

    return neighborCount;
}

//////////////////////////////////////////////////////////////////////////////
// Name:                         drawShadedBorder
// Purpose: 
//    Draw a special shaded border.
// Parameters: None
// Returns:    None
///
void drawShadedBorder()
{
    (void) drawBoundingBox(BOUNDING_BOX_ONE);
    (void) drawBoundingBox(BOUNDING_BOX_TWO);
    (void) drawBoundingBox(BOUNDING_BOX_THREE);
    (void) drawBoundingBox(BOUNDING_BOX_FOUR);

    return;
}

//////////////////////////////////////////////////////////////////////////////
// Name:                         initializeCells
// Purpose: 
//    Initialize all of the cells, living and non-living.
// Parameters: None
// Returns:    None
void initializeCells(int init)
{
    unsigned int w, h, x, y;
    register unsigned int index;

    // Init Cells Dead
    for (h = 0; h < HEIGHT; h++)
        for (w = 0; w < WIDTH; w++)
            Cells[w][h].state = STATE_DEAD;

    // Init Alive Cells
    for (index = 0; index < init; index++)
    {
        x = rand() % WIDTH;
        y = rand() % HEIGHT;
        
        Cells[x][y].state = STATE_ALIVE;
    }
    
    return;
}

//////////////////////////////////////////////////////////////////////////////
// Name:                         displayCells
// Purpose: 
//    Display all cells based on their current states.
// Parameters: None
// Returns:    None
void displayCells()
{

    unsigned int w, h;

        // Display Cells
        for (h = 0; h < HEIGHT; h++)
            for (w = 0; w < WIDTH; w++)
            {
                if (Cells[w][h].state == STATE_ALIVE) 
                    pset(w, h, CELL_COLOR);
                else 
                    pset(w, h, BG_COLOR);
            }
    return;
}

//////////////////////////////////////////////////////////////////////////////
// Name:                         processCells
// Purpose:
//    Process cell state into next states.
// Parameters: None
// Returns:    None
void processCells()
{
        int w, h, n;

        for (h = 0; h < HEIGHT; h++)
            for (w = 0; w < WIDTH; w++)
            {
                n = getNeighbors(w, h);

                if (Cells[w][h].state == STATE_ALIVE)
                {
                    if (CELL_SURVIVAL_CONDITION)
                        Cells[w][h].state = STATE_ALIVE;
                        
                    else if (CELL_OVERCROWDING_CONDITION)
                        Cells[w][h].state = STATE_DEAD;
                        
                    else if (CELL_ISOLATION_CONDITION)
                        Cells[w][h].state = STATE_DEAD;
                }
                else
                    if (EMPTY_CELL_GENERATION_CONDITION)
                        Cells[w][h].state = STATE_ALIVE;

                if (n == 8)
                    Cells[w][h].state = STATE_ALIVE;
            }

    return;
}

//////////////////////////////////////////////////////////////////////////////
// Name:                         displayMessage
// Purpose:
//    Display the program message.
// Parameters: None
// Returns:    None
void displayMessage()
{
    gotoxy(3, 18);
    printf("John Conway's - 'Game of Life'");
    gotoxy(3, 21);
    printf("Copyright (c) 2006, Peter J. Jensen,\n All Rights Reserved.");

#ifdef __MOUSE_ENABLED
    gotoxy(2, 2);
    printf("[Mouse]:ON Left-click -> 'next-state'");
#endif

    return;
}
//////////////////////////////////////////////////////////////////////////////
// Name:                         drawBoundingBox
// Purpose: 
//    This function simply draws a rectangle of a given color based on the 
//    four coordinates that the client specifies.  The color is based on a 
//    number from 0 to 256.
// Parameters: 
//    left     - The left coord.
//    right    - The right coord.
//    top      - The top coord, NOTE: top < bottom
//    bottom   - The bottom coord, NOTE: bottom > top
// Returns:    None
void drawBoundingBox(int left, int right, int top, int bottom, int color)
{
    register unsigned int x, y;

    /* Drow top boundary */
    for (x = left; x <= right; x++)
        psetn(x, top, color);
        
    /* Draw bottom boundary */
    for (x = left; x <= right; x++)
        psetn(x, bottom, color);

    /* Draw left boundary */
    for (y = top; y <= bottom; y++)
        psetn(left, y, color);
        
    /* Draw right boundary */
    for (y = top; y <= bottom; y++)
        psetn(right, y, color);

    return;
}
