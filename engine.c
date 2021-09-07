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

/* ------------ MOVE CALCULATION METHODS ------------- */

int moveGen(Move **moveList, GameState *gs){
    /* Params: Double Pointer to moveList, pointer to gamestate
        returns: integer with length of moveList, -1 if failure, 0 if
     */
    
    
    
    // METHOD: GENERATE ALL PSEUDO-LEGAL MOVES
    
    
    int *p;
    int move_num = 1;
    p = malloc(move_num * sizeof(Move));
    
    
    
    // INT RETURNS NUMBER OF POSSIBLE MOVES
    // MOST POSSIBLE MOVES IN A POSITION IS 218, probably just going to consider 100 or 150;
    int numMoves = 0;
    // Loop through all maj + min pieces
        // Calculate Their moves using precalculation
        // For pawns calculate only moves
            // Consider only queen promos for now?
    // Calculate pawn captures individually. 
    
    // Use bitmask for pawn captures?
    
    
    
    /*
     
     int testFunc(int **moveList){
         
         int length = rand()%10;
         int *p;
         p = (int*) malloc(length * sizeof(int));
         
         if (p == NULL){
             return -1;
         }
         for (int i = 0; i < length; i++){
             p[i] = i * 100;
         }
         *moveList = p;
         return length;
     }

    */
    
    
    return numMoves;
}


int calcKnightMoves(Move **knightList, Square origin_sq, GameState *gs){
    /* Calculate Knight Moves
     Params:
        - **knightList: double pointer to array of knightMoves initialized as null
        - origin_sq: origin square of the moves being considered
        - *gs: current gamestate moves are being generated for
     
     Returns: integer length of knightList that was returned
     
     */
    Move *moveList;
    moveList = malloc(8 * sizeof(Move));
    int moveCount = 0;
    
    // Start with a bitboard of possible moves
    BitBoard moveBoard = 0ULL;
    if (gs->whiteToMove){
        moveBoard = knightMoves[origin_sq] & ~(gs->boards[wPieces]);
    }else{
        moveBoard = knightMoves[origin_sq] & ~(gs->boards[bPieces]);
    }
    
    Square targetSquare;
    while(moveBoard){
        targetSquare = get_ls1b_pos(&moveBoard);
        clearBit(&moveBoard, targetSquare);
        moveList[moveCount] += targetSquare << 4;
        moveList[moveCount] += origin_sq << 10;
        // Check if capture, if so append 100 on the end
        if (gs->whiteToMove){
            if(getBit(&gs->boards[3], targetSquare)){
                moveList[moveCount] += 4;
            }
        }else{
            if(getBit(&gs->boards[2], targetSquare)){
                moveList[moveCount] += 4;
            }
        }
        
        moveCount++;
        
    }
    
    moveList = realloc(moveList, moveCount * sizeof(Move));
    *knightList = moveList; 
    
    return moveCount;
}


int calcKingMoves(Move **kingList, Square origin_sq, GameState *gs){
    /* Params:
        - **kingList: double pointer to list of king moves: should be initialized as null
        - origin_sq: Initial square of move
        - *gs: pointer to gamestate we'd like to calculate moves with
     
     Returns:
        - integer length of kingList
     */
    Move* moveList = (Move*)malloc(8* sizeof(Move));
    int moveCount = 0;
    BitBoard moveBoard = 0ULL;
    if (gs->whiteToMove){
        moveBoard = kingMoves[origin_sq] & ~(gs->boards[wPieces]);
    }else{
        moveBoard = kingMoves[origin_sq] & ~(gs->boards[bPieces]);
    }
    Square targetSquare;
    
    // Loop through the boards
    while(moveBoard){
        moveList[moveCount] = 0;
        targetSquare = get_ls1b_pos(&moveBoard);
        printBitBoard(&moveBoard, 0);
        clearBit(&moveBoard, targetSquare);
        moveList[moveCount] += targetSquare << 4;
        moveList[moveCount] += origin_sq << 10;
        // Check if capture, if so append 100 on the end
        if (gs->whiteToMove){
            if(getBit(&gs->boards[bPieces], targetSquare)){
                moveList[moveCount] += 4;
            }
        }else{
            if(getBit(&gs->boards[wPieces], targetSquare)){
                moveList[moveCount] += 4;
            }
        }
        
        moveCount++;
    }
    moveList = (Move*) realloc(moveList, moveCount * sizeof(Move));
    *kingList = moveList; 
    return moveCount;
}

