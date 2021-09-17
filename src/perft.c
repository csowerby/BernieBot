//
//  Perft.c
//  BernieBot
//
//  Created by Charlie Sowerby on 9/12/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include "perft.h"


int checkIfGameStateConsistent(GameState *gs){

    // Compare any two bitboards -> return 1 if they overlap
    BitBoard test1 = 0ULL;
    for(int i = 2; i < 14;i++){
        test1 = gs->boards[i];
        for(int j = 2; j < 14; j++){
            if (i == j){continue;}
            test1 &= gs->boards[j];
            if(test1 != 0ULL){
                return 1;
            }
        }
    }

    // TEST SUM OF PIECES EQUALS sidePieces
    // white pieces
    test1 = 0ULL;
    for(int i = 2; i < 8; i++){
        test1 |= gs->boards[i];
    }
    if(test1 != gs->boards[bPieces]){
        //BitBoard diff = test1 ^ gs->boards[bPieces];
        //printf("There was a difference in sum of black pieces and gs->boards[bPieces]: diff below\n");
        //printBitBoard(&diff);
        return 2;

    }
    //black pieces
    test1 = 0ULL;
    for(int i = 8; i < 14; i++){
        test1 |= gs->boards[i];
    }
    if(test1 != gs->boards[wPieces]){
        return 3;
    }

    //all pieces
    if((gs->boards[wPieces] | gs->boards[bPieces]) != gs->boards[aPieces]){
        return 4;
    }

    // Check square occupancy is redundant with bitboards
    for(int i = 0; i < 64; i++){
        Piece occupancy = gs->squareOccupancy[i];
        if(occupancy == no_pce){
            if(getBit(&gs->boards[aPieces], i)){
                return -i;
            }
        }else{
            if(!getBit(&gs->boards[gs->squareOccupancy[i]], i)){
                return -i;
            }
        }
    }



    return 0;
}



uint64_t Perft(int depth, GameState *gs){
    // SPEED VERSION
    Move move_list[MOVE_LIST_LENGTH] = {0};
    int n_moves, i;
    uint64_t nodes = 0;

    if (depth == 0)
        return 1ULL;
    n_moves = moveGen(move_list, gs);
    for (i = 0; i < n_moves; i++) {
        // Loop through and make all the moves
        int success = makeMove(gs, move_list[i]);
        if (success != -1){
            nodes += Perft(depth - 1, gs);
        }
        unmakeMove(gs, move_list[i]);
    }
    return nodes;

}


uint64_t Perft_speedtest(int depth, GameState *gs, uint64_t* moveGeneration, uint64_t* moveMaking, uint64_t* moveUnmaking, uint64_t* abortedMoves ){
    // VERSION WITH TIMING
    Move move_list[MOVE_LIST_LENGTH];
    int n_moves, i;
    uint64_t nodes = 0;
    if (depth == 0){
        return 1ULL;
    }
    clock_t start = clock();
    n_moves = moveGen(move_list, gs);
    clock_t finish = clock();
    *moveGeneration += finish-start;

    for(i = 0; i < n_moves; i++){
        start = clock();
        int success = makeMove(gs, move_list[i]);
        finish = clock();
        *moveMaking += finish-start;

        if(success != -1){
            nodes += Perft_speedtest(depth - 1, gs, moveGeneration, moveMaking, moveUnmaking, abortedMoves);
        }else{
            *abortedMoves += 1;
        }


        start = clock();
        unmakeMove(gs, move_list[i]);
        finish = clock();
        *moveUnmaking += finish - start;
    }
    return nodes;
}


