//
//  gui.h
//  BernieBot
//
//  Created by Charlie Sowerby on 8/21/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#ifndef gui_h
#define gui_h




#include "defs.h"
#include "GameState.h"


/* --------- FUNCTION DECLARATIONS -----------------*/
void initializeStartingPos(BitBoard *board);
void printGameBoard(GameState *gs);
char pieceNumToChar(int num);
//void fenToBitBoard(char fen[]);
void printBitBoard(BitBoard* bBoard, int nBitBoard);
const char* square_num_to_coords(char *str, int num);

int square_coords_to_num(int rank, char file);
void printGameBoard(GameState *gs);
#endif /* gui_h */
