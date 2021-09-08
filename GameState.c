//
//  GameState.c
//  BernieBot
//
//  Created by Charlie Sowerby on 8/21/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include "GameState.h"


/* ------------ BITBOARD METHODS -------------- */

void setBit(BitBoard *board, int bitPos){
    uint64_t flag = 1;
    *board |= flag << bitPos;
}

void clearBit(BitBoard *board, int bitPos){
    uint64_t flag = 1;
    *board &= ~(flag << bitPos);
}

bool getBit(BitBoard *board, int bitPos){
    uint64_t flag = 1;
    if (*board & flag << bitPos){
        return true;
    }else{
        return false;
    }
}

uint8_t get_ls1b_pos(BitBoard *board){
    // Using the deBruijin algorithm
    // http://supertech.csail.mit.edu/papers/debruijn.pdf
    return (uint8_t) DeBruijnBitPosition[((uint64_t)((*board & -(*board)) * debrujin64)) >> 58];
}








/* ------- GAMESTATE INITIALIZATION --------- */
void init_GameState(GameState *gs, char *fen){
    // Fill arrays with zero
    for (int i = 0; i < NUM_BOARDS; i++){
        gs->boards[i] = 0ULL;

    }
    for (int i = 0; i < 12; i ++){
        for (int j = 0; j < 10; j++){
            gs->pceList[i][j] = no_sqr;
        }
        gs->numPieces[i] = 0;
    }
    gs-> castlingPrivileges = 0;
    gs-> enPassantTarget = no_sqr;
    gs-> fiftyMovePly = 0;
    gs-> plyNum = 0;
    gs-> posKey = 0ULL;
    
    // Initialize GameState from FEN
    if (fen == NULL){
        fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        printf("Initializing Starting Position... \n\n");
    }else{
        printf("Initializing with FEN Inputted: %s\n\n", fen);
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
            if (piece_num < 6){
                if (piece_num == 0){
                    setBit(&gs->boards[0], square_num);
                }
                setBit(&gs->boards[2], square_num);
            }else{
                if(piece_num == 6){
                    setBit(&gs->boards[1], square_num);
                }
                setBit(&gs->boards[3], square_num);
            }
            setBit(&gs->boards[4], square_num);
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
        gs-> enPassantTarget = square_coords_to_num(rank_num, file_char);
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
}





int updateGameState(GameState *gs, Move *move){
    /* Update gamestate with the code :
        * 6 bits origin square
        * 6 bits target square
        * 4 bits special code - promo, capture, special 0, special 1
     
     00 - knight
     01 - bishop
     10 - rook
     11 - queen
     
     0000 - quiet move
     0001 - double pawn push
     0010 - kingside castle
     0011 - queenside castle
     0100 - normal cap
     0101 - en passant
         */
    
    Square origin = *move >> 10;
    Square target = (*move >> 4) & 63;
    bool promo = getBit((BitBoard *)move, 3);
    bool capture = getBit((BitBoard*)move, 2);
    bool sp0 = getBit((BitBoard*)move, 1);
    bool sp1 = getBit((BitBoard*)move, 0);
    
    // TODO: THIS IS UNFINISHED 
    
    
    
    printf("origin: %i, target: %i", origin, target);
    return 0;
}



/* ------------------- METHODS FOR PRINTING GAMESTATE --------------- */
void printGameStateInfo(GameState *gs, bool printBitBoards){
    // BitBoards
    if (printBitBoards){
        printf("\n -- GAMESTATE INFO -- \n\n");
        printf("BitBoards: (wPawns, bPawns, wPieces, bPieces, aPieces, wAttacks, bAttacks)...\n");
        for (int i = 0; i < NUM_BOARDS; i++){
            printBitBoard(gs->boards, i);
        }
    }
    
    
    // Piece Info/Locations
    printf("Piece List: \n");
    for(int pce = 0; pce < 12; pce ++ ){
        printf("Piece: %s : ", chessPieces[pce]);
        for (int i = 0; i < 10; i++){
            if (gs->pceList[pce][i] == no_sqr){
                break;
            }
            char str[3];
            printf("%s, ", square_num_to_coords(str, gs->pceList[pce][i]));
        }
        printf("\n");
    }
    printf("\n");
    printf("Move: %i, Ply: %i\n", gs->plyNum/2 + 1, gs->plyNum);
    printf("White To Move?: %i\n", gs->whiteToMove);
    printf("Castling (KQkq): %i%i%i%i\n", getBit((BitBoard *)&gs->castlingPrivileges, 3), getBit((BitBoard *)&gs->castlingPrivileges, 2), getBit((BitBoard *)&gs->castlingPrivileges, 1), getBit((BitBoard *)&gs->castlingPrivileges, 0));
    printf("Fifty Move Ply Counter: %i\n", gs->fiftyMovePly);
    printf("\nGame board:\n");
    printGameBoard(gs);
}

void printGameBoard(GameState *gs){

    char top_bot_string[] = "+---+---+---+---+---+---+---+---+\n";
    
    // CALCULATE PIECE CHARACTER STRING
    char pieces[64];
    for (int i = 0; i < 64; i++){
        pieces[i] = ' ';
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

