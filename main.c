//
//  main.c
//  BernieBot
//
//  Created by Charlie Sowerby on 8/18/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//


#include "defs.h"
#include "GameState.h"
#include "engine.h"

void moveInput(void){
    char moveInput[5];
    printf("Input Move...\n");
    scanf("%s", moveInput);
    printf("You inputted %s\n", moveInput);
}


/*
 
 TODO: -
 
 * Just finished kingmove/knightMove functions
    * Just finished move decoder function
 
 Next:
 * Implement the rest of the moveGen functions
    * Sliding pieces (diagonal too)
    *
 
 * Implement move checking?
    * Bitboards for attacking pieces?
    * vector attacks?
 

 * Write function to update the gamestate for each move
    
 
 * Implement minimax
 * implement ab pruning
 * Implement different moveGen that separates quiet moves and attacks?
 

  * Zobrist hashing not super important -> implement for transposition table.
  * Move history? Is this necessary? 
 
 */

/*
 TODO: -

 
 
 
 */




int main(void) {
    
    
    /* IN LLDB DEBUGGER
     type format add -f binary Move
     type format add -f bytes BitBoard
     type format add -f decimal Square
     
     
     */
    init();
    
    
    GameState gs;
    //init_GameState(&gs, "r1bq1bnr/p1p2ppp/3pk3/4p3/2Q1P3/5N2/PPPP1PPP/RNB2RK1 b - - 0 9");
    
    init_GameState(&gs, "B7/1B6/2B5/3B4/4B3/5B2/6B1/7B w - - 0 1");
    //init_GameState(&gs, NULL);
    printGameStateInfo(&gs, false);
    
    Move* slidingList = NULL;
    int len = calcSlidingMoves(&slidingList, d2, &gs);
    
    
    Move move;
    for (int i = 0; i < len; i++){
        move = slidingList[i];
        printMoveInfo(&move);
        }
    
    
    return 0;


}










