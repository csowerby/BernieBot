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
    
    Move* move_list;
    
    int n_moves, i;
    n_moves = moveGen(&move_list, gs);
    for(i = 0; i < n_moves; i++){
        //printMoveInfo(&move_list[i]);
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
    printMoveInfo(&bestMove);
    printf("Evaluation: %f\n", maxScore);
    return bestMove;
}


int negaMax(int depth, GameState *gs){
    if(depth == 0) return EVALUATE(gs);
    double maxScore = INT_MIN;
    
    //Generate MoveList
    Move* move_list;
    int n_moves, i;
    n_moves = moveGen(&move_list, gs);
    
    // Loop Through Moves
    for(i = 0; i < n_moves; i++){
        //printMoveInfo(&move_list[i]);
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
    free(move_list);
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
    
    int sideToMove = -1;
    if(gs->whiteToMove) sideToMove = 1;
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
    
    

    Piece originPiece, targetPiece;
    Square targetSquare, originSquare = move >>10;
    
    uint8_t moveCode = move & 15;
    
    
    int sidePieces, oppositeSidePieces, colorToMoveOffset, colorToMoveMultiple;
    if(gs->whiteToMove){
        sidePieces = wPieces;
        oppositeSidePieces = bPieces;
        colorToMoveOffset = 0;
        colorToMoveMultiple = 1;
    }else{
        sidePieces = bPieces;
        oppositeSidePieces = wPieces;
        colorToMoveOffset = 6;
        colorToMoveMultiple = -1; 
    }
    
    // Add info to Hist
    gs->gameHist[gs->histIndex][castling_rights] = gs->castlingPrivileges;
    gs->gameHist[gs->histIndex][ep_target] = gs->enPassantTarget;

    
    
    //Set EP Square to no_sqr (will be reset to the correct later if necessary
    gs->enPassantTarget = no_sqr;
    
    switch (moveCode) {
            // Promo Capture - knight, bishop, rook, queen
        case 12:
        case 13:
        case 14:
        case 15:
            targetSquare = (move >> 4) & 63;
            originPiece = wPawns + colorToMoveOffset;
            targetPiece = gs->squareOccupancy[targetSquare];
            gs->gameHist[gs->histIndex][captured_piece] = targetPiece;
            // Remove Origin Piece
            switchBit(&gs->boards[originPiece], originSquare);
            switchBit(&gs->boards[sidePieces], originSquare);
            switchBit(&gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;
            
            // Remove Target Piece
            switchBit(&gs->boards[targetPiece], targetSquare);
            switchBit(&gs->boards[oppositeSidePieces], targetSquare);
            
            //Add Promoted Piece
            Piece promoPiece = (moveCode & 3) + 1 + colorToMoveOffset;
            switchBit(&gs->boards[promoPiece], targetSquare);
            switchBit(&gs->boards[sidePieces], targetSquare);
            gs->squareOccupancy[targetSquare] = promoPiece;
            
            gs->fiftyMovePly = 0;
            gs->enPassantTarget = no_sqr;
            
            // Castling Privileges
            switch (targetSquare) {
                case a1:
                    clearBit((BitBoard*)&gs->castlingPrivileges, 2); // white long castle
                    break;
                case a8:
                    clearBit((BitBoard*)&gs->castlingPrivileges, 0); // black long castle
                case h1:
                    clearBit((BitBoard*)&gs->castlingPrivileges, 3); // white short castle
                case h8:
                    clearBit((BitBoard*)&gs->castlingPrivileges, 1); // black short castle
                    break;
                default:
                    break;
            }
            break;
 
            // Regular Promos - knight, bishop, rook, queen
        case 8:
        case 9:
        case 10:
        case 11:
            targetSquare = originSquare + (8 * colorToMoveMultiple);
            originPiece = wPawns + colorToMoveOffset;
            // Remove Origin Piece
            switchBit(&gs->boards[originPiece], originSquare);
            switchBit(&gs->boards[sidePieces], originSquare);
            switchBit(&gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;
            
            //Add Promoted Piece
            promoPiece = (moveCode & 3) + 1 + colorToMoveOffset;
            switchBit(&gs->boards[promoPiece], targetSquare);
            switchBit(&gs->boards[sidePieces], targetSquare);
            switchBit(&gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = promoPiece;
            
            gs->fiftyMovePly = 0;
            gs->enPassantTarget = no_sqr;
            break;

        // Quiet Moves
        case 1: // Double Pawn Push
            targetSquare = originSquare + (16 * colorToMoveMultiple);
            originPiece = wPawns + colorToMoveOffset;
            gs->enPassantTarget = (originSquare + targetSquare)/2;
            
            // Remove Origin Piece
            switchBit(&gs->boards[originPiece], originSquare);
            switchBit(&gs->boards[sidePieces], originSquare);
            switchBit(&gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;
            
            // Add Moved Piece
            switchBit(&gs->boards[originPiece], targetSquare);
            switchBit(&gs->boards[sidePieces], targetSquare);
            switchBit(&gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;
            
            gs->fiftyMovePly = 0;
            break;
        case 6: // PAWN QUIET MOVE
            originPiece = wPawns + colorToMoveOffset;
            targetSquare = originSquare + 8 * colorToMoveMultiple;
            
            // Remove Origin Piece
            switchBit(&gs->boards[originPiece], originSquare);
            switchBit(&gs->boards[sidePieces], originSquare);
            switchBit(&gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;
            
            // Add Moved Piece
            switchBit(&gs->boards[originPiece], targetSquare);
            switchBit(&gs->boards[sidePieces], targetSquare);
            switchBit(&gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;
            
            gs->enPassantTarget = no_sqr;
            gs->fiftyMovePly = 0;
            break;
        case 0: // QuietMove
            targetSquare = (move >> 4) & 63;
            originPiece = gs->squareOccupancy[originSquare];
            // Remove Origin Piece
            switchBit(&gs->boards[originPiece], originSquare);
            switchBit(&gs->boards[sidePieces], originSquare);
            switchBit(&gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;
            
            // Add Moved Piece
            switchBit(&gs->boards[originPiece], targetSquare);
            switchBit(&gs->boards[sidePieces], targetSquare);
            switchBit(&gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;
            
            gs->fiftyMovePly++;
            gs->enPassantTarget = no_sqr;
            break;
        case 2: // Short Castle
            originPiece = wKings + colorToMoveOffset;
            targetSquare = originSquare + 2;
            // Remove Rooks
            switchBit(&gs->boards[wRooks + colorToMoveOffset], originSquare + 3);
            switchBit(&gs->boards[sidePieces], originSquare + 3);
            switchBit(&gs->boards[aPieces], originSquare + 3);
            gs->squareOccupancy[originSquare + 3] = no_pce;
                
            // Add Rook
            switchBit(&gs->boards[wRooks + colorToMoveOffset], originSquare + 1);
            switchBit(&gs->boards[sidePieces], originSquare + 1);
            switchBit(&gs->boards[aPieces], originSquare + 1);
            gs->squareOccupancy[originSquare + 1] = wRooks + colorToMoveOffset;
            
            // Remove Origin King
            switchBit(&gs->boards[originPiece], originSquare);
            switchBit(&gs->boards[sidePieces], originSquare);
            switchBit(&gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;
            
            // Add Moved King
            switchBit(&gs->boards[originPiece], targetSquare);
            switchBit(&gs->boards[sidePieces], targetSquare);
            switchBit(&gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;
            
            gs->fiftyMovePly++;
            gs->enPassantTarget = no_sqr;
            break;
        case 3: // Long Castle
            originPiece = wKings + colorToMoveOffset;
            targetSquare = originSquare - 2;
            // Remove Rooks
            switchBit(&gs->boards[wRooks + colorToMoveOffset], originSquare - 4 );
            switchBit(&gs->boards[sidePieces], originSquare - 4);
            switchBit(&gs->boards[aPieces], originSquare - 4);
            gs->squareOccupancy[originSquare - 4] = no_pce;
                
            // Add Rook
            switchBit(&gs->boards[wRooks + colorToMoveOffset], originSquare - 1);
            switchBit(&gs->boards[sidePieces], originSquare - 1);
            switchBit(&gs->boards[aPieces], originSquare - 1);
            gs->squareOccupancy[originSquare - 1] = wRooks + colorToMoveOffset;
            
            // Remove Origin King
            switchBit(&gs->boards[originPiece], originSquare);
            switchBit(&gs->boards[sidePieces], originSquare);
            switchBit(&gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;
            
            // Add Moved King
            switchBit(&gs->boards[originPiece], targetSquare);
            switchBit(&gs->boards[sidePieces], targetSquare);
            switchBit(&gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;
            
            gs->fiftyMovePly++;
            gs->enPassantTarget = no_sqr;
            break;

        case 5:; // EP Capture
            targetSquare = (move >> 4) & 63;
            Square enPassantSquare = targetSquare - (8 *colorToMoveMultiple);
            originPiece = wPawns + colorToMoveOffset;
            targetPiece = bPawns - colorToMoveOffset;
            
            //Remove Origin Pawn
            switchBit(&gs->boards[originPiece], originSquare);
            switchBit(&gs->boards[sidePieces], originSquare);
            switchBit(&gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;
            
            //Add Moved Pawn
            switchBit(&gs->boards[originPiece], targetSquare);
            switchBit(&gs->boards[sidePieces], targetSquare);
            switchBit(&gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;
            
            //Remove Captured Pawn
            switchBit(&gs->boards[targetPiece], enPassantSquare);
            switchBit(&gs->boards[oppositeSidePieces], enPassantSquare);
            switchBit(&gs->boards[aPieces], enPassantSquare);
            gs->squareOccupancy[enPassantSquare] = no_pce;
            
            gs->fiftyMovePly = 0;
            break;
        case 4: // Regular Capture
            targetSquare = (move >> 4) & 63;
            originPiece = gs->squareOccupancy[originSquare];
            targetPiece = gs->squareOccupancy[targetSquare];
            gs->gameHist[gs->histIndex][captured_piece] = targetPiece;
            
            //Remove origin piece
            switchBit(&gs->boards[originPiece], originSquare);
            switchBit(&gs->boards[sidePieces], originSquare);
            switchBit(&gs->boards[aPieces], originSquare);
            gs->squareOccupancy[originSquare] = no_pce;
            
            // Remove captured piece
            switchBit(&gs->boards[targetPiece], targetSquare);
            switchBit(&gs->boards[oppositeSidePieces], targetSquare);
            switchBit(&gs->boards[aPieces], targetSquare);
            
            // Add Moved Piece
            switchBit(&gs->boards[originPiece], targetSquare);
            switchBit(&gs->boards[sidePieces], targetSquare);
            switchBit(&gs->boards[aPieces], targetSquare);
            gs->squareOccupancy[targetSquare] = originPiece;
            
            gs->fiftyMovePly = 0;
            
            switch (targetSquare) {
                case a1:
                    clearBit((BitBoard*)&gs->castlingPrivileges, 2); // white long castle
                    break;
                case a8:
                    clearBit((BitBoard*)&gs->castlingPrivileges, 0); // black long castle
                case h1:
                    clearBit((BitBoard*)&gs->castlingPrivileges, 3); // white short castle
                case h8:
                    clearBit((BitBoard*)&gs->castlingPrivileges, 1); // black short castle
                    break;
                default:
                    break;
            }
            
            break;
    }

    // Update Game Ply
    gs->plyNum++;
    
    // Update Castling Priviledges\
    //TODO: if a rook is captured also update castling privilegs
    switch (originSquare) {
        case e1:
            clearBit((BitBoard*)&gs->castlingPrivileges, 2); // white long castle
            clearBit((BitBoard*)&gs->castlingPrivileges, 3); // white short castle
            break;
        case a1:
            clearBit((BitBoard*)&gs->castlingPrivileges, 2); // white long castle
            break;
        case h1:
            clearBit((BitBoard*)&gs->castlingPrivileges, 3); // white short castle
            break;
        case a8:
            clearBit((BitBoard*)&gs->castlingPrivileges, 0); // black long castle
            break;
        case h8:
            clearBit((BitBoard*)&gs->castlingPrivileges, 1); // black short castle
        case e8:
            clearBit((BitBoard*)&gs->castlingPrivileges, 0); // white long castle
            clearBit((BitBoard*)&gs->castlingPrivileges, 1); // white short castle
            break;
            
        default:
            break;
    }

    
    gs->histIndex++;

    // Check if position Legal
    
    Square kingPos = get_ls1b_pos(&gs->boards[wKings + colorToMoveOffset]);
    if (inCheck(kingPos, gs)){
        gs->whiteToMove = !gs->whiteToMove;
        return -1;
    }
    
    //Switch who is on move.
    gs->whiteToMove = !gs->whiteToMove;
    return 0;
}

int unmakeMove(GameState *gs, Move move){
    //Revert who is on move
    gs->whiteToMove = !gs->whiteToMove;
    
    // Restore info from history stack
    gs->histIndex--;
    gs->enPassantTarget = gs->gameHist[gs->histIndex][ep_target];
    gs->castlingPrivileges = gs->gameHist[gs->histIndex][castling_rights];
    
    int sidePieces, oppositeSidePieces, colorToMoveOffset, colorToMoveMultiple;
    if(gs->whiteToMove){
        sidePieces = wPieces;
        oppositeSidePieces = bPieces;
        colorToMoveOffset = 0;
        colorToMoveMultiple = 1;
    }else{
        sidePieces = bPieces;
        oppositeSidePieces = wPieces;
        colorToMoveOffset = 6;
        colorToMoveMultiple = -1;
    }

    Square targetSquare, originSquare = move>> 10;
    Piece originPiece, targetPiece;
    
    int moveCode = move & 15;
    
       switch (moveCode) {
               // Promo Capture - knight, bishop, rook, queen
           case 12:
           case 13:
           case 14:
           case 15:
               targetSquare = (move >> 4) & 63;
               originPiece = wPawns + colorToMoveOffset;
               targetPiece = gs->gameHist[gs->histIndex][captured_piece];
               
               // Replace Origin Piece
               switchBit(&gs->boards[originPiece], originSquare);
               switchBit(&gs->boards[sidePieces], originSquare);
               switchBit(&gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;
               
               //Remove Promoted Piece
               Piece promoPiece = (moveCode & 3) + 1 + colorToMoveOffset;
               switchBit(&gs->boards[promoPiece], targetSquare);
               switchBit(&gs->boards[sidePieces], targetSquare);
               
               
               // Replace Target Piece
               switchBit(&gs->boards[targetPiece], targetSquare);
               switchBit(&gs->boards[oppositeSidePieces], targetSquare);
               gs->squareOccupancy[targetSquare] = targetPiece;

               //TODO: - restore fifty move ply from the stack
               break;
    
               // Regular Promos - knight, bishop, rook, queen
           case 8:
           case 9:
           case 10:
           case 11:
               targetSquare = originSquare + (8* colorToMoveMultiple);
               originPiece = wPawns + colorToMoveOffset;
               // Replace Origin Piece
               switchBit(&gs->boards[originPiece], originSquare);
               switchBit(&gs->boards[sidePieces], originSquare);
               switchBit(&gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;
               
               //Remove Promoted Piece
               promoPiece = (moveCode & 3) + 1 + colorToMoveOffset;
               switchBit(&gs->boards[promoPiece], targetSquare);
               switchBit(&gs->boards[sidePieces], targetSquare);
               switchBit(&gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;
               
               //TODO: restore fifty move ply from the stack
               break;

           // Quiet Moves
           case 1: // Double Pawn Push
               targetSquare = originSquare + 16 * colorToMoveMultiple;
               originPiece = wPawns + colorToMoveOffset;
               
               // Replace Origin Piece at originSquare
               switchBit(&gs->boards[originPiece], originSquare);
               switchBit(&gs->boards[sidePieces], originSquare);
               switchBit(&gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;
               
               // Remove Moved Piece
               switchBit(&gs->boards[originPiece], targetSquare);
               switchBit(&gs->boards[sidePieces], targetSquare);
               switchBit(&gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;
               
               //TODO: restore fifty move ply from stack
               break;
           case 6: // PAWN QUIET MOVE
               originPiece = wPawns + colorToMoveOffset;
               targetSquare = originSquare + 8 * colorToMoveMultiple;
               
               // Remove Origin Piece
               switchBit(&gs->boards[originPiece], originSquare);
               switchBit(&gs->boards[sidePieces], originSquare);
               switchBit(&gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;
               
               // Remove Moved Piece
               switchBit(&gs->boards[originPiece], targetSquare);
               switchBit(&gs->boards[sidePieces], targetSquare);
               switchBit(&gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;
               
               //TODO: restore fifty move ply from stack
               break;
           case 0: // QuietMove
               targetSquare = (move >> 4) & 63;
               originPiece = gs->squareOccupancy[targetSquare];
               // Replace Origin Piece
               switchBit(&gs->boards[originPiece], originSquare);
               switchBit(&gs->boards[sidePieces], originSquare);
               switchBit(&gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;
               
               // Remove Moved Piece
               switchBit(&gs->boards[originPiece], targetSquare);
               switchBit(&gs->boards[sidePieces], targetSquare);
               switchBit(&gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;
               
               
               gs->fiftyMovePly--;
               break;
           case 2: // Short Castle
               originPiece = wKings + colorToMoveOffset;
               targetSquare = originSquare + 2;
               // Replace Rooks in original positions
               switchBit(&gs->boards[wRooks + colorToMoveOffset], originSquare + 3);
               switchBit(&gs->boards[sidePieces], originSquare + 3);
               switchBit(&gs->boards[aPieces], originSquare + 3);
               gs->squareOccupancy[originSquare + 3] = wRooks + colorToMoveOffset;
                   
               // Remove Rook form castled position
               switchBit(&gs->boards[wRooks + colorToMoveOffset], originSquare + 1);
               switchBit(&gs->boards[sidePieces], originSquare + 1);
               switchBit(&gs->boards[aPieces], originSquare + 1);
               gs->squareOccupancy[originSquare + 1] = no_pce;
               
               // Replace Origin King in original square
               switchBit(&gs->boards[originPiece], originSquare);
               switchBit(&gs->boards[sidePieces], originSquare);
               switchBit(&gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;
               
               // Remove Moved King
               switchBit(&gs->boards[originPiece], targetSquare);
               switchBit(&gs->boards[sidePieces], targetSquare);
               switchBit(&gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;
               
               gs->fiftyMovePly--;
               break;
           case 3: // Long Castle
               originPiece = wKings + colorToMoveOffset;
               targetSquare = originSquare - 2;
               // Replace Rooks in origin Square
               switchBit(&gs->boards[wRooks + colorToMoveOffset], originSquare - 4 );
               switchBit(&gs->boards[sidePieces], originSquare - 4);
               switchBit(&gs->boards[aPieces], originSquare - 4);
               gs->squareOccupancy[originSquare - 4] = wRooks + colorToMoveOffset;
                   
               // Remove Rook from moved squares
               switchBit(&gs->boards[wRooks + colorToMoveOffset], originSquare - 1);
               switchBit(&gs->boards[sidePieces], originSquare - 1);
               switchBit(&gs->boards[aPieces], originSquare - 1);
               gs->squareOccupancy[originSquare - 1] = no_pce;
               
               // Replace Origin King
               switchBit(&gs->boards[originPiece], originSquare);
               switchBit(&gs->boards[sidePieces], originSquare);
               switchBit(&gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = wKings + colorToMoveOffset;
               
               // Remove Moved King
               switchBit(&gs->boards[originPiece], targetSquare);
               switchBit(&gs->boards[sidePieces], targetSquare);
               switchBit(&gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;
               
               gs->fiftyMovePly--;
               break;

           case 5:; // EP Capture
               targetSquare = (move >> 4) & 63;
               Square enPassantSquare = targetSquare - (8 *colorToMoveMultiple);
               originPiece = wPawns + colorToMoveOffset;
               targetPiece = bPawns - colorToMoveOffset;

               //Replace Origin Pawn
               switchBit(&gs->boards[originPiece], originSquare);
               switchBit(&gs->boards[sidePieces], originSquare);
               switchBit(&gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;
               
               //Remove Moved Pawn
               switchBit(&gs->boards[originPiece], targetSquare);
               switchBit(&gs->boards[sidePieces], targetSquare);
               switchBit(&gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = no_pce;
               
               //Replace Captured Pawn
               switchBit(&gs->boards[targetPiece], enPassantSquare);
               switchBit(&gs->boards[oppositeSidePieces], enPassantSquare);
               switchBit(&gs->boards[aPieces], enPassantSquare);
               gs->squareOccupancy[enPassantSquare] = targetPiece;
               
               //TODO: reset fifty move ply from stack
               break; 
           case 4: // Regular Capture
               targetSquare = (move >> 4) & 63;
               originPiece = gs->squareOccupancy[targetSquare];
               targetPiece = gs->gameHist[gs->histIndex][captured_piece];
               
               //Replace origin piece
               switchBit(&gs->boards[originPiece], originSquare);
               switchBit(&gs->boards[sidePieces], originSquare);
               switchBit(&gs->boards[aPieces], originSquare);
               gs->squareOccupancy[originSquare] = originPiece;
               
               // Remove Moved Piece
               switchBit(&gs->boards[originPiece], targetSquare);
               switchBit(&gs->boards[sidePieces], targetSquare);
               switchBit(&gs->boards[aPieces], targetSquare);
               
               // Replace captured piece
               switchBit(&gs->boards[targetPiece], targetSquare);
               switchBit(&gs->boards[oppositeSidePieces], targetSquare);
               switchBit(&gs->boards[aPieces], targetSquare);
               gs->squareOccupancy[targetSquare] = targetPiece;
               
               //TODO: reset fifty move ply from stack
               break;
       }

    // Update Game Ply
    gs->plyNum--;

    return 0; 
}
/* -------------- TEST IF IN CHECK -------------------- */

int inCheck(Square kingPos, GameState* gs){
    if(gs->whiteToMove){
        // White To Move
        
        // Rooks and Queens
        if(getRookMoveBoard(kingPos, gs) & (gs->boards[bRooks] | gs->boards[bQueens])){
            return 1;
        }
        
        // Bishops and Queens
        if(getBishopMoveBoard(kingPos, gs) & (gs->boards[bBishops] | gs->boards[bQueens])){
            return 1;
        }
        
        //Knights
        if(knightMoves[kingPos] & gs->boards[bKnights]){
            return 1;
        }
        
        //Pawns
        if( kingPos % 8 != 0 && (getBit(&gs->boards[bPawns], kingPos + 7))){
            return 1;
        }
        if( kingPos % 8 != 7 && (getBit(&gs->boards[bPawns], kingPos + 9))){
            return 1;
        }
        // Not in Check
        return 0;
    }else{
        // Black To Move
        
        // Rooks and Queens
        //BitBoard board1 = getRookMoveBoard(kingPos, gs);
        //BitBoard board2 =(gs->boards[wRooks] | gs->boards[wQueens]);
        //printBitBoard(&board2);
        //printBitBoard(&board1); 
        if(getRookMoveBoard(kingPos, gs) & (gs->boards[wRooks] | gs->boards[wQueens])){
            return 1;
        }
        
        // Bishops and Queens
        if(getBishopMoveBoard(kingPos, gs) & (gs->boards[wBishops] | gs->boards[wQueens])){
            return 1;
        }
        
        //Knights
        if(knightMoves[kingPos] & gs->boards[wKnights]){
            return 1;
        }
        
        //Pawns
        if( (kingPos % 8) != 0 && (getBit(&gs->boards[wPawns], kingPos - 9))){
            return 1;
        }
        if( (kingPos % 8) != 7 && (getBit(&gs->boards[wPawns], kingPos - 7))){
            return 1;
        }
        return 0;
    }
}



/* ------------ MOVE CALCULATION METHODS ------------- */

int moveGen(Move **totalMoveList, GameState *gs){
    /* Params: Double Pointer to moveList, pointer to gamestate
        returns: integer with length of moveList, -1 if failure, 0 if no moves
     
     
     NOTE- ONE TRY COULD BE TO RETURN -1 IF CAPTURE OF KING IS A LEGAL MOVE, i.e. the last move was illegal and we need to unmake the last move
     */

    int moveCount = 0;
    Move *moveList;
    moveList = malloc(218 * sizeof(Move));

    // CASTLING
    /* Procedure for checkingn if castling is legal
        - If you have castling rights
        - If No pieces are in between
        - If not in check
        - If no squares in between are in check
     */
    if(gs->whiteToMove){
        if(!inCheck(e1, gs)){
            // Check if not in Check
            if(getBit((BitBoard*)&gs->castlingPrivileges, 3)){
                // Check if White Short Castling Privileges
                if (!(w_short_castle_squares & gs->boards[aPieces])){
                    // No Pieces in the way
                    if(!inCheck(f1, gs)){
                        if(!inCheck(g1, gs)){
                            // Squares between aren't in check
                            
                            // Short Castling is available - append now
                            moveList[moveCount] = 0;
                            moveList[moveCount] += g1 << 4;
                            moveList[moveCount] += e1 << 10;
                            //Code for short Castle 0010
                            moveList[moveCount] += 2;
                            moveCount++;
                        }
                    }
                }
            }
            if(getBit((BitBoard*)&gs->castlingPrivileges, 2)){
                // Check if White Long Castling Privileges
                if (!(w_long_castle_squares & gs->boards[aPieces])){
                    // No Pieces in the way
                    if(!inCheck(d1, gs)){
                        if(!inCheck(c1, gs)){
                            if(!inCheck(b1, gs)){
                                // Squares between aren't in check
                                
                                // Long Castling is available - append now
                                moveList[moveCount] = 0;
                                moveList[moveCount] += c1 << 4;
                                moveList[moveCount] += e1 << 10;
                                //Code for long Castle 0011
                                moveList[moveCount] += 3;
                                moveCount++;
                            }
                        }
                    }
                }
            }
        }
    }else{
        if(!inCheck(e8, gs)){
            // Check if not in Check
            if(getBit((BitBoard*)&gs->castlingPrivileges, 0)){
                // Check if Black Short Castling Privileges
                if (!(b_short_castle_squares & gs->boards[aPieces])){
                    // No Pieces in the way
                    if(!inCheck(f8, gs)){
                        if(!inCheck(g8, gs)){
                            // Squares between aren't in check
                            
                            // Short Castling is available - append now
                            moveList[moveCount] = 0;
                            moveList[moveCount] += g8 << 4;
                            moveList[moveCount] += e8 << 10;
                            //Code for short Castle 0010
                            moveList[moveCount] += 2;
                            moveCount++;
                        }
                    }
                }
            }
            if(getBit((BitBoard*)&gs->castlingPrivileges, 1)){
                // Check if Black Long Castling Privileges
                if (!(b_long_castle_squares & gs->boards[aPieces])){
                    // No Pieces in the way
                    if(!inCheck(d8, gs)){
                        if(!inCheck(c8, gs)){
                            if(!inCheck(b8, gs)){
                                // Squares between aren't in check
                                
                                // Long Castling is available - append now
                                moveList[moveCount] = 0;
                                moveList[moveCount] += c8 << 4;
                                moveList[moveCount] += e8 << 10;
                                //Code for long Castle 0011
                                moveList[moveCount] += 3;
                                moveCount++;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Append other moves
    
    // Pawn Moves
    Move *pawnMoves = NULL;
    int numPawnMoves = calcPawnMoves(&pawnMoves, gs);
    memcpy(moveList + moveCount, pawnMoves, numPawnMoves *sizeof(Move));
    moveCount += numPawnMoves;
    free(pawnMoves);
    
    int colorToMoveOffset = 6;
    if(gs->whiteToMove){
        colorToMoveOffset = 0;
    }

    //King Moves
    Move *kingMoves = NULL;
    int numKingMoves = calcKingMoves(&kingMoves,get_ls1b_pos(&gs->boards[wKings + colorToMoveOffset]), gs);
    memcpy(moveList +moveCount, kingMoves, numKingMoves *sizeof(Move));
    moveCount += numKingMoves;
    free(kingMoves);
    
    //Sliding Moves
    BitBoard rookAndQueenPositions = gs->boards[wQueens+ colorToMoveOffset] | gs->boards[wRooks + colorToMoveOffset];
    while(rookAndQueenPositions){
        Square slidingPieceSquare = get_ls1b_pos(&rookAndQueenPositions);
        clearBit(&rookAndQueenPositions, slidingPieceSquare);
        
        Move *slidingMoves = NULL;
        int numSlidingMoves = calcRookMoves(&slidingMoves, slidingPieceSquare, gs);
        memcpy(moveList + moveCount, slidingMoves, numSlidingMoves * sizeof(Move));
        moveCount += numSlidingMoves;
        free(slidingMoves);
    }
    
    // Diagonal Moves
    BitBoard bishopAndQueenPositions = gs->boards[wQueens + colorToMoveOffset] | gs->boards[wBishops + colorToMoveOffset];
    while(bishopAndQueenPositions){
        Square diagPieceSquare = get_ls1b_pos(&bishopAndQueenPositions);
        clearBit(&bishopAndQueenPositions, diagPieceSquare);
        
        Move *diagMoves = NULL;
        int numDiagMoves = calcBishopMoves(&diagMoves, diagPieceSquare, gs);
        memcpy(moveList + moveCount, diagMoves, numDiagMoves * sizeof(Move));
        moveCount += numDiagMoves;
        free(diagMoves);
    }
    
    //KnightMoves
    BitBoard knightPositions = gs->boards[wKnights + colorToMoveOffset];
    while(knightPositions){
        Square knightSquare = get_ls1b_pos(&knightPositions);
        clearBit(&knightPositions, knightSquare);
        
        Move *knightMoves = NULL;
        int numKnightMoves = calcKnightMoves(&knightMoves, knightSquare, gs);
        memcpy(moveList + moveCount, knightMoves, numKnightMoves * sizeof(Move));
        moveCount += numKnightMoves;
        
        
        free(knightMoves);
    }
    
    moveList = realloc(moveList, moveCount * sizeof(Move));
    *totalMoveList = moveList;
    
    return moveCount;
}


BitBoard getBishopMoveBoard(Square origin_sq, GameState* gs){
    BitBoard occ = gs->boards[aPieces];
    occ &= bishopMagics[origin_sq].mask;
    occ *= bishopMagics[origin_sq].magic;
    occ >>= bishopMagics[origin_sq].shift;
    return bishopMagics[origin_sq].attackPtr[occ];
}

BitBoard getRookMoveBoard(Square origin_sq, GameState* gs){
    BitBoard occ = gs->boards[aPieces];
    occ &= rookMagics[origin_sq].mask;
    occ *= rookMagics[origin_sq].magic;
    occ >>= rookMagics[origin_sq].shift;
    return rookMagics[origin_sq].attackPtr[occ];
}


inline int calcKnightMoves(Move **knightList, Square origin_sq, GameState *gs){
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
        moveList[moveCount] = 0;
        targetSquare = get_ls1b_pos(&moveBoard);
        clearBit(&moveBoard, targetSquare);
        moveList[moveCount] += targetSquare << 4;
        moveList[moveCount] += origin_sq << 10;
        
        // Check if capture, if so append 100 on the end
        /* TODO: - THIS COULD BE DONE MUCH FASTER USING BITBOARDS NOT ONE AT A TIME - FIX THIS IN THE FUTURE */
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

    moveList = realloc(moveList, moveCount * sizeof(Move));
    *knightList = moveList;
    
    
    return moveCount;
}


inline int calcKingMoves(Move **kingList, Square origin_sq, GameState *gs){
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
        //printBitBoard(&moveBoard);
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

inline int calcRookMoves(Move **slidingList, Square origin_sq, GameState *gs){
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

inline int calcBishopMoves(Move **diagList, Square origin_sq, GameState *gs){
    // Allocate 13 Moves worth of memory because its the maximum move capacity
    Move* moveList = (Move*)malloc(13* sizeof(Move));
    int moveCount = 0;
        
    BitBoard occ = gs->boards[aPieces];
    occ &= bishopMagics[origin_sq].mask;
    occ *= bishopMagics[origin_sq].magic;
    occ >>= bishopMagics[origin_sq].shift;
    BitBoard moves = bishopMagics[origin_sq].attackPtr[occ];
    //printBitBoard(&moves);
        
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

inline int calcPawnMoves(Move **pawnList, GameState *gs){
    // Allocate 8 * 4 = 32 Moves worth of memory because its the maximum move capacity of all pawns
    Move* moveList = (Move*)malloc(32* sizeof(Move));
    int moveCount = 0;

    if(gs->whiteToMove){
        BitBoard pawns = gs->boards[wPawns];
        //ATTACKS
        BitBoard leftPawnAttackPromos = ((pawns & notAFile)<<7) & gs->boards[bPieces] & rank8;
        //printBitBoard(&leftPawnAttackPromos);
        BitBoard rightPawnAttackPromos = ((pawns & notHFile)<<9) & gs->boards[bPieces] & rank8;
        //printBitBoard(&rightPawnAttackPromos);
        BitBoard leftPawnAttacks = ((pawns & notAFile)<<7) & gs->boards[bPieces] & notRank8;
        //printBitBoard(&leftPawnAttacks);
        BitBoard rightPawnAttacks = ((pawns & notHFile)<<9) & gs->boards[bPieces] & notRank8;
        //printBitBoard(&rightPawnAttacks);
        
        //QUIET MOVES
        BitBoard pawnMoveTargets = (pawns << 8) & ~gs->boards[aPieces] & notRank8;
        //printBitBoard(&pawnMoveTargets);
        BitBoard pawnMovePromo = (pawns << 8) &~gs->boards[aPieces] & rank8;
        //printBitBoard(&pawnMovePromo);
        BitBoard pawnDoubleMoveTargets = ((pawns & rank2) << 16) & ~gs->boards[aPieces] & ~(gs->boards[aPieces] << 8);
        //printBitBoard(&pawnDoubleMoveTargets);
        
        
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
            CLEAR_LS1B(rightPawnAttackPromos);
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
            CLEAR_LS1B(pawnDoubleMoveTargets);
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
                moveList[moveCount] = 0;
                moveList[moveCount] += gs->enPassantTarget << 4;
                moveList[moveCount] += (gs->enPassantTarget - 7) << 10;
                // Add ep-code 0101
                moveList[moveCount] += 5;
                moveCount++;
            }
            if (getBit(&rightEP, gs->enPassantTarget)){
                moveList[moveCount] = 0; 
                moveList[moveCount] += gs->enPassantTarget << 4;
                moveList[moveCount] += (gs->enPassantTarget - 9) << 10;
                // Add ep-code 0101
                moveList[moveCount] += 5;
                moveCount++;
            }
        }
    }else{
        // Black to Move
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
            BitBoard rightEP = ((pawns & notHFile)>>7);
            if (getBit(&leftEP, gs->enPassantTarget)){
                moveList[moveCount] = 0;
                moveList[moveCount] += gs->enPassantTarget << 4;
                moveList[moveCount] += (gs->enPassantTarget + 9) << 10;
                
                // Add ep-code 0101
                moveList[moveCount] += 5;
                moveCount++;
            }
            if (getBit(&rightEP, gs->enPassantTarget)){
                moveList[moveCount] = 0; 
                moveList[moveCount] += gs->enPassantTarget << 4;
                moveList[moveCount] += (gs->enPassantTarget + 7) << 10;
                
                // Add ep-code 0101
                moveList[moveCount] += 5;
                moveCount++;
            }
        }
    }
    
   
        
    moveList = (Move*) realloc(moveList, moveCount * sizeof(Move));
    *pawnList = moveList;
    return moveCount;
}