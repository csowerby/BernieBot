//
//  engine.c
//  BernieBot
//
//  Created by Charlie Sowerby on 8/22/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include "engine.h"

/* --------------------------------------------------------*/
/* ------------------ SEARCHING ---------------------------*/
/* --------------------------------------------------------*/

Move search(int depth, GameState *gs){

    double maxScore = INT_MIN;
    Move bestMove = 0;

    Move move_list[MOVE_LIST_LENGTH];

    int n_moves, i;
    n_moves = moveGen(move_list, gs);
    for(i = 0; i < n_moves; i++){
        //printMoveInfo(move_list[i]);
        int success = makeMove(gs, move_list[i]);
        //printGameBoard(gs);
        if (success != -1){
            // Move is legal -> call negamax
            double score = -negaMax(depth - 1, gs);
            if (score > maxScore){
                maxScore = score;
                bestMove = move_list[i];
            }
        }
        unmakeMove(gs, move_list[i]);
    }
    printf("Best Move:\n");
    printMoveInfo(bestMove);
    printf("Evaluation: %f\n", maxScore);
    return bestMove;
}


int negaMax(int depth, GameState *gs){
    if(depth == 0) return EVALUATE(gs);
    double maxScore = INT_MIN;

    //Generate MoveList
    Move move_list[MOVE_LIST_LENGTH];
    int n_moves, i;
    n_moves = moveGen(move_list, gs);

    // Loop Through Moves
    for(i = 0; i < n_moves; i++){
        //printMoveInfo(move_list[i]);
        int success = makeMove(gs, move_list[i]);
        if(success != -1){
            // Legal Move -> Check for score
            double score = -negaMax(depth - 1, gs);
            if (score > maxScore){
                maxScore = score;
            }
        }

        unmakeMove(gs, move_list[i]);
    }
    return maxScore;
}

/* ------------------------------------------------------------*/
/* -------------------------- EVALUATION --------------------- */
/* ------------------------------------------------------------*/
float naiveEvaluation(GameState *gs){
    /* Static evaluation that returns higher score for better move for who is to move */
    float evaluation;
    float materialScore = 0;

    //Material
    materialScore += 9*(NUMBITS(gs->boards[wQueens]) - NUMBITS(gs->boards[bQueens]));
    materialScore += 5*(NUMBITS(gs->boards[wRooks]) - NUMBITS(gs->boards[bRooks]));
    materialScore += 3*(NUMBITS(gs->boards[wBishops]) - NUMBITS(gs->boards[bBishops]));
    materialScore += 3*(NUMBITS(gs->boards[wKnights]) - NUMBITS(gs->boards[bKnights]));
    materialScore += 1*(NUMBITS(gs->boards[wPawns]) - NUMBITS(gs->boards[bPawns]));

    //Structure


    // Return Final Evaluation
    int sideToMove = -1 + 2 * gs->sideToMove;// 0 for black, 1 for white
    evaluation = materialScore * sideToMove;
    return evaluation;
}


/* ------------------------------------------------------------------*/
/* --------------------------   MOVE MAKING ------------------------ */
/* ------------------------------------------------------------------*/

