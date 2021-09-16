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

inline int encodeMoves(Move move_list[MOVE_LIST_LENGTH], int moveCount, int moveType, BitBoard moves, Square origin_sq);
inline BitBoard getBishopMoveBoard(Square origin_sq, GameState* gs);
inline BitBoard getRookMoveBoard(Square origin_sq, GameState* gs);
inline int calcPawnMoves(Move move_list[MOVE_LIST_LENGTH], int moveCount, GameState *gs);


#endif /* movegen_h */
