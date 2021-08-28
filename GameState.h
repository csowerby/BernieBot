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
uint8_t get_ls1b_pos(BitBoard *board);



/* ----------- GAMESTATE STRUCT --------------- */
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
    
    // En passant target square [0, 64] (64 = no_sqr) if no enPassant is possible
    Square enPassantTarget;
    
    // Number of halfmoves since
    uint16_t fiftyMovePly;
    bool whiteToMove;
    
    uint64_t posKey; // Key of position, to be looked up in transposition table.
        // ZOBRIST HASHING
    
}GameState;


/* --------------- GameState Methods ------------- */
void init_GameState(GameState *gs, char *fen);
int updateGameState(GameState *gs, Move*move);

// Rotation Methods
int rotateBitBoard(BitBoard *oldBoard, BitBoard *newBoard);
int diagBitBoard(BitBoard *oldBoard, BitBoard *newBoard);
int adiagBitBoard(BitBoard *oldBoard, BitBoard *newBoard);



/* ------ METHODS FOR PRINTING GAMESTATE -------*/
void printGameStateInfo(GameState *gs);
void printGameBoard(GameState *gs);
void printBitBoard(BitBoard *bBoard, int nBitBoard);




#endif /* GameState_h */
