//
//  debugMethods.c
//  BernieBot
//
//  Created by Charlie Sowerby on 9/9/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//
#include "defs.h"

#ifdef DEBUG_METHODS


/*
 FILE FOR A LIBRARY OF DEBUGGING FUNCTIONS
    * Specific Piece Move Generation
 
 
 */

/* ----------------------------------------------------------------*/
/* ------------- Speed Testing ------------------------------------*/
/* ----------------------------------------------------------------*/

BitBoard testBoard = 0xAFBEC0113ULL;
clock_t startTest = clock();
for(int i = 0; i < 63; i ++ ){
    if(getBit(&testBoard, i)){
        clearBit(&testBoard, i);
    }
}
clock_t endTest = clock();
printf("Clock Cycles Elapsed: %lu\n", endTest - startTest);


/* ----------------------------------------------------------------*/
/* ------------- Move Generation-----------------------------------*/
/* ----------------------------------------------------------------*/

// Testing moveGen for all pieces at once

Move* moveList = NULL;
int numMoves = moveGen(&moveList, &gs);

printf("NUMBER OF MOVES: %i\n", numMoves);
for(int i = 0; i < numMoves; i++){
    printMoveInfo(&moveList[i]);
}

// Example for Pawn Moves

init();
GameState gs;
init_GameState(&gs,  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
printGameStateInfo(&gs, false);


Move *moveList = NULL;
int numMoves = calcPawnMoves(&moveList, &gs);
for(int i = 0; i < numMoves; i++){
    printMoveInfo(&moveList[i]);
}





/* ------------------------------------------------------*/



#endif
