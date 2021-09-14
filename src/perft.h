//
//  Perft.h
//  BernieBot
//
//  Created by Charlie Sowerby on 9/12/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#ifndef Perft_h
#define Perft_h


#include <stdio.h>
#include "engine.h"
#include "defs.h"


uint64_t Perft(int depth, GameState *gs);
uint64_t Perft_debug(int depth, GameState *gs);
uint64_t Perft_speedtest(int depth, GameState *gs, uint64_t* moveGeneration, uint64_t* moveMaking, uint64_t* moveUnmaking, uint64_t* abortedMoves );
void startPerft(int depth);

#endif /* Perft_h */
