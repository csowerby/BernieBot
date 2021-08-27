//
//  GameState.h
//  BernieBot
//
//  Created by Charlie Sowerby on 8/21/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#ifndef GameState_h
#define GameState_h

#include <stdio.h>

#include "defs.h"



/* ------ BITBOARD METHODS --------*/

void setBit(BitBoard *board, int bitPos);
void clearBit(BitBoard *board, int bitPos);
bool getBit(BitBoard *board, int bitPos);



/* Structs*/

//struct GameHistory;
//struct GameState;


/* ------------------------------ */
typedef struct GameHistory{
        
}GameHistory;

/* -------------GAMESTATE----------------- */
typedef struct{
    
    BitBoard boards[NUM_BOARDS]; // wP, bP, wPce, bPce, aPce
    BitBoard rot_boards[NUM_BOARDS];
    BitBoard diag_boards[NUM_BOARDS];
    BitBoard adiag_boards[NUM_BOARDS];
    
    Square pceList[12][10]; // Represents an array of the 10 possible pieces and it is possible to have 10 of each piece, fill with square numbers for each one, no_sqr
    uint8_t numPieces[12];
    
        // we have knights, bishops, rooks, queens, kings
    
    uint8_t plyNum;
    
    // Bitwise castling privileges in order White short, white long, black short, black long
    
    uint8_t castlingPrivileges;
    
    // En passant privileges square [0, 64] (64 = no_sqr) if no castling is possible
    Square enPassantTarget;
    uint16_t fiftyMovePly;
    bool whiteToMove;
    
    uint64_t posKey; // Key of position, to be looked up in transposition table.
        // ZOBRIST HASHING
    
}GameState;


/* --------------- GameState Methods ------------- */
void init_GameState(GameState *gs, char *fen);
void printGameStateInfo(GameState *gs);
uint8_t get_ls1b_pos(BitBoard *board);



#endif /* GameState_h */
