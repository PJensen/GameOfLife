/*
** Copyright 1998-2005, Peter J. Jensen.
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
**
**        ______
**       /_____/\
**      /_____\\ \      FILE:    graph.h
**     /_____\ \\ /     AUTHOR:  Pete Jensen
**    /_____/ \/ / /    DATE:    Oct 19th, 2006
**   /_____/ /   \//\   VERSION: 2.0.1
**   \_____\//\   / /   
**    \_____/ / /\ /    
**     \_____/ \\ \     HTTP://euler.acadiau.ca/~064881j
**      \_____\ \\ 
**       \_____\/
**
** This in-line module handles all low level video operations, 
** such as initializing a graphics mode, ending a graphics mode, 
** puting pixels etc.
** 
** <IN-LINE MODULE CONTENTS>
**    initgraph();
**    endgraph();
**    pset();
**    get_pixelcolor();
** 
**    I admit PSET reminds me quite a bit of QBASIC.
**
** WARNING:
**    Do not use psetn(word, word, word); anywhere but GameOfLife!
**
** USAGE:
**    Only include graph.h, since this is in-line this IS the implementation.
*/

#if !defined(__GRAPH_H)
#define __GRAPH_H

#include <DOS.H>

//////////////////////////////////////////////////////////////////////////////
// Defines for graphics module
#define VIDEO_BIOS      10H
#define VIDEO_RAM       0xA000 

//////////////////////////////////////////////////////////////////////////////
// Screen size, width & height defined
#define SCREEN_SIZE     (320 * 200)
#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   200

//////////////////////////////////////////////////////////////////////////////
// Typdefs, byte & word.
typedef unsigned char byte;
typedef unsigned short word;

//////////////////////////////////////////////////////////////////////////////
// Prototypes
void initgraph(void);
void endgraph(void);
void pset(word X, word Y, byte color);
void psetn(word X, word Y, byte color);
byte get_pixelcolor(word X, word Y);

//////////////////////////////////////////////////////////////////////////////
/**
 * Function: initgraph
 * Purpose : To initialize the graphics mode into mode 13H; it has the 
 *           properties of 320x200 screensize & 8 bits per pixel.
 * Inputs  : Void
 * Returns : Void
 * Modifies: The current graphics mode.
 * Sample Call : initgraph();
 */
void initgraph(void) {
	 //union REGS IN, OUT; /* Register variables */
    //IN.h.al = 0x13; 
    //IN.h.ah = 0x00; /* This is the actual screen settings Zord!*/
    //int86(0x10, &IN, &OUT); 

    asm MOV AL, 13H;
    asm MOV AH, 00H;
    asm INT VIDEO_BIOS;
}

//////////////////////////////////////////////////////////////////////////////
/*
 * Function: endgraph
 * Purpose : To take us out of the 13H graphics mode and back into dos text
 *           mode. This should be run anytime the program hits an error. 
 *           and displays an error message.
 * Inputs  : Void
 * Returns : Void.
 * Modifies: The Current Graphics Mode.
 * Sample Call : endgraph();
 */
void endgraph(void) {
	//union REGS IN, OUT; /* Register variables */
	//IN.h.al = 0x3;    /* Video mode parameter - mode 3H*/
	//IN.h.ah = 0x00;   /* Interrupt 10H - service 0H */
	//int86(0x10, &IN, &OUT); /* Call the interrupt 10H (Video Control)*/

    asm MOV AL, 3H;
    asm MOV AH, 00H;
    asm INT VIDEO_BIOS;
    
}

//////////////////////////////////////////////////////////////////////////////
/*
 * Function: pset
 * Purpose : To put a pixel @ x,y with given colorByte.
 * Inputs  : word X: The X location of the pixel.
             word Y: The Y location of the pixel.
 * Returns : Void
 * Modifies: Adds one pixel to the graphics screen.
 * Sample Call : pset(10, 10, 14);
 */
void pset(word X, word Y, byte color) {

    // OFFSET HACK!! DO NOT RE-USE THIS FUNCTION!
    // THIS IS A QUICK HACK FOR CENTERING A PARTICULAR OBJECT.
    X  = X + ((320/2) - 50);
    Y = Y + (200 / 2) - 75;
    asm{
        MOV DI, 0xA000  /* Vram */
        MOV ES, DI     
        MOV AX, Y       
        MOV BX, Y       
        SHL AX, 8       
        SHL BX, 6       
        ADD AX, BX      
        ADD AX, X       
        MOV DI, AX      
                                
        MOV AL, color      /* Set color byte*/
        MOV ES:[DI], AL    /* Move the byte to that offset in VRAM */
	}
}

//////////////////////////////////////////////////////////////////////////////
/*
 * Function: psetn
 * WARNING : DO NOT USE ELSEWHERE!
 * Purpose : To put a pixel @ x,y with given colorByte.
 * Inputs  : word X: The X location of the pixel.
             word Y: The Y location of the pixel.
 * Returns : Void
 * Modifies: Adds one pixel to the graphics screen.
 * Sample Call : psetn(10, 10, 14);
 */
void psetn(word X, word Y, byte color) {
    //NO OFFSET HACK!
    asm{
        MOV DI, 0xA000  /* Vram */
        MOV ES, DI     
        MOV AX, Y       
        MOV BX, Y       
        SHL AX, 8       
        SHL BX, 6       
        ADD AX, BX      
        ADD AX, X       
        MOV DI, AX      
                                
        MOV AL, color      /* Set color byte*/
        MOV ES:[DI], AL    /* Move the byte to that offset in VRAM */
	}
}

//////////////////////////////////////////////////////////////////////////////
/*
 * Function: get_pixelcolor
 * Purpose : To get a pixel @ x,y with & return colorByte.
 * Inputs  : word X: The X location of the pixel.
             word Y: The Y location of the pixel.
 * Returns : The byteColor @ x,y.
 * Sample Call : byteColor = get_pixelcolor(10, 10);
 */
byte get_pixelcolor(word X, word Y) {
    byte color;

    //OFFSET HACK!
    X  = X + ((320/2) - 50);
    Y = Y + (200 / 2) - 75;
        
    asm MOV AH, 0DH  // Select function.
    asm MOV CX,X     /* X: Location */
    asm MOV DX,Y     /* Y: Location */
    asm INT 10H      // BIOS VIDEO INT
    asm MOV color,AL /* Color Byte Val. */
            
    return color;    // Return color.
}

#endif