uint64_t Perft_debug(int depth, GameState *gs){
    // DEBUGGING VERSION
    assert(gs->boards[wKings]);
    assert(gs->boards[bKings]);
    int consistencyTest;
    // int consistencyTest = checkIfGameStateConsistent(gs);
    // if(consistencyTest != 0){
    //     printf("CONSISTENCY ERROR THROWN: %i\n", consistencyTest);
    //     printGameStateInfo(gs, true);
    //     assert(false);
    // }

    int DEBUGprintmoveinfo = 0;
    // Start
    Move move_list[MOVE_LIST_LENGTH] = {0};
    int n_moves, i;
    uint64_t nodes = 0;

    if (depth == 0)
        return 1ULL;

    GameState tempGS;
    memcpy(&tempGS, gs, sizeof(GameState));

    n_moves = moveGen(move_list, gs);
    assert(gs->boards[wKings]);
    assert(gs->boards[bKings]);

    // consistencyTest = checkIfGameStateConsistent(gs);
    // if(consistencyTest != 0){
    //     printf("CONSISTENCY ERROR THROWN: %i\n", consistencyTest);
    //     printGameStateInfo(gs, true);
    //     assert(false);
    // }

    if(DEBUGprintmoveinfo){
        for(int i = 0; i < n_moves; i++){
            printMoveInfo(move_list[i]);
        }
    }



    for (i = 0; i < n_moves; i++) {
        // Loop through and make all the moves

        //printMoveInfo(move_list[i]);

        int success = makeMove(gs, move_list[i]);


        if((gs->enPassantTarget/8 != 2) && (gs->enPassantTarget / 8 != 5) && gs->enPassantTarget != no_sqr){
            printf("Previous game state:\n");
            printGameStateInfo(&tempGS, true);
            printf("CURRENT GS: \n");
            printGameStateInfo(gs, false);
            printf("Move made: \n");
            printMoveInfo(move_list[i]);
            assert(false);
        }

        consistencyTest = checkIfGameStateConsistent(gs);
        if(consistencyTest != 0){
            printf("CONSISTENCY ERROR THROWN: %i\n", consistencyTest);
            printf("INITIAL GAMESTATE: \n");
            printGameStateInfo(&tempGS, true);
            printf("GAMESTATE AFTER MOVE MADE:\n");
            printGameStateInfo(gs, true);
            printf("Move Made:");
            printMoveInfo(move_list[i]);
            assert(false);
        }

        assert(gs->boards[wKings]);
        assert(gs->boards[bKings]);
        //printGameStateInfo(gs, true);
        int divide_moves = 0;
        if (success != -1){
            divide_moves = Perft_debug(depth - 1, gs);
            nodes += divide_moves;
        }else{
            //printf("bad move\n");
        }

        if (depth == 8){
            printMoveInfo(move_list[i]);
            printf("Number of response moves in this position: %i\n\n------------\n", divide_moves);
        }


        unmakeMove(gs, move_list[i]);


        consistencyTest = checkIfGameStateConsistent(gs);
        if(consistencyTest != 0){
            printf("CONSISTENCY ERROR THROWN: %i\n", consistencyTest);
            printGameStateInfo(gs, true);
            printf("Move Unmade:");
            printMoveInfo(move_list[i]);
            assert(false);
        }

        assert(gs->boards[wKings]);
        assert(gs->boards[bKings]);

        int testGS = compareGameStates(gs, &tempGS);

        if(testGS != 0){
            printf("ERROR CODE: %i\n", testGS);
            printf("MOVE: \n");
            printMoveInfo(move_list[i]);
            printf("ORIGINAL GAMESTATE:\n");
            printGameStateInfo(&tempGS, true);
            printf("MODIFIED GAMESTATE: \n");
            printGameStateInfo(gs, true);
            assert(false);
        }
    }
    return nodes;
}

void startPerft(int depth, char* fen){
    init();
    GameState gs;
    init_GameState(&gs, fen);
    printf("Starting Perft...\n\n");



    clock_t executionStart = clock();


    uint64_t total_nodes = 0;
    for(int i = 0; i <= depth; i++){

        clock_t perftStart = clock();
        uint64_t nodes = Perft(i, &gs);
        clock_t perftEnd = clock();
        double perft_time = (double) (perftEnd - perftStart)/CLOCKS_PER_SEC;
        printf("Perft(%i): %10llu    |  %6.2f s\n" ,i ,nodes, perft_time);
        total_nodes += nodes;
    }
    printf("\nTotal Nodes Accssed: %llu\n", total_nodes);


    clock_t executionEnd = clock();
    double elapsedTime = (double) (executionEnd - executionStart)/ CLOCKS_PER_SEC;
    //printf("Execution finished in %f seconds.\n", elapsedTime);

    uint64_t nps = total_nodes/elapsedTime;
    double Mnps = nps/1000000.0;

    printf("Average nodes per second (approx): %.2f Mnps\n", Mnps);


}
