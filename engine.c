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
        returns: integer with length of moveList, -1 if failure, 0 if no moves
     
     
     NOTE- ONE TRY COULD BE TO RETURN -1 IF CAPTURE OF KING IS A LEGAL MOVE, i.e. the last move was illegal and we need to unmake the last move
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
        printBitBoard(&moveBoard);
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
        - integer length of moveList
     */
    
    // Allocate 14 Moves worth of memory because its the maximum move capacity
    Move* moveList = (Move*)malloc(14* sizeof(Move));
    int moveCount = 0;
    
    BitBoard occ = gs->boards[aPieces];
    occ &= rookMagics[origin_sq].mask;
    occ *= rookMagics[origin_sq].magic;
    occ >>= rookMagics[origin_sq].shift;
    BitBoard moves = rookMagics[origin_sq].attackPtr[occ];
    
    BitBoard attacks;
    // Remove captures of our own pieces and let attacks be a separate board
    if (gs->whiteToMove){
        attacks = moves & gs->boards[bPieces];
        moves &= ~(gs->boards[aPieces]);
    }else{
        attacks = moves & gs->boards[wPieces];
        moves &= ~(gs->boards[aPieces]);
    }
    Square targetSquare;
    while(attacks){
        // Append Attacks
        moveList[moveCount] = 0;
        targetSquare = get_ls1b_pos(&attacks);
        clearBit(&attacks, targetSquare);
        moveList[moveCount] += targetSquare << 4;
        moveList[moveCount] += origin_sq << 10;
        // Append set 4digit code to 0100 because capture.
        moveList[moveCount] += 4;
        moveCount++;
    }
    while(moves){
        //append quietmoves
        moveList[moveCount] = 0;
        targetSquare = get_ls1b_pos(&moves);
        clearBit(&moves, targetSquare);
        moveList[moveCount] += targetSquare << 4;
        moveList[moveCount] += origin_sq << 10;
        moveCount++;
    }
    
    
    moveList = (Move*) realloc(moveList, moveCount * sizeof(Move));
    *slidingList = moveList;
    return moveCount;
}

int calcBishopMoves(Move **diagList, Square origin_sq, GameState *gs){
    // Allocate 13 Moves worth of memory because its the maximum move capacity
    Move* moveList = (Move*)malloc(13* sizeof(Move));
    int moveCount = 0;
        
    BitBoard occ = gs->boards[aPieces];
    occ &= bishopMagics[origin_sq].mask;
    occ *= bishopMagics[origin_sq].magic;
    occ >>= bishopMagics[origin_sq].shift;
    BitBoard moves = bishopMagics[origin_sq].attackPtr[occ];
    printBitBoard(&moves);
        
    BitBoard attacks;
        // Remove captures of our own pieces and let attacks be a separate board
    if (gs->whiteToMove){
        attacks = moves & gs->boards[bPieces];
        moves &= ~(gs->boards[aPieces]);
    }else{
        attacks = moves & gs->boards[wPieces];
        moves &= ~(gs->boards[aPieces]);
    }
    Square targetSquare;
    while(attacks){
        // Append Attacks
        moveList[moveCount] = 0;
        targetSquare = get_ls1b_pos(&attacks);
        clearBit(&attacks, targetSquare);
        moveList[moveCount] += targetSquare << 4;
        moveList[moveCount] += origin_sq << 10;
        // Append set 4digit code to 0100 because capture.
        moveList[moveCount] += 4;
        moveCount++;
    }
    while(moves){
        //append quietmoves
        moveList[moveCount] = 0;
        targetSquare = get_ls1b_pos(&moves);
        clearBit(&moves, targetSquare);
        moveList[moveCount] += targetSquare << 4;
        moveList[moveCount] += origin_sq << 10;
        moveCount++;
    }
        
        
    moveList = (Move*) realloc(moveList, moveCount * sizeof(Move));
    *diagList = moveList;
    return moveCount;
}