int calcRookMoves(Move **slidingList, Square origin_sq, GameState *gs){
    /* Params:
        - **slidingList: double pointer to list of sliding moves: should be initialized as null
        - origin_sq: Initial square of move
        - *gs: pointer to gamestate we'd like to calculate moves with
     
     Returns:
        - integer length of kingList
     */
    
    // Allocate 14 Moves worth of memory because its the maximum move capacity
    Move* moveList = (Move*)malloc(14* sizeof(Move));
    int moveCount = 0;
    BitBoard moveBoard = 0ULL;
    
    // Start by calculating all the horizontalMoves
    
    
    // Calculate occupation number for the rank
    int rank = origin_sq / 8;
    int file = origin_sq % 8;
    
    
    int occNum = 255 & (gs->boards[aPieces] >> (8 * rank));
    
    // Remove capturs of own pieces and add attackMoves to a separate list:
    BitBoard horizontalAttacks;
    BitBoard horizontalBoard = rookMoves[origin_sq][occNum];
    //printBitBoard(&horizontalBoard, 0);
    if (gs->whiteToMove){
        horizontalAttacks = horizontalBoard & gs->boards[bPieces];
        horizontalBoard &= ~(gs->boards[aPieces]);
    }else{
        horizontalAttacks = horizontalBoard & gs->boards[wPieces];
        horizontalBoard &= ~(gs->boards[aPieces]);
    }
    
    //printBitBoard(&horizontalAttacks, 0);
    //printBitBoard(&horizontalBoard, 0);
    
    // Calculate Vertical Moves
    
    
    Square rot_sq = 8 * file + rank;
    int vert_occNum = 255 & (gs->rot_boards[aPieces] >> (8 * file));
    BitBoard verticalBoard = rookMoves[rot_sq][vert_occNum];
    BitBoard verticalAttacks;
    
    //printBitBoard(&verticalBoard, 0);
    
    if (gs->whiteToMove){
        verticalAttacks = verticalBoard & gs->rot_boards[bPieces];
        verticalBoard &= ~(gs->rot_boards[aPieces]);
    }else{
        verticalAttacks = verticalBoard & gs->rot_boards[wPieces];
        verticalBoard &= ~(gs->rot_boards[aPieces]);
    }

    // Now loop through and append moves to finalList: attacks first
    Square targetSquare;
    Square rot_targetSquare;
    while(verticalAttacks){
        // Append Vertical Attacks
        moveList[moveCount] = 0;
        rot_targetSquare = get_ls1b_pos(&verticalAttacks);
        targetSquare = 8 * (rot_targetSquare % 8) + (rot_targetSquare / 8);
        clearBit(&verticalAttacks, rot_targetSquare);
        moveList[moveCount] += targetSquare << 4;
        moveList[moveCount] += origin_sq << 10;
        // Append set 4digit code to 0100 because capture.
        moveList[moveCount] += 4;
        moveCount++;
    }
    while(horizontalAttacks){
        // Append Horizontal Attacks
        moveList[moveCount] = 0;
        targetSquare = get_ls1b_pos(&horizontalAttacks);
        clearBit(&horizontalAttacks, targetSquare);
        moveList[moveCount] += targetSquare << 4;
        moveList[moveCount] += origin_sq << 10;
        // Append set 4digit code to 0100 because capture.
        moveList[moveCount] += 4;
        moveCount++;
    }
    while(horizontalBoard) {
        // Append Horizontal QuietMoves
        moveList[moveCount] = 0;
        targetSquare = get_ls1b_pos(&horizontalBoard);
        clearBit(&horizontalBoard, targetSquare);
        moveList[moveCount] += targetSquare << 4;
        moveList[moveCount] += origin_sq << 10;
        moveCount++;
    }
    while(verticalBoard){
        // Append Vertical QuietMoves
        moveList[moveCount] = 0;
        rot_targetSquare = get_ls1b_pos(&verticalBoard);
        targetSquare = 8 * (rot_targetSquare % 8) + (rot_targetSquare / 8);
        clearBit(&verticalBoard, rot_targetSquare);
        moveList[moveCount] += targetSquare << 4;
        moveList[moveCount] += origin_sq << 10;
        moveCount++;
    }
    
    moveList = (Move*) realloc(moveList, moveCount * sizeof(Move));
    *slidingList = moveList;
    return moveCount;
}

int calcDiagMoves(Move **diagList, Square origin_sq, GameState *gs){
    /* Params:
        - **diagList: double pointer to list of sliding moves: should be initialized as null
        - origin_sq: Initial square of move
        - *gs: pointer to gamestate we'd like to calculate moves with
     
     Returns:
        - integer length of diagList
     */
    Move* moveList = (Move*)malloc(13* sizeof(Move)); // 13 maxmium number of bishop moves
    int moveCount = 0;
    BitBoard moveBoard = 0ULL;
    
    // Calculate Occupancy Number for Diagonal
    BitBoard occBoard = gs->diag_boards[aPieces];
    //printBitBoard(&occBoard, 0);
    
    int diagSize = (1 << (diagonalSizes[origin_sq]+1)) - 1;
    occBoard = (occBoard >> diagShift[origin_sq]) & diagSize;
    printBitBoard(&occBoard, 0);
    
    // Get moves for given square and occNum
    BitBoard testMoves = rookMoves[norm_to_diag[origin_sq]][occBoard];
    printBitBoard(&testMoves, 0);
    
    // Calculate Occupancy Number for Anti-diagonal
    

    return 0; 
}
