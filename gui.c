//
//  gui.c
//  BernieBot
//
//  Created by Charlie Sowerby on 8/21/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include "gui.h"


int fenCharToNum(char fChar){
    // Order of Pieces are P, N, B, R, Q, K, p, n, b, r, q, k
    switch (fChar) {
        case 'P':
            return 0;
            break;
        case 'N':
            return 1;
            break;
        case 'B':
            return 2;
            break;
        case 'R':
            return 3;
            break;
        case 'Q':
            return 4;
            break;
        case 'K':
            return 5;
            break;
        case 'p':
            return 6;
            break;
        case 'n':
            return 7;
            break;
        case 'b':
            return 8;
            break;
        case 'r':
            return 9;
            break;
        case 'q':
            return 10;
            break;
        case 'k':
            return 11;
            break;
        default:
            assert(false);
            return -1;
            break;
    }
}




/* ---------------------------------------------------- */
/* -------------PRINT GRAPHICAL REPRESENTATION--------- */
/* ---------------------------------------------------- */
void printGameBoard(GameState *gs){

    char top_bot_string[] = "+---+---+---+---+---+---+---+---+\n";
    
    // CALCULATE PIECE CHARACTER STRING
    char pieces[64];
    for (int i = 0; i < 64; i++){
        pieces[i] = '.';
    }
    for (int i = 0; i < 12; i++){
        for (int j = 0; j < 10; j++)
            if (gs->pceList[i][j] == 64){
                break;
            }else{
                pieces[gs->pceList[i][j]] = pieceNumToChar(i);
            }
    }
    
    // PUT PIECES INTO THE BOARD AND PRINT IT:
    for(int rank = 7; rank >= 0; rank--){
        printf("%s", top_bot_string);
        for(int file = 0; file < 8; file++){
            printf("| %c ", pieces[8 * rank + file]);
        }
        printf("|\n");
    }
    printf("%s", top_bot_string);
    
    
}

/* ---------------------------------------------------- */
/* ---------------------------------------------------- */
/* ---------------------------------------------------- */



char pieceNumToChar(int num){
    /* Function that returns the piece symbol given the index of bitboard
     i.e. bitboard 0 -> white pawns
          bitboard 3 -> black knight
     
     order is Pawns, Knights, Bishops, Rooks, Queens, Kings (white then black)
     */
    char p = '\0';
    switch (num) {
        case 0:
            // White Pawn
            p = 'P';
            break;
        case 1:
            // White Knight
            p = 'N';
            break;
        case 2:
            // White Bishop
            p = 'B';
            break;
        case 3:
            // White Rook
            p = 'R';
            break;
        case 4:
            // White Queen
            p = 'Q';
            break;
        case 5:
            // White King
            p = 'K';
            break;
        case 6:
            // Black Pawn
            p = 'p';
            break;
        case 7:
            // Black Knight
            p = 'k';
            break;
        case 8:
            // Black Bishop
            p = 'b';
            break;
        case 9:
            // Black Rook
            p = 'r';
            break;
        case 10:
            // Black Queen
            p = 'q';
            break;
        case 11:
            // Black King
            p = 'k';
            break;
    }
    return p;
}


void printBitBoard(BitBoard* bBoard, int nBitBoard){
    // Print out bitboard in array bBoard with index nBitBoard.
    for (int rank = 7; rank >= 0; rank--){
        for (int file = 0; file < 8; file ++){
            printf("%i ", getBit((bBoard + nBitBoard), 8 * rank + file));
        }
        printf("\n");
    }
    printf("\n");
    
}

const char* square_num_to_coords(char *str, int num){
    int rank = num / 8;
    int file = num % 8;
    
    str[0] = 'a' + file;
    str[1] = '1' + rank;

    return str;
}

int square_coords_to_num(int rank, char file){
    return 8 * rank + (file - 'a');
}


