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

int badEvaluation(GameState *gs);

Move encodeMove(Square startSq, Square endSq);

int decodeMove(Move move);
int moveGen(Move *moveList, GameState *gs); 

#endif /* engine_h */
