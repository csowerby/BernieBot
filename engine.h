//
//  engine.h
//  BernieBot
//
//  Created by Charlie Sowerby on 8/22/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#ifndef engine_h
#define engine_h

#include <stdio.h>
#include "defs.h"
#include "GameState.h"

//Evaluation

int badEvaluation(GameState *gs);

// Move Making
int makeMove(GameState *gs, Move *move);
int inCheck(Square kingPos, GameState* gs);

// Move Generation

int moveGen(Move **moveList, GameState *gs);

BitBoard getBishopMoveBoard(Square origin_sq, GameState* gs);
BitBoard getRookMoveBoard(Square origin_sq, GameState* gs);

int calcKingMoves(Move **kingList, Square origin_sq, GameState *gs);
int calcKnightMoves(Move **kingList, Square origin_sq, GameState *gs);
int calcRookMoves(Move **slidingList, Square origin_sq, GameState *gs);
int calcBishopMoves(Move **diagList, Square origin_sq, GameState *gs);
int calcPawnMoves(Move **pawnList, GameState *gs);

#endif /* engine_h */
