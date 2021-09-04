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
    // CALCULATE KNIGHT MOVES, DYNAMICALLY ALLOCATE SHIT
    // Create a list of 8 possible moves, will realloc later to chop off the unused part.
    Move *moveList;
    moveList = malloc(8 * sizeof(Move));
    int moveCount = 0;
    
    // Start with a bitboard of possible moves
    BitBoard moveBoard;
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
    /* Params
    
     */
    Move* moveList = (Move*)malloc(8* sizeof(Move));
    int moveCount = 0;
    BitBoard moveBoard;
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
