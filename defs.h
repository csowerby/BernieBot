//
//  BernieBot.h
//  BernieBot
//
//  Created by Charlie Sowerby on 8/21/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#ifndef BernieBot_h
#define BernieBot_h

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <inttypes.h>

#include <stdbool.h>
#include <ctype.h>

#include <assert.h>

/* ------ DEFINITIONS ----------*/

#define NUM_BOARDS 5 // -> Represents White Pawns, Black Pawns, All Pawns?, White Pieces, Black Pieces, All Pieces


/* ----- deBrujin lookup stuff ------- */
#define debrujin64 0x022fdd63cc95386dULL
static const uint8_t DeBruijnBitPosition[64] = {0,
    1,  2, 53,  3,  7, 54, 27, 4, 38, 41,  8, 34, 55, 48, 28,
    62,  5, 39, 46, 44, 42, 22,  9, 24, 35, 59, 56, 49, 18, 29, 11,
    63, 52,  6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,
    51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12};
/* ------------- ENUMS ------------------*/

// SQUARES
enum {a1, b1, c1, d1, e1, f1, g1, h1,
      a2, b2, c2, d2, e2, f2, g2, h2,
      a3, b3, c3, d3, e3, f3, g3, h3,
      a4, b4, c4, d4, e4, f4, g4, h4,
      a5, b5, c5, d5, e5, f5, g5, h5,
      a6, b6, c6, d6, e6, f6, g6, h6,
      a7, b7, c7, d7, e7, f7, g7, h7,
      a8, b8, c8, d8, e8, f8, g8, h8, no_sqr
};

//Pieces
enum {wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK, no_pce};

// Castling
enum {w_short_castle = 8, w_long_castle=4, b_short_castle=2, b_long_castle=1};



/* ------ DECLARATIONS --------*/
typedef uint64_t BitBoard;
typedef uint16_t Move;
typedef uint8_t Square; 

/* ---------- GLOBAL VARIABLES --------- */
extern BitBoard knightMoves[64];
extern BitBoard kingMoves[64];
extern BitBoard slidingMoves[64][256];
extern uint64_t zobristTable[64][12];
extern BitBoard pawnCaptures[64][2];
extern const char *chessPieces[13];


/* ------------ DEFAULT METHODS --------*/
const char* square_num_to_coords(char *str, int num);
int square_coords_to_num(int rank, char file);
char pieceNumToChar(int num);
int fenCharToNum(char fChar);

/* ----------- INITIALIZATION METHODS ------- */


void preCalcKnightMoves(BitBoard knightMoves[64]);
void preCalcKingMoves(BitBoard kingMoves[64]);
void preCalcSlidingMoves(BitBoard rankMoves[64][256]);
void preCalcPawnCaptures(BitBoard pawnCaptures[64][2]);
void initZobrist(uint64_t zobrist_table[64][12] );

void init(void);



 
 
#endif /* BernieBot_h */
