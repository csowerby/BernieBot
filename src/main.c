//
//  main.c
//  BernieBot
//
//  Created by Charlie Sowerby on 8/18/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//


#include "defs.h"
#include "gamestate.h"
#include "engine.h"
#include "perft.h"



/*

 NOTES TO SELF:



 Next steps:


 * add funcationality for 50 move draw
 * add special code 0110 for quiet pawn push



 * implement ab pruning
 * Implement different moveGen that separates quiet moves and attacks?
    *i.e. move ordering

  * Zobrist hashing not super important -> implement for transposition table.
  * Move history? Is this necessary?


 * Consider in the future using a bitboard for attacked squares - legal move generation instead of PL

 * better to just be passed a bitboard with target Squares and origin Square & 4 digit code?

 * PRETTY SURE IT IS FASTER TO DO PIECE CENTRIC MOVE GENERATION BECAUSE OF SUPER FAST BITBOARD OPERATIONS i.e. encode moves with originPiece, targetSquare? also to move gen function pass just bitboards of each knight/king/sliding/diag piece moves, then use bitwise ops to isolate ls1b bitboard, and apply xor to the bitboards, might not even need
 */



/* DEBUGGING FEN LIST:

    * kiwipete
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "

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











int main(void) {
    printf("\n------------------ BernieBot ------------------\n\n");

    init();
    startPerft(8, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");



    // init();
    //
    //
    // GameState gs;
    //
    // init_GameState(&gs, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4RK1 b kq - 1 1");
    // int test = checkIfGameStateConsistent(&gs);
    // printGameStateInfo(&gs, true);
    //
    // Move rookMove = 65488;
    // makeMove(&gs, rookMove);
    // printMoveInfo(rookMove);
    //
    //
    // printGameStateInfo(&gs, true);
    // Move move_list[MOVE_LIST_LENGTH];
    // int nMoves = moveGen(move_list, &gs);
    // for(int i = 0; i < nMoves; i++){
    //     printMoveInfo(move_list[i]);
    // }




    // GameState gs;
    //
    // init_GameState(&gs, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    // clock_t executionStart = clock();
    //
    // uint64_t total_nodes = Perft_debug(8, &gs);
    // printf("Total Nodes: %llu", total_nodes);
    //
    //
    // clock_t executionEnd = clock();
    // double elapsedTime = (double) (executionEnd - executionStart)/ CLOCKS_PER_SEC;

    //printf("Execution finished in %f seconds.\n", elapsedTime);





    printf("\n---------------- End BernieBot ----------------\n\n");



    return 0;
}
