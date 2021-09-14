//
//  init.c
//  BernieBot
//
//  Created by Charlie Sowerby on 8/22/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include <stdio.h>
#include "defs.h"
#include "magic.h"
#include "GameState.h"


/*--------- GLOBAL VARIABLES INITIALIZATION -------- */

BitBoard knightMoves[64] = {0};
BitBoard kingMoves[64] = {0};


BitBoard pawnCaptures[64][2] = {0};
uint64_t zobristTable[64][12] = {0};


// Magics
sMagic rookMagics[64];
sMagic bishopMagics[64];
BitBoard attacks[ATTACK_LENGTH] = {0}; // 107684 Bitboards = ~800 kB of attacks

const char *chessPieces[13] = {"\u2659","\u2658","\u2657","\u2656"," \u2655","\u2654","\u265F","\u265E","\u265D","\u265C","\u265B","\u265A", " "};


/* --------- DEFAULT METHODS --------------- */

void printMoveInfo(Move *move){
    Square targetSquare = 63 & (*move >> 4);
    Square originSquare = (*move >> 10);
    char str[3];
    printf("Move info: %i\n", *move);
    square_num_to_coords(str, originSquare);
    printf("Origin Square: %s\n", str);
    square_num_to_coords(str, targetSquare);
    printf("Target Square: %s\n", str);
    printf("Special Flags (promo, cap, sp0, sp1): ");
    for (int i = 3; i>= 0; i--){
        printf("%i", getBit((BitBoard*)move, i));
    }
    printf("\n\n");
}


const char* square_num_to_coords(char *str, int num){
    int rank = num / 8;
    int file = num % 8;
    
    str[0] = 'a' + file;
    str[1] = '1' + rank;
    str[2] = '\0';

    return str;
}

int square_coords_to_num(int rank, char file){
    return 8 * rank + (file - 'a');
}

char pieceNumToChar(int num){
    /* Function that returns the piece symbol given the index of bitboard
     i.e. bitboard 0 -> white pawns
          bitboard 3 -> black knight
     
     order is Pawns, Knights, Bishops, Rooks, Queens, Kings (white then black)
     */
    char p = '\0';
    switch (num) {
        case 0:
            // White Pawn
            p = 'P';
            break;
        case 1:
            // White Knight
            p = 'N';
            break;
        case 2:
            // White Bishop
            p = 'B';
            break;
        case 3:
            // White Rook
            p = 'R';
            break;
        case 4:
            // White Queen
            p = 'Q';
            break;
        case 5:
            // White King
            p = 'K';
            break;
        case 6:
            // Black Pawn
            p = 'p';
            break;
        case 7:
            // Black Knight
            p = 'n';
            break;
        case 8:
            // Black Bishop
            p = 'b';
            break;
        case 9:
            // Black Rook
            p = 'r';
            break;
        case 10:
            // Black Queen
            p = 'q';
            break;
        case 11:
            // Black King
            p = 'k';
            break;
        case no_pce:
            p = ' ';
            break;
    }
    return p;
}


int fenCharToNum(char fChar){
    /* Takes character in FEN string and converts to piece number */
    switch (fChar) {
        case 'P':
            return 0;
            break;
        case 'N':
            return 1;
            break;
        case 'B':
            return 2;
            break;
        case 'R':
            return 3;
            break;
        case 'Q':
            return 4;
            break;
        case 'K':
            return 5;
            break;
        case 'p':
            return 6;
            break;
        case 'n':
            return 7;
            break;
        case 'b':
            return 8;
            break;
        case 'r':
            return 9;
            break;
        case 'q':
            return 10;
            break;
        case 'k':
            return 11;
            break;
        default:
            assert(false);
            return -1;
            break;
    }
}
/* ------------- INITIALIZATION METHODS -------------*/

void preCalcKnightMoves(BitBoard knightMoves[64]){
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
}

void preCalcKingMoves(BitBoard kingMoves[64]){
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
}






void initZobrist(uint64_t zobristTable[64][12] ){
    // GENERATE RANDOM NUMBERS TO FILL THE ZOBRIST TABLE.
    // TODO: -
    // Probably just want to calculate once and leave them here.
}


// Magics Precalculation

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



void init_magics(sMagic rookMagics[64], sMagic bishopMagics[64], BitBoard attacks[ATTACK_LENGTH]){
    
    BitBoard bishopMasks[64] = {0};
    BitBoard rookMasks[64] = {0};
    
    generateRookMasks(rookMasks);
    generateBishopMasks(bishopMasks);
    
    // Loop and fill sMagics
    int attackCounter = 0;
    
    // ROOKS
    for(Square i = a1; i <= h8; i++){
        // Pointer to AttackList
        rookMagics[i].attackPtr = attacks + attackCounter;
        attackCounter += 1 << RBits[i];
        
        // Mask
        rookMagics[i].mask = rookMasks[i];
        
        // Magic Number
        rookMagics[i].magic = rMagicNumbers[i];
        
        // Shift
        rookMagics[i].shift = 64 - RBits[i];
    }
    // BISHOPS
    for(Square i = a1; i <= h8; i++){
        // Pointer to AttackList
        bishopMagics[i].attackPtr = attacks + attackCounter;
        attackCounter += 1 << BBits[i];
        
        // Mask
        bishopMagics[i].mask = bishopMasks[i];
        
        // Magic Number
        bishopMagics[i].magic = bMagicNumbers[i];
        
        // Shift
        bishopMagics[i].shift = 64 - BBits[i];
    }
    //printf("Attack counter: %i", attackCounter); 
}

