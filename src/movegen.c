//
//  movegen.c
//  BernieBot
//
//  Created by Charlie Sowerby on 9/15/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include "movegen.h"


/* ------------ MOVE GENERATION METHODS ------------- */

int moveGen(Move move_list[MOVE_LIST_LENGTH], GameState *gs){
    /*
    Params:
        Move move_list[]    - uninitialized list to be filled with max of 218 possible MOVES
        GameState *gs       - pointer to current GameState
    Returns:
        int                 - number of moves in the position
    */
    int moveCount = 0;
    //int ally_pieces = ALLY_PIECES;
    //int enemy_pieces = ENEMY_PIECES;
    // CASTLING
    /* Procedure for checking if castling is legal
        - If you have castling rights
        - If No pieces are in between
        - If not in check
        - If no squares in between are in check
     */

     //TODO: avoid branching here
    if(gs->sideToMove == wPieces){
        if(!inCheck(e1, gs)){
            // Check if not in Check
            if(GET_BIT(gs->castlingPrivileges, 3)){
                // Check if White Short Castling Privileges
                if (!(w_short_castle_squares & gs->boards[aPieces])){
                    // No Pieces in the way
                    if(!inCheck(f1, gs)){
                        if(!inCheck(g1, gs)){
                            // Squares between aren't in check

                            // Short Castling is available - append now
                            move_list[moveCount] = 0;
                            move_list[moveCount] += g1 << 4;
                            move_list[moveCount] += e1 << 10;
                            //Code for short Castle 0010
                            move_list[moveCount] += 2;
                            moveCount++;
                        }
                    }
                }
            }
            if(GET_BIT(gs->castlingPrivileges, 2)){
                // Check if White Long Castling Privileges
                if (!(w_long_castle_squares & gs->boards[aPieces])){
                    // No Pieces in the way
                    if(!inCheck(d1, gs)){
                        if(!inCheck(c1, gs)){
                            // Squares between aren't in check

                            // Long Castling is available - append now
                            move_list[moveCount] = 0;
                            move_list[moveCount] += c1 << 4;
                            move_list[moveCount] += e1 << 10;
                            //Code for long Castle 0011
                            move_list[moveCount] += 3;
                            moveCount++;
                        }
                    }
                }
            }
        }
    }else{
        // Black to Move
        if(!inCheck(e8, gs)){
            // Check if not in Check
            if(GET_BIT(gs->castlingPrivileges, 1)){
                // Check if Black Short Castling Privileges
                if (!(b_short_castle_squares & gs->boards[aPieces])){
                    // No Pieces in the way
                    if(!inCheck(f8, gs)){
                        if(!inCheck(g8, gs)){
                            // Squares between aren't in check

                            // Short Castling is available - append now
                            move_list[moveCount] = 0;
                            move_list[moveCount] += g8 << 4;
                            move_list[moveCount] += e8 << 10;
                            //Code for short Castle 0010
                            move_list[moveCount] += 2;
                            moveCount++;
                        }
                    }
                }
            }
            if(GET_BIT(gs->castlingPrivileges, 0)){
                // Check if Black Long Castling Privileges
                if (!(b_long_castle_squares & gs->boards[aPieces])){
                    // No Pieces in the way
                    if(!inCheck(d8, gs)){
                        if(!inCheck(c8, gs)){
                            // Squares between aren't in check

                            // Long Castling is available - append now
                            move_list[moveCount] = 0;
                            move_list[moveCount] += c8 << 4;
                            move_list[moveCount] += e8 << 10;
                            //Code for long Castle 0011
                            move_list[moveCount] += 3;
                            moveCount++;

                        }
                    }
                }
            }
        }
    }

    // Pawn Moves
    moveCount = calcPawnMoves(move_list, moveCount, gs);

    // Non Pawn Moves (only captures/quiet moves)

    //King Moves
    Square kingSquare = GET_LS1B(gs->boards[ALLY_KINGS]);
    BitBoard kMoves = kingMoves[kingSquare];
    BitBoard kAttacks = kMoves & gs->boards[ENEMY_PIECES];
    kMoves &= ~gs->boards[aPieces];
    moveCount = encodeMoves(move_list, moveCount, CAPTURE_MC, kAttacks, kingSquare);
    moveCount = encodeMoves(move_list, moveCount, QUIET_MOVE_MC, kMoves, kingSquare);

    //Sliding Moves
    BitBoard rookAndQueenPositions = gs->boards[ALLY_QUEENS] | gs->boards[ALLY_ROOKS];
    while(rookAndQueenPositions){
        // Get Origin Square
        Square originSquare = GET_LS1B(rookAndQueenPositions);
        CLEAR_LS1B(rookAndQueenPositions);

        // Get Target Board and spearate into
        BitBoard rMoves = getRookMoveBoard(originSquare, gs);
        // Remove captures of our own pieces and let attacks be a separate board

        BitBoard rAttacks = rMoves & gs->boards[ENEMY_PIECES];
        rMoves &= ~(gs->boards[aPieces]);

        moveCount = encodeMoves(move_list, moveCount, CAPTURE_MC, rAttacks, originSquare);
        moveCount = encodeMoves(move_list, moveCount, QUIET_MOVE_MC, rMoves, originSquare);
    }

    // Diagonal Moves
    BitBoard bishopAndQueenPositions = gs->boards[ALLY_QUEENS] | gs->boards[ALLY_BISHOPS];
    while(bishopAndQueenPositions){
        // Get Origin Square
        Square originSquare = GET_LS1B(bishopAndQueenPositions);
        CLEAR_LS1B(bishopAndQueenPositions);

        // Get Target Board and spearate into
        BitBoard bMoves = getBishopMoveBoard(originSquare, gs);
        // Remove captures of our own pieces and let attacks be a separate board
        BitBoard bAttacks = bMoves & gs->boards[ENEMY_PIECES];
        bMoves &= ~(gs->boards[aPieces]);

        moveCount = encodeMoves(move_list, moveCount, CAPTURE_MC, bAttacks, originSquare);
        moveCount = encodeMoves(move_list, moveCount, QUIET_MOVE_MC, bMoves, originSquare);
    }

    //Knight Moves
    BitBoard knightPositions = gs->boards[ALLY_KNIGHTS];
    while(knightPositions){
        // Get Origin Square
        Square originSquare = GET_LS1B(knightPositions);
        CLEAR_LS1B(knightPositions);

        // Get Target Board and spearate into attacks and quiet moves
        BitBoard nMoves = knightMoves[originSquare];
        // Remove captures of our own pieces and let attacks be a separate board
        BitBoard nAttacks = nMoves & gs->boards[ENEMY_PIECES];
        nMoves &= ~(gs->boards[aPieces]);

        moveCount = encodeMoves(move_list, moveCount, CAPTURE_MC, nAttacks, originSquare);
        moveCount = encodeMoves(move_list, moveCount, QUIET_MOVE_MC, nMoves, originSquare);
    }

    return moveCount;
}

