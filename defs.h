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

#include <time.h>

#include <assert.h>
#include <string.h>

/* ------ DEFINITIONS ----------*/

#define NUM_BOARDS 15 // -> Represents White Pawns, White Knights, White Bishops, White Rooks, White Queens, White King, All White Pieces, Black Pawns, Black Knights, Black Bishops, Black Rooks, Black Queens, Black King, All Black Pieces, All Pieces

#define ATTACK_LENGTH 107648


#define rank1 0x00000000000000FFULL
#define rank2 0x000000000000FF00ULL
#define rank3 0x0000000000FF0000ULL
#define rank4 0x00000000FF000000ULL
#define rank5 0x000000FF00000000ULL
#define rank6 0x0000FF0000000000ULL
#define rank7 0x00FF000000000000ULL
#define rank8 0xFF00000000000000ULL

#define notRank8 0x00FFFFFFFFFFFFFFULL
#define notRank1 0xFFFFFFFFFFFFFF00ULL

#define aFile 0x0101010101010101ULL
#define bFile 0x0202020202020202ULL
#define cFile 0x0404040404040404ULL
#define dFile 0x0808080808080808ULL
#define eFile 0x1010101010101010ULL
#define fFile 0x2020202020202020ULL
#define gFile 0x4040404040404040ULL
#define hFile 0x8080808080808080ULL

#define notAFile 0xFEFEFEFEFEFEFEFEULL
#define notHFile 0x7F7F7F7F7F7F7F7FULL

#define w_short_castle_squares 0x0000000000000060ULL
#define w_long_castle_squares 0x0000000000000007ULL

#define b_short_castle_squares 0x6000000000000000ULL
#define b_long_castle_squares 0x0700000000000000ULL


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

// Bitboards
enum bitboards {wPawns, wKnights, wBishops, wRooks, wQueens, wKings, bPawns, bKnights, bBishops, bRooks, bQueens, bKings, wPieces, bPieces, aPieces};

/* ------ DECLARATIONS --------*/
typedef uint64_t BitBoard;
typedef uint16_t Move;
typedef uint8_t Square;
typedef uint8_t Piece; 

/* ------------ MAGIC STRUCT --------------- */

typedef struct{
    BitBoard *attackPtr;
    BitBoard mask;
    uint64_t magic;
    int shift;
}sMagic;


/* ---------- GLOBAL VARIABLES --------- */
extern BitBoard knightMoves[64];
extern BitBoard kingMoves[64];

extern uint64_t zobristTable[64][12];

extern const char *chessPieces[13];

extern sMagic rookMagics[64];
extern sMagic bishopMagics[64]; 
extern BitBoard attacks[107648];



/* ------------ DEFAULT METHODS --------*/
void printMoveInfo(Move *move);
const char* square_num_to_coords(char *str, int num);
int square_coords_to_num(int rank, char file);
char pieceNumToChar(int num);
int fenCharToNum(char fChar);

/* ------ BITBOARD METHODS --------*/

void setBit(BitBoard *board, int bitPos);
void clearBit(BitBoard *board, int bitPos);
void switchBit(BitBoard *board, int bitPos);
bool getBit(BitBoard *board, int bitPos);
uint8_t get_ls1b_pos(BitBoard *board);

/* ----------- INITIALIZATION METHODS ------- */


void preCalcKnightMoves(BitBoard knightMoves[64]);
void preCalcKingMoves(BitBoard kingMoves[64]);

void initZobrist(uint64_t zobrist_table[64][12] );
void init_magics(sMagic rookMagics[64], sMagic bishopMagics[64], BitBoard attacks[ATTACK_LENGTH]);
BitBoard genBishopAttacks(Square current_sq, BitBoard blockers);
BitBoard genRookAttacks(Square current_sq, BitBoard blockers);
void init_attacks(BitBoard attacks[ATTACK_LENGTH]);

void init(void);



 
#endif /* BernieBot_h */