int makeMove(GameState *gs, Move move){
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

    Piece originPiece, targetPiece, promoPiece;
    Square targetSquare = 0b111111 & move >> 4;
    Square originSquare = move >>10;

    uint8_t moveCode = move & 0b1111;

    // Add info to Hist
    gs->gameHist[gs->histIndex][castling_rights] = gs->castlingPrivileges;
    gs->gameHist[gs->histIndex][ep_target] = gs->enPassantTarget;
    gs->gameHist[gs->histIndex][fifty_move_ply] = gs->fiftyMovePly;

    switch (moveCode) {
            // Promo Capture - knight, bishop, rook, queen
        case 12:
        case 13:
        case 14:
        case 15:
            originPiece = ALLY_PAWNS;
            targetPiece = gs->squareOccupancy[targetSquare];
            gs->gameHist[gs->histIndex][captured_piece] = targetPiece;
            // Remove Origin Piece
            SWITCH_BIT(gs->boards[originPiece], originSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
            SWITCH_BIT(gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;

            // Remove Target Piece
            SWITCH_BIT(gs->boards[targetPiece], targetSquare);
            SWITCH_BIT(gs->boards[ENEMY_PIECES], targetSquare);

            //Add Promoted Piece
            promoPiece = PROMO_PIECE(moveCode);
            SWITCH_BIT(gs->boards[promoPiece], targetSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
            gs->squareOccupancy[targetSquare] = promoPiece;

            gs->fiftyMovePly = 0;
            gs->enPassantTarget = no_sqr;

            // Castling Privileges (if a rook is captured, need to update)
            switch (targetSquare) {
                case a1:
                    CLEAR_BIT(gs->castlingPrivileges, 2); // white long castle
                    break;
                case a8:
                    CLEAR_BIT(gs->castlingPrivileges, 0); // black long castle
                    break;
                case h1:
                    CLEAR_BIT(gs->castlingPrivileges, 3); // white short castle
                    break;
                case h8:
                    CLEAR_BIT(gs->castlingPrivileges, 1); // black short castle
                    break;
                default:
                    break;
            }
            break;

            // Regular Promos - knight, bishop, rook, queen
        case 8:
        case 9:
        case 10:
        case 11:;
            originPiece = ALLY_PAWNS;
            promoPiece = PROMO_PIECE(moveCode);
            // Remove Origin Piece
            SWITCH_BIT(gs->boards[originPiece], originSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
            SWITCH_BIT(gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;

            //Add Promoted Piece
            SWITCH_BIT(gs->boards[promoPiece], targetSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
            SWITCH_BIT(gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = promoPiece;

            gs->fiftyMovePly = 0;
            gs->enPassantTarget = no_sqr;
            break;

        // Quiet Moves
        case 1: // Double Pawn Push
            originPiece = ALLY_PAWNS;
            gs->enPassantTarget = (originSquare + targetSquare)/2;

            // Remove Origin Piece
            SWITCH_BIT(gs->boards[originPiece], originSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
            SWITCH_BIT(gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;

            // Add Moved Piece
            SWITCH_BIT(gs->boards[originPiece], targetSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
            SWITCH_BIT(gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;

            gs->fiftyMovePly = 0;
            break;
        case 6: // PAWN QUIET MOVE
            originPiece = ALLY_PAWNS;

            // Remove Origin Piece
            SWITCH_BIT(gs->boards[originPiece], originSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
            SWITCH_BIT(gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;

            // Add Moved Piece
            SWITCH_BIT(gs->boards[originPiece], targetSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
            SWITCH_BIT(gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;

            gs->enPassantTarget = no_sqr;
            gs->fiftyMovePly = 0;
            break;
        case 0: // QuietMove
            originPiece = gs->squareOccupancy[originSquare];
            // Remove Origin Piece
            SWITCH_BIT(gs->boards[originPiece], originSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
            SWITCH_BIT(gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;

            // Add Moved Piece
            SWITCH_BIT(gs->boards[originPiece], targetSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
            SWITCH_BIT(gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;

            gs->fiftyMovePly++;
            gs->enPassantTarget = no_sqr;
            break;
        case 2: // Short Castle
            // Remove Rooks
            //TODO: test out whether or not precomputing these macros is faster
            SWITCH_BIT(gs->boards[ALLY_ROOKS], originSquare + 3);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare + 3);
            SWITCH_BIT(gs->boards[aPieces], originSquare + 3);
            gs->squareOccupancy[originSquare + 3] = no_pce;

            // Add Rook
            SWITCH_BIT(gs->boards[ALLY_ROOKS], originSquare + 1);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare + 1);
            SWITCH_BIT(gs->boards[aPieces], originSquare + 1);
            gs->squareOccupancy[originSquare + 1] = ALLY_ROOKS;

            // Remove Origin King
            SWITCH_BIT(gs->boards[ALLY_KINGS], originSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
            SWITCH_BIT(gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;

            // Add Moved King
            SWITCH_BIT(gs->boards[ALLY_KINGS], targetSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
            SWITCH_BIT(gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = ALLY_KINGS;

            gs->fiftyMovePly++;
            gs->enPassantTarget = no_sqr;
            break;
        case 3: // Long Castle
            // Remove Rooks
            SWITCH_BIT(gs->boards[ALLY_ROOKS], originSquare - 4 );
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare - 4);
            SWITCH_BIT(gs->boards[aPieces], originSquare - 4);
            gs->squareOccupancy[originSquare - 4] = no_pce;

            // Add Rook
            SWITCH_BIT(gs->boards[ALLY_ROOKS], originSquare - 1);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare - 1);
            SWITCH_BIT(gs->boards[aPieces], originSquare - 1);
            gs->squareOccupancy[originSquare - 1] = ALLY_ROOKS;

            // Remove Origin King
            SWITCH_BIT(gs->boards[ALLY_KINGS], originSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
            SWITCH_BIT(gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;

            // Add Moved King
            SWITCH_BIT(gs->boards[ALLY_KINGS], targetSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
            SWITCH_BIT(gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = ALLY_KINGS;

            gs->fiftyMovePly++;
            gs->enPassantTarget = no_sqr;
            break;

        case 5:; // EP Capture
            Square enPassantSquare = targetSquare - PAWN_FORWARD_MOVE;
            originPiece = ALLY_PAWNS;
            targetPiece = ENEMY_PAWNS;

            //Remove Origin Pawn
            SWITCH_BIT(gs->boards[originPiece], originSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
            SWITCH_BIT(gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;

            //Add Moved Pawn
            SWITCH_BIT(gs->boards[originPiece], targetSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
            SWITCH_BIT(gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;

            //Remove Captured Pawn
            SWITCH_BIT(gs->boards[targetPiece], enPassantSquare);
            SWITCH_BIT(gs->boards[ENEMY_PIECES], enPassantSquare);
            SWITCH_BIT(gs->boards[aPieces], enPassantSquare);
            gs->squareOccupancy[enPassantSquare] = no_pce;

            gs->enPassantTarget = no_sqr;
            gs->fiftyMovePly = 0;
            break;
        case 4: // Regular Capture
            originPiece = gs->squareOccupancy[originSquare];
            targetPiece = gs->squareOccupancy[targetSquare];
            gs->gameHist[gs->histIndex][captured_piece] = targetPiece;

            //Remove origin piece
            SWITCH_BIT(gs->boards[originPiece], originSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
            SWITCH_BIT(gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;

            // Remove captured piece
            SWITCH_BIT(gs->boards[targetPiece], targetSquare);
            SWITCH_BIT(gs->boards[ENEMY_PIECES], targetSquare);
            SWITCH_BIT(gs->boards[aPieces], targetSquare);

            // Add Moved Piece
            SWITCH_BIT(gs->boards[originPiece], targetSquare);
            SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
            SWITCH_BIT(gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;
            gs->enPassantTarget = no_sqr;
            gs->fiftyMovePly = 0;

            switch (targetSquare) {
                case a1:
                    CLEAR_BIT(gs->castlingPrivileges, 2); // white long castle
                    break;
                case a8:
                    CLEAR_BIT(gs->castlingPrivileges, 0); // black long castle
                    break;
                case h1:
                    CLEAR_BIT(gs->castlingPrivileges, 3); // white short castle
                    break;
                case h8:
                    CLEAR_BIT(gs->castlingPrivileges, 1); // black short castle
                    break;
                default:
                    break;
            }
    }

    // Update Game Ply
    gs->plyNum++;

    // Update Castling Priviledges
    switch (originSquare) {
        case e1:
            CLEAR_BIT(gs->castlingPrivileges, 2); // white long castle
            CLEAR_BIT(gs->castlingPrivileges, 3); // white short castle
            break;
        case a1:
            CLEAR_BIT(gs->castlingPrivileges, 2); // white long castle
            break;
        case h1:
            CLEAR_BIT(gs->castlingPrivileges, 3); // white short castle
            break;
        case a8:
            CLEAR_BIT(gs->castlingPrivileges, 0); // black long castle
            break;
        case h8:
            CLEAR_BIT(gs->castlingPrivileges, 1); // black short castle
            break;
        case e8:
            CLEAR_BIT(gs->castlingPrivileges, 0); // b long castle
            CLEAR_BIT(gs->castlingPrivileges, 1); // b short castle
            break;

        default:
            break;
    }

    // assert(gs->boards[wKings]);
    // assert(gs->boards[bKings]);

    gs->histIndex++;
    // Check if position Legal
    Square kingPos = GET_LS1B(gs->boards[ALLY_KINGS]);
    int returnValue = inCheck(kingPos, gs);

    // Switch who is on move
    gs->sideToMove = !gs->sideToMove;

    return returnValue;

}

int unmakeMove(GameState *gs, Move move){
    //Revert who is on move
    gs->sideToMove = !gs->sideToMove;

    // Restore info from history stack
    gs->histIndex--;
    gs->enPassantTarget = gs->gameHist[gs->histIndex][ep_target];
    gs->castlingPrivileges = gs->gameHist[gs->histIndex][castling_rights];

    Square targetSquare = 0b111111 & (move >> 4);
    Square originSquare = move>> 10;
    Piece originPiece, targetPiece;

    int moveCode = move & 15;

       switch (moveCode) {
               // Promo Capture - knight, bishop, rook, queen
           case 12:
           case 13:
           case 14:
           case 15:
               originPiece = ALLY_PAWNS;
               targetPiece = gs->gameHist[gs->histIndex][captured_piece];
               Piece promoPiece = gs->squareOccupancy[targetSquare];

               // Replace Origin Piece
               SWITCH_BIT(gs->boards[originPiece], originSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
               SWITCH_BIT(gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;

               //Remove Promoted Piece
               SWITCH_BIT(gs->boards[promoPiece], targetSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);

               // Replace Target Piece
               SWITCH_BIT(gs->boards[targetPiece], targetSquare);
               SWITCH_BIT(gs->boards[ENEMY_PIECES], targetSquare);
               gs->squareOccupancy[targetSquare] = targetPiece;

               gs->fiftyMovePly = gs->gameHist[gs->histIndex][fifty_move_ply];
               break;

               // Regular Promos - knight, bishop, rook, queen
           case 8:
           case 9:
           case 10:
           case 11:
               originPiece = ALLY_PAWNS;
               promoPiece = gs->squareOccupancy[targetSquare];

               // Replace Origin Piece
               SWITCH_BIT(gs->boards[originPiece], originSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
               SWITCH_BIT(gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;

               //Remove Promoted Piece
               //TODO: Change all the macros so that for the ones that are done multiple times per, i.e. ALLY PAWNS etc, they're computed at the beginning
               promoPiece = PROMO_PIECE(moveCode);
               SWITCH_BIT(gs->boards[promoPiece], targetSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
               SWITCH_BIT(gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;

               gs->fiftyMovePly = gs->gameHist[gs->histIndex][fifty_move_ply];
               break;

           // Quiet Moves
           case 1: // Double Pawn Push
               originPiece = ALLY_PAWNS;

               // Replace Origin Piece at originSquare
               SWITCH_BIT(gs->boards[originPiece], originSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
               SWITCH_BIT(gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;

               // Remove Moved Piece
               SWITCH_BIT(gs->boards[originPiece], targetSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
               SWITCH_BIT(gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;

               gs->fiftyMovePly = gs->gameHist[gs->histIndex][fifty_move_ply];
               break;
           case 6: // PAWN QUIET MOVE
               originPiece = ALLY_PAWNS;

               // Remove Origin Piece
               SWITCH_BIT(gs->boards[originPiece], originSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
               SWITCH_BIT(gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;

               // Remove Moved Piece
               SWITCH_BIT(gs->boards[originPiece], targetSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
               SWITCH_BIT(gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;

               gs->fiftyMovePly = gs->gameHist[gs->histIndex][fifty_move_ply];
               break;
           case 0: // QuietMove
               originPiece = gs->squareOccupancy[targetSquare];
               // Replace Origin Piece
               SWITCH_BIT(gs->boards[originPiece], originSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
               SWITCH_BIT(gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;

               // Remove Moved Piece
               SWITCH_BIT(gs->boards[originPiece], targetSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
               SWITCH_BIT(gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;


               gs->fiftyMovePly--;
               break;
           case 2: // Short Castle
               // Replace Rooks in original positions
               SWITCH_BIT(gs->boards[ALLY_ROOKS], originSquare + 3);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare + 3);
               SWITCH_BIT(gs->boards[aPieces], originSquare + 3);
               gs->squareOccupancy[originSquare + 3] = ALLY_ROOKS;

               // Remove Rook form castled position
               SWITCH_BIT(gs->boards[ALLY_ROOKS], originSquare + 1);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare + 1);
               SWITCH_BIT(gs->boards[aPieces], originSquare + 1);
               gs->squareOccupancy[originSquare + 1] = no_pce;

               // Replace Origin King in original square
               SWITCH_BIT(gs->boards[ALLY_KINGS], originSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
               SWITCH_BIT(gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = ALLY_KINGS;

               // Remove Moved King
               SWITCH_BIT(gs->boards[ALLY_KINGS], targetSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
               SWITCH_BIT(gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;

               gs->fiftyMovePly--;
               break;
           case 3: // Long Castle
               // Replace Rooks in origin Square
               SWITCH_BIT(gs->boards[ALLY_ROOKS], originSquare - 4 );
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare - 4);
               SWITCH_BIT(gs->boards[aPieces], originSquare - 4);
               gs->squareOccupancy[originSquare - 4] = ALLY_ROOKS;

               // Remove Rook from moved squares
               SWITCH_BIT(gs->boards[ALLY_ROOKS], originSquare - 1);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare - 1);
               SWITCH_BIT(gs->boards[aPieces], originSquare - 1);
               gs->squareOccupancy[originSquare - 1] = no_pce;

               // Replace Origin King
               SWITCH_BIT(gs->boards[ALLY_KINGS], originSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
               SWITCH_BIT(gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = ALLY_KINGS;

               // Remove Moved King
               SWITCH_BIT(gs->boards[ALLY_KINGS], targetSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
               SWITCH_BIT(gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;

               gs->fiftyMovePly--;
               break;

           case 5:; // EP Capture
               Square enPassantSquare = targetSquare - PAWN_FORWARD_MOVE;

               //Replace Origin Pawn
               SWITCH_BIT(gs->boards[ALLY_PAWNS], originSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
               SWITCH_BIT(gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = ALLY_PAWNS;

               //Remove Moved Pawn
               SWITCH_BIT(gs->boards[ALLY_PAWNS], targetSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
               SWITCH_BIT(gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;

               //Replace Captured Pawn
               SWITCH_BIT(gs->boards[ENEMY_PAWNS], enPassantSquare);
               SWITCH_BIT(gs->boards[ENEMY_PIECES], enPassantSquare);
               SWITCH_BIT(gs->boards[aPieces], enPassantSquare);
               gs->squareOccupancy[enPassantSquare] = ENEMY_PAWNS;

               gs->fiftyMovePly = gs->gameHist[gs->histIndex][fifty_move_ply];
               break;
           case 4: // Regular Capture
               originPiece = gs->squareOccupancy[targetSquare];
               targetPiece = gs->gameHist[gs->histIndex][captured_piece];

               //Replace origin piece
               SWITCH_BIT(gs->boards[originPiece], originSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], originSquare);
               SWITCH_BIT(gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;

               // Remove Moved Piece
               SWITCH_BIT(gs->boards[originPiece], targetSquare);
               SWITCH_BIT(gs->boards[ALLY_PIECES], targetSquare);
               SWITCH_BIT(gs->boards[aPieces], targetSquare);

               // Replace captured piece
               SWITCH_BIT(gs->boards[targetPiece], targetSquare);
               SWITCH_BIT(gs->boards[ENEMY_PIECES], targetSquare);
               SWITCH_BIT(gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = targetPiece;

           }

    // Update Game Ply
    gs->plyNum--;

    return 0;
}
/* -------------- TEST IF IN CHECK -------------------- */

int inCheck(Square kingPos, GameState* gs){
    /*
    Params:
        Square kingPos      - square of king whose side is being evaluated (passed as argument since sometimes it is desired to check if a different square is in check)
        GameState* gs       - pointer to current gamestate
    Returns:
        int                - -1 if in check; 0 otherwise
    */


    if(gs->sideToMove == wPieces){
        // White To Move

        // Rooks and Queens
        //TODO: Fix this
        BitBoard rookMoves = getRookMoveBoard(kingPos, gs);
        if(rookMoves & (gs->boards[bRooks] | gs->boards[bQueens])){
            return -1;
        }

        // Bishops and Queens
        if(getBishopMoveBoard(kingPos, gs) & (gs->boards[bBishops] | gs->boards[bQueens])){
            return -1;
        }

        //Knights
        if(knightMoves[kingPos] & gs->boards[bKnights]){
            return -1;
        }

        //Pawns
        if( ((kingPos % 8) != 0) && (getBit(&gs->boards[bPawns], kingPos + 7))){
            return -1;
        }
        if( ((kingPos % 8) != 7) && (getBit(&gs->boards[bPawns], kingPos + 9))){
            return -1;
        }
        // Other King
        if(kingMoves[kingPos] & gs->boards[bKings]){
            return -1;
        }
        // Not in Check
        return 0;
    }else{
        // Black To Move
        // Rooks and Queens
        if(getRookMoveBoard(kingPos, gs) & (gs->boards[wRooks] | gs->boards[wQueens])){
            return -1;
        }

        // Bishops and Queens
        if(getBishopMoveBoard(kingPos, gs) & (gs->boards[wBishops] | gs->boards[wQueens])){
            return -1;
        }

        //Knights
        if(knightMoves[kingPos] & gs->boards[wKnights]){
            return -1;
        }

        //Pawns
        if( ((kingPos % 8) != 0) && (getBit(&gs->boards[wPawns], kingPos - 9))){
            return -1;
        }
        if( ((kingPos % 8) != 7) && (getBit(&gs->boards[wPawns], kingPos - 7))){
            return -1;
        }
        // Other King
        if(kingMoves[kingPos] & gs->boards[wKings]){
            return -1;
        }
        return 0;
    }
}
