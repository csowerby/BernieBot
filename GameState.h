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







/* ----------- GAMESTATE STRUCT --------------- */
typedef struct{
    
    BitBoard boards[NUM_BOARDS]; // wP, bP, wPce, bPce, aPce
    
    /* - WE DON'T WANT THE PCE LIST SHIT - I'M JUST KEEPING IT HERE FOR */
    Square pceList[12][10]; // Represents an array of the 10 possible pieces and it is possible to have 10 of each piece, fill with square numbers for each one, no_sqr
    uint8_t numPieces[12];
    
        // we have knights, bishops, rooks, queens, kings
    
    
    // Lookup of what piece is in a square for easier make/unmake move functions
    uint8_t squareOccupancy[64];
    
    uint8_t plyNum;
    
    // Bitwise castling privileges in order 3 = White short, 2 = white long, 1 = black short, 0 = black long
    
    uint8_t castlingPrivileges;
    
    // En passant target square [0, 64] (64 = no_sqr) if no enPassant is possible
    Square enPassantTarget;
    
    // Number of halfmoves since
    uint16_t fiftyMovePly;
    bool whiteToMove;
    
    uint64_t posKey; // Key of position, to be looked up in transposition table.
        // ZOBRIST HASHING
    
    uint8_t gameHist[25][3];
    int histIndex;
    
}GameState;


/* --------------- GameState Methods ------------- */
void init_GameState(GameState *gs, char *fen);
int updateGameState(GameState *gs, Move*move);



/* ------ METHODS FOR PRINTING GAMESTATE -------*/
void printGameStateInfo(GameState *gs, bool printBitBoards);
void printGameBoard(GameState *gs);
void printBitBoard(BitBoard *bBoard);




#endif /* GameState_h */
