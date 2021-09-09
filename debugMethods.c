//
//  debugMethods.c
//  BernieBot
//
//  Created by Charlie Sowerby on 9/9/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#ifdef DEBUG_METHODS

/*
 FILE FOR A LIBRARY OF DEBUGGING FUNCTIONS
    * Specific Piece Move Generation
 
 
 */

/* ----------------------------------------------------------------*/
/* ------------- Specific Piece Move Generation  ------------------*/
/* ----------------------------------------------------------------*/

// Example for Pawn Moves

init();
GameState gs;
init_GameState(&gs,  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
printGameStateInfo(&gs, false);


Move *moveList = NULL;
int numMoves = calcPawnMoves(&moveList, &gs);
for(int i = 0; i < numMoves; i++){
    printMoveInfo(&moveList[i]);
}





/* ------------------------------------------------------*/



#endif
