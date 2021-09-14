//
//  GameState.c
//  BernieBot
//
//  Created by Charlie Sowerby on 8/21/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include "gamestate.h"


/* ------------ BITBOARD METHODS -------------- */

inline void setBit(BitBoard *board, int bitPos){
    *board |= (1ULL << bitPos);
}

inline void clearBit(BitBoard *board, int bitPos){
    *board &= ~(1ULL << bitPos);
}

inline void switchBit(BitBoard *board, int bitPos){
    *board ^= (1ULL << bitPos);
}

inline bool getBit(BitBoard *board, int bitPos){
    return *board & (1ULL << bitPos);
}

inline uint8_t get_ls1b_pos(BitBoard *board){
    // Using the deBruijin algorithm
    // http://supertech.csail.mit.edu/papers/debruijn.pdf
    //return (uint8_t) DeBruijnBitPosition[((uint64_t)((*board & -(*board)) * debrujin64)) >> 58];

        // I WAS using the deBruijin algorithm, but I decided to use these x86 instructions instead
    return __builtin_ffsll(*board)-1;
}

inline void clear_ls1b(BitBoard *board){
    *board = *board & (*board - 1);
}

inline uint8_t get_num_1b(BitBoard board){
    return __builtin_popcountll(board);
};



/* ------- GAMESTATE INITIALIZATION --------- */
void init_GameState(GameState *gs, char *fen){
    // Fill BitBoards with zero
    for (int i = 0; i < NUM_BOARDS; i++){
        gs->boards[i] = 0ULL;

    }

    // Fill PCELIST with Zero - NOTE: THIS FUNCTIONALITY IS BEING REMOVED IN THE FUTURE
    for (int i = 0; i < 12; i ++){
        for (int j = 0; j < 10; j++){
            gs->pceList[i][j] = no_sqr;
        }
        gs->numPieces[i] = 0;
    }

    //Fill squareOccupancy table with no_pce
    for (int i = 0; i < 64; i++){
        gs->squareOccupancy[i] = no_pce;
    }

    gs->histIndex = 0;

    gs-> castlingPrivileges = 0;
    gs-> enPassantTarget = no_sqr;
    gs-> fiftyMovePly = 0;
    gs-> plyNum = 0;
    gs-> posKey = 0ULL;

    // Initialize GameState from FEN
    if (fen == NULL){
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        //printf("Initializing Starting Position... \n\n");
    }else{
        //printf("Initializing with FEN Inputted: %s\n\n", fen);
    }

    // PARSING FEN
    int i;
    int rank = 7;
    int file = 0;
    // Board Positions
    for (i = 0; fen[i] != ' '; i++){
        //printf("Character Registered! %c\n", fen[i]);
        if (fen[i] == '/'){
            rank--;
            file = 0;
            continue;
        }else if(isdigit(fen[i])){
            // Digit -> skip number of spots
            int digit = fen[i] - '0';
            assert(0 < digit && digit < 9);
            file = file + digit;
            if (file > 7){
                file = 0;
                continue;
            }
        }else{
            // Piece needs to be appended

            int piece_num = fenCharToNum(fen[i]);
            int square_num = 8* rank + file;
            // Add Piece to squareOccupancy
            gs->squareOccupancy[square_num] = piece_num;

            // add piece to numPieces
            gs-> numPieces[piece_num]++;
            // add piece and square to pceList;
            for (int j = 0; j<10; j++){
                if (gs->pceList[piece_num][j] == no_sqr){
                    // append piece, then break
                    gs->pceList[piece_num][j] = square_num;
                    break;
                }
            }

            // Add Piece to BitBoard

            setBit(&gs->boards[piece_num], square_num);
            if (piece_num < 6){
                setBit(&gs->boards[wPieces], square_num);
            }else{
                setBit(&gs->boards[bPieces], square_num);
            }
            setBit(&gs->boards[aPieces], square_num);
            file++;
        }
    }





    // WHO IS ON MOVE:
    i++;
    //printf("Character Registered! %c\n", fen[i]);
    if (fen[i] == 'w'){
        gs->whiteToMove = true;
    }else if (fen[i] == 'b'){
        gs-> whiteToMove = false;
    }else{
        assert(false);
        // Only two legal characters here
    }
    i = i +2;
    //CASTLING RIGHTS
    for (; fen[i] != ' '; i++){
        //printf("Character Registered! %c\n", fen[i]);
        switch (fen[i]) {
            case 'K':
                gs-> castlingPrivileges += w_short_castle;
                break;
            case 'k':
                gs-> castlingPrivileges += b_short_castle;
                break;
            case 'Q':
                gs-> castlingPrivileges += w_long_castle;
                break;
            case 'q':
                gs-> castlingPrivileges += b_long_castle;
                break;
            case '-':
                break;
            default:
                assert(false);
                // only 4 legal characters here.
                break;
        }
    }
    i++;
    // En Passant Target .
    int rank_num = -1;
    char file_char = -1;
    for(; fen[i] != ' '; i++){
        if (fen[i] == '-'){
            i++;
            break;
        }else if(isdigit(fen[i])){
            rank_num = fen[i] - '0';
        }else{
            file_char = fen[i];
        }
    }
    if (rank_num == -1 && file_char == -1){
        gs->enPassantTarget = no_sqr;
    }else{
        gs-> enPassantTarget = square_coords_to_num(rank_num-1, file_char);
    }
    i++;

    // Update the 50 move ply rule


    for(; fen[i] != ' '; i++){
        //printf("50 Move Ply Character Registered! %c\n", fen[i]);
        gs-> fiftyMovePly = 10 * gs->fiftyMovePly + (fen[i] - '0');
    }
    i++;
    // Update the gamemove number
    int gameMove = 0;
    for(; fen[i] != '\0'; i++){
        //printf("GameMove Num Character Registered! %c\n", fen[i]);
        gameMove = 10 * gameMove + (fen[i] - '0');
    }
    int ply = (gameMove-1) * 2;
    if (!gs->whiteToMove){
        ply++;
    }
    gs->plyNum = ply;

    if (fen != NULL){
        // ASSERT GENERATED FEN IS ACCURATE
        char generatedFEN[200];
        generateFEN(gs, generatedFEN);
        for(int j = 0; fen[j]!= '\0'; j++){
            assert(fen[j] == generatedFEN[j]);
        }

    }
}





