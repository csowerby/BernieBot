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
    for(int i = 0; i < 12;i++){
        test1 = gs->boards[i];
        for(int j = 0; j < 12; j++){
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
    for(int i = 0; i < 6; i++){
        test1 |= gs->boards[i];
    }
    if(test1 != gs->boards[wPieces]){
        return 2;
    }
    //black pieces
    test1 = 0ULL;
    for(int i = 6; i < 12; i++){
        test1 |= gs->boards[i];
    }
    if(test1 != gs->boards[bPieces]){
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
    if (depth == 0){
        return 1ULL;
    }

    n_moves = moveGen(move_list, gs);

    for(i = 0; i < n_moves; i++){
        int success = makeMove(gs, move_list[i]);
        if(success != -1){
            nodes += Perft(depth - 1, gs);
        }
        unmakeMove(gs, move_list[i]);
    }
    free(move_list);
    return nodes;
}


uint64_t Perft_speedtest(int depth, GameState *gs, uint64_t* moveGeneration, uint64_t* moveMaking, uint64_t* moveUnmaking, uint64_t* abortedMoves ){
    // VERSION WITH TIMING
    Move move_list[MOVE_LIST_LENGTH] = {0};
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
    free(move_list);
    return nodes;
}


uint64_t Perft_debug(int depth, GameState *gs){
    // DEBUGGING VERSION
    Move move_list[MOVE_LIST_LENGTH] = {0};
    int n_moves, i;
    uint64_t nodes = 0;

    if (depth == 0)
        return 1ULL;

    GameState tempGS;
    memcpy(&tempGS, gs, sizeof(GameState));

    n_moves = moveGen(move_list, gs);
    /*
    for(int i = 0; i < n_moves; i++){
        printMoveInfo(move_list[i]);
    }
    */

    for (i = 0; i < n_moves; i++) {
        // Loop through and make all the moves

        printMoveInfo(move_list[i]);

        int success = makeMove(gs, move_list[i]);

        printGameStateInfo(gs, true);

        if (success != -1){
            nodes += Perft_debug(depth - 1, gs);
        }

        unmakeMove(gs, move_list[i]);

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
    free(move_list);
    return nodes;
}

void startPerft(int depth){
    init();
    GameState gs;
    init_GameState(&gs, NULL);
    printf("Starting Perft...\n");



    clock_t executionStart = clock();
    uint64_t nodes = Perft(depth, &gs);
    printf("Perft Nodes Accessed: %llu\n", nodes);

    clock_t executionEnd = clock();
    double elapsedTime = (double) (executionEnd - executionStart)/ CLOCKS_PER_SEC;
    printf("Execution finished in %f seconds.\n", elapsedTime);

    uint64_t nps = nodes/elapsedTime;
    double Mnps = nps/1000000.0;

    printf("Nodes per second (approx): %.2f Mnps\n", Mnps);


}