BitBoard genRookAttacks(Square current_sq, BitBoard blockers){
    // Stole this https://www.chessprogramming.org/Looking_for_Magics
    uint64_t result = 0ULL;
    int rk = current_sq/8, fl = current_sq%8, r, f;
    for(r = rk+1; r <= 7; r++) {
      result |= (1ULL << (fl + r*8));
      if(blockers & (1ULL << (fl + r*8))) break;
    }
    for(r = rk-1; r >= 0; r--) {
      result |= (1ULL << (fl + r*8));
      if(blockers & (1ULL << (fl + r*8))) break;
    }
    for(f = fl+1; f <= 7; f++) {
      result |= (1ULL << (f + rk*8));
      if(blockers & (1ULL << (f + rk*8))) break;
    }
    for(f = fl-1; f >= 0; f--) {
      result |= (1ULL << (f + rk*8));
      if(blockers & (1ULL << (f + rk*8))) break;
    }
    return result;
}

BitBoard genBishopAttacks(Square current_sq, BitBoard blockers){
    // Stole this https://www.chessprogramming.org/Looking_for_Magics
    uint64_t result = 0ULL;
    int rk = current_sq/8, fl = current_sq%8, r, f;
    for(r = rk+1, f = fl+1; r <= 7 && f <= 7; r++, f++) {
      result |= (1ULL << (f + r*8));
      if(blockers & (1ULL << (f + r * 8))) break;
    }
    for(r = rk+1, f = fl-1; r <= 7 && f >= 0; r++, f--) {
      result |= (1ULL << (f + r*8));
      if(blockers & (1ULL << (f + r * 8))) break;
    }
    for(r = rk-1, f = fl+1; r >= 0 && f <= 7; r--, f++) {
      result |= (1ULL << (f + r*8));
      if(blockers & (1ULL << (f + r * 8))) break;
    }
    for(r = rk-1, f = fl-1; r >= 0 && f >= 0; r--, f--) {
      result |= (1ULL << (f + r*8));
      if(blockers & (1ULL << (f + r * 8))) break;
    }
    return result;
}



void init_attacks(BitBoard attacks[ATTACK_LENGTH]){
    // LOOP THROUGH ROOK SQUARES
    for (Square i = a1; i <= h8; i++){
        //printf("Starting rook square: %i\n", i);
        
        // Generate all Possible ROOK Blockers for that square
        BitBoard blockers[16384] = {0ULL};
        int rank = i/8;
        int file = i%8;

        for(BitBoard j = 0; j < 16384; j++){
            int plus1 = 0;
            for(int k = 0; k < 7; k++){
                if (k == rank){
                    plus1 = 1;
                }
                if(getBit(&j, k)){
                    setBit(&blockers[j], 8 * (k + plus1) + file);
                }
            }
            plus1 = 0;
            for(int k = 0; k < 7; k++){
                if (k == file){
                    plus1 = 1;
                }
                if(getBit(&j, k+7)){
                    setBit(&blockers[j], (8 * rank) + k + plus1);
                }
            }
            //printf("occupancy number: %i\n", j);
            //printBitBoard(&blockers[j], 0);
        }
        
        // Fill rook attacks
        for (int j = 0; j < (16384); j++){
            BitBoard occ = rookMagics[i].mask & blockers[j];
            occ *= rookMagics[i].magic;
            occ >>= rookMagics[i].shift;
            rookMagics[i].attackPtr[occ] = genRookAttacks(i, blockers[j]);
        }
    }
    
    // LOOP THROUGH BISHOP SQUARES
    for(Square i = a1; i <= h8; i++){
        //printf("Starting bishop square: %i\n", i);
        BitBoard blockers2[8192] = {0ULL};
        int rank = i/8;
        int file = i % 8;
        
        // Calculate list of squares that are accesible by this bishop.
        Square sqList[13] = {no_sqr, no_sqr,no_sqr,no_sqr,no_sqr,no_sqr,no_sqr,no_sqr,no_sqr,no_sqr,no_sqr,no_sqr,no_sqr,};
        int index = 0;
        for(int r = rank+1, f = file+1; r <= 7 && f <= 7; r++, f++) {
            sqList[index] = 8 * r + f;
            index++;
        }
        for(int r = rank-1, f = file-1; r >= 0 && f >= 0; r--, f--) {
            sqList[index] = 8 * r + f;
            index++;
        }
        for(int r = rank+1, f = file-1; r <= 7 && f >= 0; r++, f--) {
            sqList[index] = 8 * r + f;
            index++;
        }
        for(int r = rank-1, f = file+1; r >= 0 && f <= 7; r--, f++) {
            sqList[index] = 8 * r + f;
            index++;
        }
        
        // Set array of all possible blockers for bishop moves
        for(BitBoard j = 0ULL; j < (1 << index); j++){
            for(int k = 0; k < index; k++){
                if(getBit(&j, k)){
                    setBit(&blockers2[j], sqList[k]);
                }
            }
            
        }
        //Fill bishop attacks
        for (int j = 0; j < (1 << index); j++){
            BitBoard occ = bishopMagics[i].mask & blockers2[j];
            occ *= bishopMagics[i].magic;
            occ >>= bishopMagics[i].shift;
            bishopMagics[i].attackPtr[occ] = genBishopAttacks(i, blockers2[j]);
        }
        

    }
}







/* INITIALIZATION REQUIRED FOR PLAYING A GAME */
void init(){

    preCalcKnightMoves(knightMoves);
    preCalcKingMoves(kingMoves);
    initZobrist(zobristTable);
    init_magics(rookMagics, bishopMagics, attacks);
    
    
    init_attacks(attacks);
    

    
}