static inline int encodeMoves(Move move_list[MOVE_LIST_LENGTH], int moveCount, int moveType, BitBoard moves, Square origin_sq){
    int numBits = NUMBITS(moves);
    for(int i = 0; i < numBits ;i++){
        // Append move to list
        Square targetSquare = GET_LS1B(moves);
        CLEAR_LS1B(moves);
        move_list[moveCount] = (targetSquare << 4) | (origin_sq << 10) | moveType;
        moveCount++;
    }
    return moveCount;
}



static inline int calcPawnMoves(Move move_list[MOVE_LIST_LENGTH], int moveCount, GameState *gs){
    /*
    Params:
        Move move_list[]    - list of all moves generated in this position
        int moveCount       - current index in move_list[]
        GameState *gs       - pointer to current GameState
    Returns:
        int moveCount       - Updated moveCount
    */

    BitBoard pawns = gs->boards[ALLY_PAWNS];
    BitBoard rightPawnAttackPromos = PAWN_BITSHIFT_RIGHT(pawns & notHFile) & gs->boards[ENEMY_PIECES] & PROMO_RANK;
    BitBoard leftPawnAttackPromos = PAWN_BITSHIFT_LEFT(pawns & notAFile) & gs->boards[ENEMY_PIECES] & PROMO_RANK;
    BitBoard leftPawnAttacks = PAWN_BITSHIFT_LEFT(pawns & notAFile) & gs->boards[ENEMY_PIECES] & ~(PROMO_RANK);
    BitBoard rightPawnAttacks = PAWN_BITSHIFT_RIGHT(pawns & notHFile) & gs->boards[ENEMY_PIECES] & ~(PROMO_RANK);
    BitBoard pawnMovePromo = PAWN_BITSHIFT_FORWARD(pawns & notHFile) & PROMO_RANK & ~(gs->boards[aPieces]);
    BitBoard pawnQuietMove = PAWN_BITSHIFT_FORWARD(pawns) & ~gs->boards[aPieces] & ~(PROMO_RANK);
    BitBoard pawnDoubleMoveTargets = PAWN_BITSHIFT_DOUBLEFORWARD(pawns & PAWN_STARTING_RANK) & ~gs->boards[aPieces] & PAWN_BITSHIFT_FORWARD(~gs->boards[aPieces]);

    //BitBoard test = PROMO_RANK;

    int numBits;
    // EN PASSANT!
    if (gs->enPassantTarget != no_sqr){
        BitBoard leftEP = PAWN_BITSHIFT_LEFT(pawns & notAFile);
        BitBoard rightEP = PAWN_BITSHIFT_RIGHT(pawns & notHFile);
        if (GET_BIT(leftEP, gs->enPassantTarget)){
            move_list[moveCount] = (gs->enPassantTarget << 4) | ((gs->enPassantTarget - PAWN_LEFT_MOVE) << 10);
            move_list[moveCount] |= EP_CAPTURE_MC;
            moveCount++;
        }
        if (GET_BIT(rightEP, gs->enPassantTarget)){
            move_list[moveCount] = (gs->enPassantTarget << 4) | ((gs->enPassantTarget - PAWN_RIGHT_MOVE) << 10);
            move_list[moveCount] |= EP_CAPTURE_MC;
            moveCount++;
        }
    }


    // Pop from move boards and append.

    // PAWN ATTACK PROMOTIONS
    numBits = NUMBITS(leftPawnAttackPromos);
    for(int i = 0; i < numBits; i++){
        Square targetSquare = GET_LS1B(leftPawnAttackPromos);
        CLEAR_LS1B(leftPawnAttackPromos);
        for(int i = 0; i < 4; i++){
            move_list[moveCount + i] = (targetSquare << 4) | ((targetSquare - PAWN_LEFT_MOVE) << 10);
        }
        // Add move codes for promo capture
        move_list[moveCount] |= QUEEN_PROMO_CAP_MC;
        move_list[moveCount + 1] |= KNIGHT_PROMO_CAP_MC;
        move_list[moveCount + 2] |= ROOK_PROMO_CAP_MC;
        move_list[moveCount + 3] |= BISHOP_PROMO_CAP_MC;
        moveCount += 4;
    }

    numBits = NUMBITS(rightPawnAttackPromos);
    for(int i = 0; i < numBits; i++){
        Square targetSquare = GET_LS1B(rightPawnAttackPromos);
        CLEAR_LS1B(rightPawnAttackPromos);
        for(int i = 0; i < 4; i++){
            move_list[moveCount + i] = (targetSquare << 4) | ((targetSquare - PAWN_RIGHT_MOVE) << 10);
        }
        // Add move codes for promo capture
        move_list[moveCount] |= QUEEN_PROMO_CAP_MC;
        move_list[moveCount + 1] |= KNIGHT_PROMO_CAP_MC;
        move_list[moveCount + 2] |= ROOK_PROMO_CAP_MC;
        move_list[moveCount + 3] |= BISHOP_PROMO_CAP_MC;
        moveCount += 4;
    }

    // QUIET MOVE PROMOTIONS
    numBits = NUMBITS(pawnMovePromo);
    for(int i = 0; i < numBits; i++){
        Square targetSquare = GET_LS1B(pawnMovePromo);
        CLEAR_LS1B(pawnMovePromo);
        for(int i = 0; i < 4; i++){
            move_list[moveCount + i] = (targetSquare << 4) | ((targetSquare - PAWN_FORWARD_MOVE) << 10);
        }
        // add move code for promo
        move_list[moveCount] |= QUEEN_PROMO_MC;
        move_list[moveCount + 1] |= KNIGHT_PROMO_MC;
        move_list[moveCount + 2] |= ROOK_PROMO_MC;
        move_list[moveCount + 3] |= BISHOP_PROMO_MC;
        moveCount += 4;
    }



    // REGULAR PAWN ATTACKS
    numBits = NUMBITS(leftPawnAttacks);
    for(int i = 0; i < numBits; i++){
        Square targetSquare = GET_LS1B(leftPawnAttacks);
        CLEAR_LS1B(leftPawnAttacks);
        move_list[moveCount] = (targetSquare << 4) | ((targetSquare - PAWN_LEFT_MOVE) << 10);
        // Add Code for Regular Pawn Capture - 0100
        move_list[moveCount] |= CAPTURE_MC;
        moveCount++;
    }
    numBits = NUMBITS(rightPawnAttacks);
    for(int i = 0; i < numBits; i++){
        Square targetSquare = GET_LS1B(rightPawnAttacks);
        CLEAR_LS1B(rightPawnAttacks);
        move_list[moveCount] = (targetSquare << 4) | ((targetSquare - PAWN_RIGHT_MOVE) << 10);
        // Add Code for Regular Pawn Capture - 0100
        move_list[moveCount] |= CAPTURE_MC;
        moveCount++;
    }

    // DOUBLE PAWN MOVES
    numBits = NUMBITS(pawnDoubleMoveTargets);
    for(int i = 0; i < numBits; i++){
        Square targetSquare = GET_LS1B(pawnDoubleMoveTargets);
        CLEAR_LS1B(pawnDoubleMoveTargets);
        move_list[moveCount] += targetSquare << 4;
        move_list[moveCount] += ((targetSquare - 2 * PAWN_FORWARD_MOVE) << 10);
        // Add Code for Double Pawn Push - 0001
        move_list[moveCount] |= DOUBLE_PAWN_PUSH_MC;
        moveCount++;
    }

    // SINGLE QUIET PAWN MOVES
    numBits = NUMBITS(pawnQuietMove);
    for(int i = 0; i < numBits; i++){
        Square targetSquare = GET_LS1B(pawnQuietMove);
        CLEAR_LS1B(pawnQuietMove);
        move_list[moveCount] = (targetSquare << 4) | ((targetSquare - PAWN_FORWARD_MOVE) << 10);
        move_list[moveCount] |= QUIET_PAWN_PUSH_MC;
        moveCount++;
    }
    return moveCount;
}
