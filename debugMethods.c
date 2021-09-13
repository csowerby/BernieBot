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




/* --------------- PERFT SPEED TEST ------------------------------*/

printf("Starting Perft...\n");

uint64_t moveGen = 0;
uint64_t moveMaking = 0;
uint64_t moveUnmaking = 0;
uint64_t abortedMoves = 0;




clock_t executionStart = clock();
uint64_t nodes = Perft_speedtest(5, &gs, &moveGen, &moveMaking, &moveUnmaking, &abortedMoves);
printf("Perft Nodes Accessed: %llu\n", nodes);

printf("Generating Moves: %llu\n", moveGen);
printf("Making Moves: %llu\n", moveMaking);
printf("Unmaking Moves: %llu\n", moveUnmaking);
printf("# of Aborted Moves: %llu\n", abortedMoves);




clock_t executionEnd = clock();
double elapsedTime = (double) (executionEnd - executionStart)/ CLOCKS_PER_SEC;
printf("Execution finished in %f seconds.\n", elapsedTime);
 

/* --------------- MAKE MOVE TESTING -----------------------------*/


init_GameState(&gs, "rnbqkbnr/p1pp1ppp/8/Pp2p3/8/8/1PPPPPPP/RNBRKBNR w KQkq b6 0 3");
printGameStateInfo(&gs, false);

Move *moveList = NULL;
int numMoves = moveGen(&moveList, &gs);

for(int i = 0; i < numMoves; i++){
    printMoveInfo(&moveList[i]);
}

for(int i = 0; i < numMoves; i++){
    printMoveInfo(&moveList[i]);
    makeMove(&gs, moveList[i]);
    
    int test = checkIfGameStateConsistent(&gs);
    assert(test == 0);
    
    unmakeMove(&gs, moveList[i]);
    
    test = checkIfGameStateConsistent(&gs);
    assert(test == 0);
}

/* --------------- UNMAKE MOVE TESTING ---------------------------*/


init_GameState(&gs, "rnbqkbnr/p1pp1ppp/8/Pp2p3/8/8/1PPPPPPP/RNBRKBNR w KQkq b6 0 3");
printGameBoard(&gs);

init_GameState(&gs, "rnbqkbnr/2pppppp/P7/8/8/8/1PPPPPPP/RNBQKBNR b KQkq - 0 3");
printGameBoard(&gs);

unmakeMove(&gs, 0b1000011010000101);
int test = checkIfGameStateConsistent(&gs);



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