/* ------------------- METHODS FOR PRINTING GAMESTATE --------------- */
void printGameStateInfo(GameState *gs, bool printBitBoards){
    // BitBoards
    char FEN[200];
    printf("FEN: %s", generateFEN(gs, FEN));
    if (printBitBoards){
        printf("\n -- GAMESTATE INFO -- \n\n");
        printf("BitBoards: (wPawns, wKnights, wBishops, wRooks, wQueens, wKings, bPawns, ... , wPieces, bPieces \n");
        for (int i = 0; i < NUM_BOARDS; i++){
            printBitBoard(gs->boards + i);
        }
    }


    printf("Move: %i, Ply: %i\n", gs->plyNum/2 + 1, gs->plyNum);
    printf("White To Move?: %i\n", gs->whiteToMove);
    printf("Castling (KQkq): %i%i%i%i\n", getBit((BitBoard *)&gs->castlingPrivileges, 3), getBit((BitBoard *)&gs->castlingPrivileges, 2), getBit((BitBoard *)&gs->castlingPrivileges, 1), getBit((BitBoard *)&gs->castlingPrivileges, 0));
    printf("Fifty Move Ply Counter: %i\n", gs->fiftyMovePly);

    char ep_square[3];

    printf("EP Square %s\n", square_num_to_coords(ep_square, gs->enPassantTarget));
    printf("\nGame board:\n");
    printGameBoard(gs);
}

