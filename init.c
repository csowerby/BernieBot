//
//  init.c
//  BernieBot
//
//  Created by Charlie Sowerby on 8/22/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include <stdio.h>
#include "defs.h"
#include "GameState.h"
#include "gui.h"

/*------------ GLOBAL VARIABLES ------------ */

BitBoard knightMoves[64] = {0};
BitBoard kingMoves[64] = {0};
BitBoard slidingMoves[64][256] = {0};
BitBoard pawnCaptures[64][2] = {0};
uint64_t zobristTable[64][12] = {0};

const char *chessPieces[13] = {"\u2659","\u2658","\u2657","\u2656"," \u2655","\u2654","\u265F","\u265E","\u265D","\u265C","\u265B","\u265A", " "};

/* ------------- INITIALIZATION METHODS -------------*/

BitBoard* preCalcKnightMoves(BitBoard knightMoves[64]){
    for (int i = 0; i < 64; i++){
        int rank = i / 8;
        int file = i % 8;
        
        if (rank + 2 < 8 && file + 1 < 8){
            setBit((knightMoves + i), i + 17);
        }
        if (rank + 2 < 8 && file - 1 >= 0){
            setBit((knightMoves + i), i + 15);
        }
        if (rank + 1 < 8 && file + 2 < 8){
            setBit((knightMoves + i), i + 10);
        }
        if (rank + 1 < 8 && file - 2 >= 0){
            setBit((knightMoves + i), i + 6);
        }
        if (rank - 1 >= 0 && file + 2 < 8){
            setBit((knightMoves + i), i - 6);
        }
        if (rank - 1 >= 0 && file - 2 >= 0){
            setBit((knightMoves + i), i - 10);
        }
        if (rank - 2 >= 0 && file - 1 >= 0){
            setBit((knightMoves + i), i - 17);
        }
        if (rank - 2 >= 0 && file + 1 < 8){
            setBit((knightMoves + i), i - 15);
        }
    }
    return knightMoves;
}

BitBoard* preCalcKingMoves(BitBoard kingMoves[64]){
    for (int i = 0; i < 64; i++){
        int rank = i / 8;
        int file = i % 8;
        
        if (rank == 7){
            if (file == 7){
                setBit((kingMoves + i), i-1);
                setBit((kingMoves + i), i-8);
                setBit((kingMoves + i), i-9);
            }else if (file == 0){
                setBit((kingMoves + i), i+1);
                setBit((kingMoves + i), i-8);
                setBit((kingMoves + i), i-7);
            }else {
                setBit((kingMoves + i), i-1);
                setBit((kingMoves + i), i+1);
                setBit((kingMoves + i), i-7);
                setBit((kingMoves + i), i-8);
                setBit((kingMoves + i), i-9);
            }
        }else if (rank == 0){
            if (file == 7){
                setBit((kingMoves + i), i+7);
                setBit((kingMoves + i), i+8);
                setBit((kingMoves + i), i-1);
            }else if (file == 0){
                setBit((kingMoves + i), i+8);
                setBit((kingMoves + i), i+9);
                setBit((kingMoves + i), i+1);
            }else {
                setBit((kingMoves + i), i+7);
                setBit((kingMoves + i), i+8);
                setBit((kingMoves + i), i+9);
                setBit((kingMoves + i), i-1);
                setBit((kingMoves + i), i+1);
            }
        }else {
            if (file == 7){
                setBit((kingMoves + i), i+7);
                 setBit((kingMoves + i), i+8);
                 setBit((kingMoves + i), i-1);
                 setBit((kingMoves + i), i-8);
                 setBit((kingMoves + i), i-9);
            }else if (file == 0){
                setBit((kingMoves + i), i+8);
                setBit((kingMoves + i), i+9);
                setBit((kingMoves + i), i+1);
                setBit((kingMoves + i), i-7);
                setBit((kingMoves + i), i-8);
            }else {
                setBit((kingMoves + i), i+7);
                setBit((kingMoves + i), i+8);
                setBit((kingMoves + i), i+9);
                setBit((kingMoves + i), i-1);
                setBit((kingMoves + i), i+1);
                setBit((kingMoves + i), i-7);
                setBit((kingMoves + i), i-8);
                setBit((kingMoves + i), i-9);
            }
        }
    }
    
    return kingMoves;
}


void preCalcSlidingMoves(BitBoard rankMoves[64][256]){
    // Rankmoves in indexed by rankMoves[square][occNum]
    for (int square = 0; square < 64; square++){
        for (int occNum = 0; occNum < 256; occNum++){
            //int rank = square / 8;
            int file = square % 8;
            
            //printf("Trying square = %i and occNum %i\n", square, occNum);
            
            for (int i = 1; i + file < 8; i++){
                setBit((&rankMoves[square][occNum]), square + i);
                if (occNum >> (7 - file - i) & 1){
                    break;
                }
            }
            for (int i = -1; file + i >= 0; i--){
                    setBit((&rankMoves[square][occNum]), square + i);
                    if (occNum >> (7 - file - i) & 1){
                        break;
                }
            }
            
            
            //printf("Finished rankMoves for square = %i and occNum = %i \n", square, occNum);
        }
        
    }
}

void preCalcPawnCaptures(BitBoard pawnCaptures[64][2]){
    // Indexed by square and pawn color. WHITE = 0, BLACK = 1.
    for(int i = 0; i < 64; i++){
        int file = i % 8;
        int rank = i/8;
        if (file != 0){
            if (rank != 0){
                setBit(&pawnCaptures[i][1], i-9);
            }
            if(rank !=7){
                setBit(&pawnCaptures[i][0], i + 7);
            }
        }
        if (file != 7){
            if (rank != 0){
                setBit(&pawnCaptures[i][1], i - 7);
            }
            if(rank !=7){
                setBit(&pawnCaptures[i][0], i + 9);
            }
        }
    }
}



void initZobrist(uint64_t zobristTable[64][12] ){
    // GENERATE RANDOM NUMBERS TO FILL THE ZOBRIST TABLE.
    
    // Probably just want to calculate once and leave them here.
}





/* INITIALIZATION REQUIRED FOR PLAYING A GAME */
void init(){
    printf("Initializing Game...\n");

    
    printf("Precalculating Knight Move Tables...\n");
    preCalcKnightMoves(knightMoves);
    printf("Precalculating King Move Tables...\n");
    preCalcKingMoves(kingMoves);
    printf("Precalculating Rank Move Tables...\n");
    preCalcSlidingMoves(slidingMoves);
    printf("Precalculating Pawn Capture Tables...\n");
    preCalcPawnCaptures(pawnCaptures);
    
    initZobrist(zobristTable);
    
    
    
}