int calcPawnMoves(Move **pawnList, GameState *gs){
    // Allocate 8 * 4 = 32 Moves worth of memory because its the maximum move capacity of all pawns
    Move* moveList = (Move*)malloc(32* sizeof(Move));
    int moveCount = 0;

    if(gs->whiteToMove){
        BitBoard pawns = gs->boards[wPawns];
        printBitBoard(&pawns); 
        //ATTACKS
        BitBoard leftPawnAttackPromos = ((pawns & notAFile)>>7) & gs->boards[bPieces] & rank8;
        printBitBoard(&leftPawnAttackPromos);
        BitBoard rightPawnAttackPromos = ((pawns & notHFile)>>9) & gs->boards[bPieces] & rank8;
        printBitBoard(&rightPawnAttackPromos);
        BitBoard leftPawnAttacks = ((pawns & notAFile)>>7) & gs->boards[bPieces] & notRank8;
        printBitBoard(&leftPawnAttacks);
        BitBoard rightPawnAttacks = ((pawns & notHFile)>>9) & gs->boards[bPieces] & notRank8;
        printBitBoard(&rightPawnAttacks);
        
        //QUIET MOVES
        BitBoard pawnMoveTargets = (pawns << 8) & ~gs->boards[aPieces] & notRank8;
        printBitBoard(&pawnMoveTargets);
        BitBoard pawnMovePromo = (pawns << 8) &~gs->boards[aPieces] & rank8;
        printBitBoard(&pawnMovePromo);
        BitBoard pawnDoubleMoveTargets = ((pawns & rank2) << 16) & ~gs->boards[aPieces] & ~(gs->boards[aPieces] << 8);
        printBitBoard(&pawnDoubleMoveTargets);
        
        
        // PAWN ATTACK PROMOTIONS
        while(leftPawnAttackPromos){
            for(int i = 0; i < 4; i++){
                moveList[moveCount+i] = 0;
            }
            Square targetSquare = get_ls1b_pos(&leftPawnAttackPromos);
            clearBit(&leftPawnAttackPromos, targetSquare);
            for(int i = 0; i < 4; i++){
                moveList[moveCount + i] += targetSquare << 4;
                moveList[moveCount + i] += (targetSquare - 7) << 10;
            }
            /* Add Code for Promo Pawn Push Capture:
                1100 - 12  - knight
                1101 - 13  - bishop
                1110 - 14 - rook
                1111 - 15 - queen
             */
            moveList[moveCount] += 15;
            moveList[moveCount + 1] += 12;
            moveList[moveCount + 2] += 14;
            moveList[moveCount + 3] += 13;
            moveCount += 4;
        }
        while(rightPawnAttackPromos){
            for(int i = 0; i < 4; i++){
                moveList[moveCount+i] = 0;
            }
            Square targetSquare = get_ls1b_pos(&rightPawnAttackPromos);
            clearBit(&rightPawnAttackPromos, targetSquare);
            for(int i = 0; i < 4; i++){
                moveList[moveCount + i] += targetSquare << 4;
                moveList[moveCount + i] += (targetSquare - 9) << 10;
            }
            /* Add Code for Promo Pawn Push Capture:
                1100 - 12  - knight
                1101 - 13  - bishop
                1110 - 14 - rook
                1111 - 15 - queen
             */
            moveList[moveCount] += 15;
            moveList[moveCount + 1] += 12;
            moveList[moveCount + 2] += 14;
            moveList[moveCount + 3] += 13;
            moveCount += 4;
        }
        
        // QUIET MOVE PROMOTIONS
        while(pawnMovePromo){
            for(int i = 0; i < 4; i++){
                moveList[moveCount+i] = 0;
            }
            Square targetSquare = get_ls1b_pos(&pawnMovePromo);
            clearBit(&pawnMovePromo, targetSquare);
            for(int i = 0; i < 4; i++){
                moveList[moveCount + i] += targetSquare << 4;
                moveList[moveCount + i] += (targetSquare - 8) << 10;
            }
            /* Add Code for Promo Pawn Push:
                1000 - 8  - knight
                1001 - 9  - bishop
                1010 - 10 - rook
                1011 - 11 - queen
             */
            moveList[moveCount] += 11;
            moveList[moveCount + 1] += 8;
            moveList[moveCount + 2] += 10;
            moveList[moveCount + 3] += 9;
            moveCount += 4;
        }
        
        // REGULAR PAWN ATTACKS
        while(leftPawnAttacks){
            moveList[moveCount] = 0;
            Square targetSquare = get_ls1b_pos(&leftPawnAttacks);
            clearBit(&leftPawnAttacks, targetSquare);
            moveList[moveCount] += targetSquare << 4;
            moveList[moveCount] += (targetSquare - 7) << 10;
            // Add Code for Regular Pawn Capture - 0100
            moveList[moveCount] += 4;
            moveCount++;
        }
        while(rightPawnAttacks){
            moveList[moveCount] = 0;
            Square targetSquare = get_ls1b_pos(&rightPawnAttacks);
            clearBit(&rightPawnAttacks, targetSquare);
            moveList[moveCount] += targetSquare << 4;
            moveList[moveCount] += (targetSquare - 9) << 10;
            // Add Code for Regular Pawn Capture - 0100
            moveList[moveCount] += 4;
            moveCount++;
        }

        // DOUBLE PAWN MOVES
        while(pawnDoubleMoveTargets){
            moveList[moveCount] = 0;
            Square targetSquare = get_ls1b_pos(&pawnDoubleMoveTargets);
            clearBit(&pawnDoubleMoveTargets, targetSquare);
            moveList[moveCount] += targetSquare << 4;
            moveList[moveCount] += (targetSquare - 16) << 10;
            // Add Code for Double Pawn Push - 0001
            moveList[moveCount]++;
            moveCount++;
        }
        
        // SINGLE QUIET PAWN MOVES
        while(pawnMoveTargets){
            moveList[moveCount] = 0;
            Square targetSquare = get_ls1b_pos(&pawnMoveTargets);
            clearBit(&pawnMoveTargets, targetSquare);
            moveList[moveCount] += targetSquare << 4;
            moveList[moveCount] += (targetSquare - 8) << 10;
            moveCount++;
        }
        // EN PASSANT!
        
        if (gs->enPassantTarget != no_sqr){
            BitBoard leftEP = ((pawns & notAFile)<<7);
            BitBoard rightEP = ((pawns & notHFile)<<9);
            if (getBit(&leftEP, gs->enPassantTarget)){
                moveList[moveCount] += gs->enPassantTarget << 4;
                moveList[moveCount] += (gs->enPassantTarget - 7) << 10;
                moveCount++;
            }
            if (getBit(&rightEP, gs->enPassantTarget)){
                moveList[moveCount] += gs->enPassantTarget << 4;
                moveList[moveCount] += (gs->enPassantTarget - 9) << 10;
                moveCount++;
            }
        }
    }else{
        BitBoard pawns = gs->boards[bPawns];
        
        //ATTACKS
        BitBoard leftPawnAttackPromos = ((pawns & notAFile)>>9) & gs->boards[wPieces] & rank1;
        BitBoard rightPawnAttackPromos = ((pawns & notHFile)>>7) & gs->boards[wPieces] & rank1;
        
        BitBoard leftPawnAttacks = ((pawns & notAFile)>>9) & gs->boards[wPieces] & notRank1;
        BitBoard rightPawnAttacks = ((pawns & notHFile)>>7) & gs->boards[wPieces] & notRank1;
        
        //QUIET MOVES
        BitBoard pawnMoveTargets = (pawns >> 8) & ~gs->boards[aPieces] & notRank1;
        BitBoard pawnMovePromo = (pawns >> 8) &~gs->boards[aPieces] & rank1;
        BitBoard pawnDoubleMoveTargets = ((pawns & rank7) >> 16) & ~gs->boards[aPieces] & ~(gs->boards[aPieces] >> 8);
        
        
        // PAWN ATTACK PROMOTIONS
        while(leftPawnAttackPromos){
            for(int i = 0; i < 4; i++){
                moveList[moveCount+i] = 0;
            }
            Square targetSquare = get_ls1b_pos(&leftPawnAttackPromos);
            clearBit(&leftPawnAttackPromos, targetSquare);
            for(int i = 0; i < 4; i++){
                moveList[moveCount + i] += targetSquare << 4;
                moveList[moveCount + i] += (targetSquare + 9) << 10;
            }
            /* Add Code for Promo Pawn Push Capture:
                1100 - 12  - knight
                1101 - 13  - bishop
                1110 - 14 - rook
                1111 - 15 - queen
             */
            moveList[moveCount] += 15;
            moveList[moveCount + 1] += 12;
            moveList[moveCount + 2] += 14;
            moveList[moveCount + 3] += 13;
            moveCount += 4;
        }
        while(rightPawnAttackPromos){
            for(int i = 0; i < 4; i++){
                moveList[moveCount+i] = 0;
            }
            Square targetSquare = get_ls1b_pos(&rightPawnAttackPromos);
            clearBit(&rightPawnAttackPromos, targetSquare);
            for(int i = 0; i < 4; i++){
                moveList[moveCount + i] += targetSquare << 4;
                moveList[moveCount + i] += (targetSquare + 7) << 10;
            }
            /* Add Code for Promo Pawn Push Capture:
                1100 - 12  - knight
                1101 - 13  - bishop
                1110 - 14 - rook
                1111 - 15 - queen
             */
            moveList[moveCount] += 15;
            moveList[moveCount + 1] += 12;
            moveList[moveCount + 2] += 14;
            moveList[moveCount + 3] += 13;
            moveCount += 4;
        }
        
        // QUIET MOVE PROMOTIONS
        while(pawnMovePromo){
            for(int i = 0; i < 4; i++){
                moveList[moveCount+i] = 0;
            }
            Square targetSquare = get_ls1b_pos(&pawnMovePromo);
            clearBit(&pawnMovePromo, targetSquare);
            for(int i = 0; i < 4; i++){
                moveList[moveCount + i] += targetSquare << 4;
                moveList[moveCount + i] += (targetSquare + 8) << 10;
            }
            /* Add Code for Promo Pawn Push:
                1000 - 8  - knight
                1001 - 9  - bishop
                1010 - 10 - rook
                1011 - 11 - queen
             */
            moveList[moveCount] += 11;
            moveList[moveCount + 1] += 8;
            moveList[moveCount + 2] += 10;
            moveList[moveCount + 3] += 9;
            moveCount += 4;
        }
        
        // REGULAR PAWN ATTACKS
        while(leftPawnAttacks){
            moveList[moveCount] = 0;
            Square targetSquare = get_ls1b_pos(&leftPawnAttacks);
            clearBit(&leftPawnAttacks, targetSquare);
            moveList[moveCount] += targetSquare << 4;
            moveList[moveCount] += (targetSquare + 9) << 10;
            // Add Code for Regular Pawn Capture - 0100
            moveList[moveCount] += 4;
            moveCount++;
        }
        while(rightPawnAttacks){
            moveList[moveCount] = 0;
            Square targetSquare = get_ls1b_pos(&rightPawnAttacks);
            clearBit(&rightPawnAttacks, targetSquare);
            moveList[moveCount] += targetSquare << 4;
            moveList[moveCount] += (targetSquare + 7) << 10;
            // Add Code for Regular Pawn Capture - 0100
            moveList[moveCount] += 4;
            moveCount++;
        }

        // DOUBLE PAWN MOVES
        while(pawnDoubleMoveTargets){
            moveList[moveCount] = 0;
            Square targetSquare = get_ls1b_pos(&pawnDoubleMoveTargets);
            clearBit(&pawnDoubleMoveTargets, targetSquare);
            moveList[moveCount] += targetSquare << 4;
            moveList[moveCount] += (targetSquare + 16) << 10;
            // Add Code for Double Pawn Push - 0001
            moveList[moveCount]++;
            moveCount++;
        }
        
        // SINGLE QUIET PAWN MOVES
        while(pawnMoveTargets){
            moveList[moveCount] = 0;
            Square targetSquare = get_ls1b_pos(&pawnMoveTargets);
            clearBit(&pawnMoveTargets, targetSquare);
            moveList[moveCount] += targetSquare << 4;
            moveList[moveCount] += (targetSquare + 8) << 10;
            moveCount++;
        }
        // EN PASSANT!
        
        if (gs->enPassantTarget != no_sqr){
            BitBoard leftEP = ((pawns & notAFile)>>9);
            BitBoard rightEP = ((pawns & notHFile)<<7);
            if (getBit(&leftEP, gs->enPassantTarget)){
                moveList[moveCount] += gs->enPassantTarget << 4;
                moveList[moveCount] += (gs->enPassantTarget + 9) << 10;
                moveCount++;
            }
            if (getBit(&rightEP, gs->enPassantTarget)){
                moveList[moveCount] += gs->enPassantTarget << 4;
                moveList[moveCount] += (gs->enPassantTarget + 7) << 10;
                moveCount++;
            }
        }
    }
    
   
        
    moveList = (Move*) realloc(moveList, moveCount * sizeof(Move));
    *pawnList = moveList;
    return moveCount;
}
