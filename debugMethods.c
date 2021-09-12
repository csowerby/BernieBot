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



/* FAILED PERFT
 

 uint64_t Perft(int depth, GameState* gs){
     
     gs->moveIndex = 0;
     
     int test = checkIfGameStateConsistent(gs);
     assert(test == 0);
     
     Move* move_list = NULL;
     int n_moves, i;
     uint64_t nodes = 0;

     if (depth == 0)
         return 1ULL;
     
     //printGameStateInfo(gs, false);
     //printGameBoard(gs);
     
     char gamestateFEN[200] = "";
     generateFEN(gs, gamestateFEN);
     
     n_moves = moveGen(&move_list, gs);

     for(int i = 0; i < n_moves; i++){
         //printMoveInfo(&move_list[i]);
     }

     
     for (i = 0; i < n_moves; i++) {
         //printMoveInfo(&move_list[i]);
         
         GameState tempGS;
         memcpy(&tempGS, &gs, sizeof(GameState));
         //printf("%s\n", gamestateFEN);
         
         Move currentMove = move_list[i];
         Square originSquare = 15 &(currentMove >> 10);
         Piece attackingPiece = gs->squareOccupancy[originSquare];
         
         
         //printGameStateInfo(gs, false);
         gs->moveHist[gs->moveIndex] = move_list[i];
         gs->moveIndex++;
         int success = makeMove(gs, move_list[i]);
         
         test = checkIfGameStateConsistent(gs);
         if (test != 0){
             printf("FEN: %s\n", gamestateFEN);
             printf("attacking piece: %i\n", attackingPiece);
             printMoveInfo(&currentMove);
             printGameStateInfo(gs, true);
         }
         assert(test == 0);
         
         //printGameStateInfo(gs, false);
         if (success != -1){
             nodes += Perft(depth - 1, gs);
         }
         
         
         char gamestateFEN2[200] = "";
         generateFEN(gs, gamestateFEN2);
         //printf("%s\n", gamestateFEN2);
         
         
         unmakeMove(gs, move_list[i]);
         
         gs->moveIndex--;
         gs->moveHist[gs->moveIndex] = 0b0000000000000000;
         
         int testGS2 = compareGameStates(gs, &tempGS);
         
         if(testGS2 != 0){
             printf("ERROR CODE: %i\n", testGS2);
             printf("ORIGINAL GAMESTATE:\n");
             printGameStateInfo(&tempGS, true);
             printf("MODIFIED GAMESTATE: \n");
             printGameStateInfo(gs, true);
             printf("MOVE:\n");
             printMoveInfo(&currentMove);
             assert(false);
             
         }

         
         test = checkIfGameStateConsistent(gs);
         assert(test == 0);
         
         //printGameStateInfo(gs, true);
     }
     return nodes;
 }

 
 */

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
