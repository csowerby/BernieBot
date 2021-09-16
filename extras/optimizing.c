int originSquare(void){
    BitBoard test1 = 1327461983467193764ULL;
    moveList[64];
    int moveCount = 0;
    while(test1){
        Square targetSquare = GET_LS1B(test1);
        Square originSquare = targetSquare - 7;
        CLEAR_LS1B(test1);
        moveList[moveCount] = (targetSquare << 4) | (originSquare << 10);
        // Add Code for Regular Pawn Capture - 0100
        moveList[moveCount] |= CAPTURE;
        moveCount++;
    }
}

int targetSquareminus7(void){
    BitBoard test1 = 1327461983467193764ULL;
    moveList[64];
    int moveCount = 0;
    while(test1){
        Square targetSquare = GET_LS1B(test1);
        CLEAR_LS1B(test1);
        moveList[moveCount] = (targetSquare << 4) | ((targetSquare - 7)<<10);
        // Add Code for Regular Pawn Capture - 0100
        moveList[moveCount] |= CAPTURE;
        moveCount++;
    }
}

int mult(void){
    BitBoard test1 = 1327461983467193764ULL;
    moveList[64];
    int moveCount = 0;
    while(test1){
        Square targetSquare = GET_LS1B(test1);
        CLEAR_LS1B(test1);
        moveList[moveCount] = targetSquare * 1048 - 7168;
        // Add Code for Regular Pawn Capture - 0100
        moveList[moveCount] |= CAPTURE;
        moveCount++;
    }
}
