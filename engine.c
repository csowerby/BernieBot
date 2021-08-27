//
//  engine.c
//  BernieBot
//
//  Created by Charlie Sowerby on 8/22/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include "engine.h"



int badEvaluation(GameState *gs){
    int evaluation = 0;
    int multiple = 100;
    for (int i = 0; i < 6; i++){
        switch (i) {
            case 1:
            case 2:
                multiple = 300;
                break;
            case 3:
                multiple = 500;
                break;
            case 4:
                multiple = 900;
            default:
                break;
        }
        evaluation += gs->numPieces[i] - gs->numPieces[i + 6];
    }
    return evaluation;
}


int moveGen(Move *moveList, GameState *gs){
    // MOST POSSIBLE MOVES IN A POSITION IS 218, probably just going to consider 100 or 150;
    int numMoves = 0;
    // Loop through all maj + min pieces
        // Calculate Their moves using precalculation
        // For pawns calculate only moves
            // Consider only queen promos for now.
    // Calculate pawn captures individually. 
    
    // Use bitmask for pawn captures? 
    
    
    
    return numMoves;
}
