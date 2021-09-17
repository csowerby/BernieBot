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
#include "gamestate.h"
#include "movegen.h"


// Search

Move search(int depth, GameState *gs);
int negaMax(int depth, GameState* gs);

//Evaluation

float naiveEvaluation(GameState *gs);

// Move Making
int makeMove(GameState *gs, Move move);
int unmakeMove(GameState *gs, Move move);
int inCheck(Square kingPos, GameState* gs);


#endif /* engine_h */
