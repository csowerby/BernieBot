//
//  Perft.c
//  BernieBot
//
//  Created by Charlie Sowerby on 9/12/21.
//  Copyright © 2021 Charlie Sowerby. All rights reserved.
//

#include "Perft.h"


uint64_t Perft(int depth){
    
    GameState gs;
    init_GameState(&gs, NULL);
    
    Move* move_list = NULL;
    int n_moves, i;
    uint64_t nodes = 0;

    if (depth == 0)
        return 1ULL;

    n_moves = moveGen(&move_list, &gs);
    for (i = 0; i < n_moves; i++) {
        // Record irreversible info - castling rights, EP,
        int success = makeMove(&gs, move_list[i]);
        if (success != -1){
            nodes += Perft(depth - 1);
        }
        unmakeMove(&gs, move_list[i]);
    }
    return nodes;
}
