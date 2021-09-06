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
 
  * magic bitboards for move generation? 
 */

/*
 TODO: -

 
 
 
 */




int main(void) {
    clock_t executionStart = clock();
    /* IN LLDB DEBUGGER
     type format add -f binary Move
     type format add -f bytes BitBoard
     type format add -f decimal Square
     
     
     */
    init();
    
    
    GameState gs;
    //init_GameState(&gs, "r1bq1bnr/p1p2ppp/3pk3/4p3/2Q1P3/5N2/PPPP1PPP/RNB2RK1 b - - 0 9");
    
    init_GameState(&gs, "b7/1b6/2b5/3b4/4b3/5b2/6b1/7b w - - 0 1");
    //init_GameState(&gs, NULL);
    printGameStateInfo(&gs, false);
    
    Move* slidingList = NULL;
    int len = calcSlidingMoves(&slidingList, d2, &gs);
    
    
    Move move;
    for (int i = 0; i < len; i++){
        move = slidingList[i];
        printMoveInfo(&move);
        }
    
    
    
    
    
    clock_t executionEnd = clock();

    
    double elapsedTime = (double) (executionEnd - executionStart)/ CLOCKS_PER_SEC;
    printf("Execution finished in %f seconds.\n", elapsedTime);
    return 0;
}