void printGameBoard(GameState *gs){

    char top_bot_string[] = "+---+---+---+---+---+---+---+---+\n";

    // CALCULATE PIECE CHARACTER STRING

    // PUT PIECES INTO THE BOARD AND PRINT IT:
    for(int rank = 7; rank >= 0; rank--){
        printf("%s", top_bot_string);
        for(int file = 0; file < 8; file++){
            printf("| %c ", pieceNumToChar(gs->squareOccupancy[8 * rank + file]));
        }
        printf("|\n");
    }
    printf("%s", top_bot_string);
}

void printBitBoard(BitBoard* bBoard){
    // Print out bitboard
    for (int rank = 7; rank >= 0; rank--){
        for (int file = 0; file < 8; file ++){
            printf("%i ", getBit(bBoard, 8 * rank + file));
        }
        printf("\n");
    }
    printf("\n");
}

char* generateFEN(GameState * gs, char* FEN){
    int i = 0;
    int spaces = 0;
    for(int rank = 7; rank >= 0; rank--){
        for(int file = 0; file < 8; file++){
            Piece currentPiece = gs->squareOccupancy[8 * rank + file];
            if (currentPiece == no_pce){
                spaces++;
            }else{
                if(spaces != 0){
                    FEN[i] = '0' + spaces;
                    i++;
                }
                FEN[i] = pieceNumToChar(currentPiece);
                spaces = 0;
                i++;
            }
        }
        if(spaces != 0){
            FEN[i] = '0' + spaces;
            i++;
        }
        FEN[i] = '/';
        spaces = 0;
        i++;
    }
    i--;

    // TURN TO MOVE
    FEN[i] = ' ';
    i++;
    if(gs->whiteToMove){
        FEN[i] = 'w';
    }else{
        FEN[i] = 'b';
    }
    i++;

    //Castling Rights
    FEN[i] = ' ';
    i++;
    if(getBit((BitBoard*)&gs->castlingPrivileges, 3)){
        FEN[i] = 'K';
        i++;
    }
    if(getBit((BitBoard*)&gs->castlingPrivileges, 2)){
        FEN[i] = 'Q';
         i++;
     }
    if(getBit((BitBoard*)&gs->castlingPrivileges, 1)){
        FEN[i] = 'k';
         i++;
     }
    if(getBit((BitBoard*)&gs->castlingPrivileges, 0)){
        FEN[i] = 'q';
         i++;
     }
    if(FEN[i-1] == ' '){
        FEN[i] = '-';
        i++;
    }

    //En Passant
    FEN[i] = ' ';
    i++;
    char epSq[3];
    square_num_to_coords(epSq, gs->enPassantTarget);
    if(gs->enPassantTarget == no_sqr){
        FEN[i] = '-';
        i++;
    }else{
        FEN[i] = epSq[0];
        i++;
        FEN[i] = epSq[1];
        i++;
    }

    // 50 move ply rule
    FEN[i] = ' ';
    i++;
    if(gs->fiftyMovePly/10){
        FEN[i] = gs->fiftyMovePly/10 + '0';
        i++;
    }
    FEN[i] = gs->fiftyMovePly % 10 + '0';
    i++;
    FEN[i] = ' ';
    i++;

    int gameMove = gs->plyNum/2 + 1;

    if(gameMove/10){
        FEN[i] = gameMove/10 + '0';
        i++;
    }
    FEN[i] = gameMove % 10 + '0';
    i++;










    FEN[i] = '\0';
    return FEN;
}


int compareGameStates(GameState *gs1, GameState *gs2){

    for(int i = 0; i < NUM_BOARDS; i++){
        if(gs1->boards[i] != gs2->boards[i]){
            return -i - 100; ;
        }
    }

    for(int i = 0; i < 64; i++){
        if(gs1->squareOccupancy[i] != gs2->squareOccupancy[i]){
            return i + 100;
        }
    }

    return 0;
}
