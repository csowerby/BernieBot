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
    /*
    Square test_norm_to_diag[64];
    Square test_diag_to_norm[64];
    
    int diag_sq = 56;
    // TOP HALF
    for(int i = 7; i >= 0; i-- ){
        for (int j = 0;  j + i <= 7; j++){
            printf("norm Board: %2i - %2i : rot Board \n", 8 * (7 - j) + i + j, diag_sq);
            
            test_norm_to_diag[8 * (7 - j) + i + j] = (Square) diag_sq;
            test_diag_to_norm[diag_sq] =  8 * (7 - j) + i + j;
            // Increment square counter
            if (diag_sq % 8 == 7){
                diag_sq -= 15;

            }else{
                diag_sq++;
            }
        }
    }
    for(int i = 6; i >= 0; i--){
        for (int j = 0; j <= i; j++){
            printf("norm Board: %i - %i : rot Board \n", (8 * (i - j)) + j, diag_sq);
            test_norm_to_diag[(8 * (i - j)) + j] = (Square) diag_sq;
            test_diag_to_norm[diag_sq] =  (8 * (i - j)) + j;
            // Increment square counter
            if (diag_sq % 8 == 7){
                diag_sq -= 15;

            }else{
                diag_sq++;
            }
        }
    }
    printf("\n");

    */
    init();
    

    
    GameState gs;
    //init_GameState(&gs, "r1bq1bnr/p1p2ppp/3pk3/4p3/2Q1P3/5N2/PPPP1PPP/RNB2RK1 b - - 0 9");
    
    init_GameState(&gs, "B7/1B6/2B5/3B4/4B3/5B2/6B1/7B w - - 0 1");
    printGameStateInfo(&gs);


    
    
    return 0;

}










