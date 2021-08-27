//
//  main.c
//  BernieBot
//
//  Created by Charlie Sowerby on 8/18/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//


#include "defs.h"
#include "GameState.h"
#include "gui.h"
#include "engine.h"

void moveInput(void){
    char moveInput[5];
    printf("Input Move...\n");
    scanf("%s", moveInput);
    printf("You inputted %s\n", moveInput);
}


/*
 
 TODO: -
 * UNLINK ALL THE HEADERFILES
    * get rid of gui.c and gui.h
    * Move default functions to init
    * move gamestate display functions to Gamestate.c
 * Write function to rotate bitboards, rotate, diag, adiag
 * Write function to update the gamestate for each move
 * write function to search for each move
    * maybe separate function for each type of move? king, knight, pawn, pawn cap, rank slide, file slide, diag, adiag?

 * Implement minimax
 * implement ab pruning 
 

  * Zobrist hashing not super important -> implement for transposition table.
  * Move history? Is this necessary? 
 
 */

/*
 TODO: -

 
 
 
 */

int main(void) {
    init();
    

    
    GameState gs;
    //init_GameState(&gs, "r1bq1bnr/p1p2ppp/3pk3/4p3/2Q1P3/5N2/PPPP1PPP/RNB2RK1 b - - 0 9");
    
    init_GameState(&gs, NULL);
    printGameStateInfo(&gs);


    

    return 0;
}










