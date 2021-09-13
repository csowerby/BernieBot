//
//  Perft.c
//  BernieBot
//
//  Created by Charlie Sowerby on 9/12/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include "Perft.h"


int checkIfGameStateConsistent(GameState *gs){
    
    // Compare any two bitboards -> return 1 if they overlap
    BitBoard test1 = 0ULL;
    for(int i = 0; i < 12;i++){
        test1 = gs->boards[i];
        for(int j = 0; j < 12; j++){
            if (i == j){continue;}
            test1 &= gs->boards[j];
            if(test1 != 0ULL){
                return 1;
            }
        }
    }

    // TEST SUM OF PIECES EQUALS sidePieces
    // white pieces
    test1 = 0ULL;
    for(int i = 0; i < 6; i++){
        test1 |= gs->boards[i];
    }
    if(test1 != gs->boards[wPieces]){
        return 2;
    }
    //black pieces
    test1 = 0ULL;
    for(int i = 6; i < 12; i++){
        test1 |= gs->boards[i];
    }
    if(test1 != gs->boards[bPieces]){
        return 3;
    }
    
    //all pieces
    if((gs->boards[wPieces] | gs->boards[bPieces]) != gs->boards[aPieces]){
        return 4;
    }
    
    // Check square occupancy is redundant with bitboards
    for(int i = 0; i < 64; i++){
        Piece occupancy = gs->squareOccupancy[i];
        if(occupancy == no_pce){
            if(getBit(&gs->boards[aPieces], i)){
                return -i;
            }
        }else{
            if(!getBit(&gs->boards[gs->squareOccupancy[i]], i)){
                return -i;
            }
        }
    }
    
    
    
    return 0;
}


uint64_t Perft(int depth, GameState* gs){
    
    Move* move_list = NULL;
    int n_moves, i;
    uint64_t nodes = 0;

    if (depth == 0)
        return 1ULL;
    
    GameState tempGS;
    memcpy(&tempGS, gs, sizeof(GameState));
    
    n_moves = moveGen(&move_list, gs);
    /*
    for(int i = 0; i < n_moves; i++){
        printMoveInfo(&move_list[i]);
    }
    */
    
    for (i = 0; i < n_moves; i++) {
        // Loop through and make all the moves
        
        printMoveInfo(&move_list[i]);
        
        int success = makeMove(gs, move_list[i]);
        
        printGameBoard(gs);
        
        if (success != -1){
            nodes += Perft(depth - 1, gs);
        }
        
        unmakeMove(gs, move_list[i]);
        
        int testGS = compareGameStates(gs, &tempGS);
        
        if(testGS != 0){
            printf("ERROR CODE: %i\n", testGS);
            printf("ORIGINAL GAMESTATE:\n");
            printGameStateInfo(&tempGS, true);
            printf("MODIFIED GAMESTATE: \n");
            printGameStateInfo(gs, true);
            assert(false);
            
        }
    }
    free(move_list);
    return nodes;
}
