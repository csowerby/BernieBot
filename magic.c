//
//  magic.c
//  BernieBot
//
//  Created by Charlie Sowerby on 9/7/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include "magic.h"

sMagic rMagics[64];
sMagic bMagics[64]; 


    // MAGIC SHIT
/* ---------------------------------- */
/* ---------------------------------- */
/* ---------------------------------- */
/* ---------------------------------- */
BitBoard rookMasks[64] = {0};
BitBoard bishopMasks[64] = {0};

int generateRookMasks(BitBoard rookMasks[64]){
    for(int i = 0; i < 64; i++){
        int rank = i/8;
        int file = i%8;
        
        for (int j = 1; rank + j < 7; j++){
            setBit(&rookMasks[i], i + 8*j);
        }
        for (int j = -1; rank + j >= 1; j--){
            setBit(&rookMasks[i], i + 8*j);
        }
        for (int j = 1; file + j < 7; j++){
            setBit(&rookMasks[i], i + j);
        }
        for (int j = -1; file + j >= 1; j--){
            setBit(&rookMasks[i], i + j);
        }
    }
    return 0;
}

int generateBishopMasks(BitBoard bishopMasks[64]){
    for(int i = 0; i < 64; i++){
        int rank = i/8;
        int file = i%8;
        
        for (int j = 1; j + rank < 7 && j + file < 7; j++){
            setBit(&bishopMasks[i], i + 9*j);
        }
        for (int j = 1; rank - j >= 1 && j + file < 7; j++){
            setBit(&bishopMasks[i], i - 7*j);
        }
        for (int j = 1; j + rank < 7 && file - j >= 1; j++){
            setBit(&bishopMasks[i], i + 7*j);
        }
        for (int j = 1; rank - j >= 1 && file - j >= 1; j++){
            setBit(&bishopMasks[i], i - 9*j);
        }
    }
    return 0;
}

/* --------------------------------- */
uint64_t rookMagics[64] = {0};
uint64_t bishopMagics[64] = {0};

BitBoard rookMagicHash(BitBoard masked_board, Square current_sq){
    
    
    return 0ULL;
}




int init_magics(){
    
    
    
    return 0; 
}
