//
//  movegen.h
//  BernieBot
//
//  Created by Charlie Sowerby on 9/12/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#ifndef movegen_h
#define movegen_h


#include <stdio.h>
#include "engine.h"
#include "defs.h"
#include "gamestate.h"

int inCheck(Square kingPos, GameState* gs);

// Move Generation

int moveGen(Move move_list[MOVE_LIST_LENGTH], GameState *gs);

static inline int encodeMoves(Move move_list[MOVE_LIST_LENGTH], int moveCount, int moveType, BitBoard moves, Square origin_sq);
static inline BitBoard getBishopMoveBoard(Square origin_sq, GameState* gs);
static inline BitBoard getRookMoveBoard(Square origin_sq, GameState* gs);
static inline int calcPawnMoves(Move move_list[MOVE_LIST_LENGTH], int moveCount, GameState *gs);

// Inline function definitions
static inline BitBoard getBishopMoveBoard(Square origin_sq, GameState* gs){
    /*
    Purpose:
        Get a bitboard with all possible bishop moves for a given square using magic bitboards (including captures of all enemy/friendly pieces)
    Params:
        Square origin_sq    - common origin square of bishop move board
        GameState* gs       - pointer to current gamestate
    Returns:
        BitBoard            - board of bishop moves
    */
    BitBoard occ = gs->boards[aPieces];
    occ &= bishopMagics[origin_sq].mask;
    occ *= bishopMagics[origin_sq].magic;
    occ >>= bishopMagics[origin_sq].shift;
    return bishopMagics[origin_sq].attackPtr[occ];
}

static inline BitBoard getRookMoveBoard(Square origin_sq, GameState* gs){
    /*
    Purpose:
        Get a bitboard with all possible rook moves for a given square using magic bitboards (including captures of all enemy/friendly pieces)
    Params:
        Square origin_sq    - common origin square of rook move board
        GameState* gs       - pointer to current gamestate
    Returns:
        BitBoard            - board of rook moves
    */
    BitBoard occ = gs->boards[aPieces];
    occ &= rookMagics[origin_sq].mask;
    occ *= rookMagics[origin_sq].magic;
    occ >>= rookMagics[origin_sq].shift;
    return rookMagics[origin_sq].attackPtr[occ];
}
#endif /* movegen_h */
