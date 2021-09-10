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
    
 * Implement final function for move generation!
    * Castling 
    * Generate all PL move
 
 * Implement make/unmake move
    * Use XOR to update bitboards https://www.chessprogramming.org/General_Setwise_Operations#UpdateByMove
    * Check Legality of Move immediately after making move
    * Must not be in check immediately after move is made
        * if in check after all PL moves, then you're in mate... darn.
 
 
 * Implement function to check legality of PL moves?
    
 
 * Implement minimax
 * implement ab pruning
 * Implement different moveGen that separates quiet moves and attacks?
 
 * Conider writing pop_ls1b funcntion using better bit-twiddling to reset bit (might save an instruction)

  * Zobrist hashing not super important -> implement for transposition table.
  * Move history? Is this necessary? 
 
  * magic bitboards for move generation? 
 */



/* DEBUGGING FEN LIST:
 
 *starting position fen
 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
 
 * Wack position
 "r1bq1bnr/p1p2ppp/3pk3/4p3/2Q1P3/5N2/PPPP1PPP/RNB2RK1 b - - 0 9"
 
 * BISHOP anti-diagonal:
 "b7/1b6/2b5/3b4/4b3/5b2/6b1/7b w - - 0 1"
 
 * black Pieces on diagonals:
 "3n3b/2n3b1/1n3b2/n3b3/3b3q/2b3q1/1b3q2/b3q3 w - - 0 1"
 
 * Square of various pieces:
 8/2RRRRR1/2P3P1/2P3P1/2bbbbb1/8/8/8 w - - 0 1
 
 */

/* IN LLDB DEBUGGER
 type format add -f binary Move
 type format add -f bytes BitBoard
 type format add -f decimal Square
 */

#include "magic.h"

int main(void) {
    clock_t executionStart = clock();
    

    init();
    GameState gs;
    init_GameState(&gs,  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");


    
    //printGameStateInfo(&gs, true);
    
    Move* moveList = NULL;
    int numMoves = moveGen(&moveList, &gs);
    
    printf("NUMBER OF MOVES: %i\n", numMoves);
    for(int i = 0; i < numMoves; i++){
        printMoveInfo(&moveList[i]); 
    }
    
    clock_t executionEnd = clock();
    double elapsedTime = (double) (executionEnd - executionStart)/ CLOCKS_PER_SEC;
    printf("Execution finished in %f seconds.\n", elapsedTime);
    return 0;
}










