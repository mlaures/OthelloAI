#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.hpp"
#include "board.hpp"
#include <limits>
using namespace std;

#define MAX_DEPTH 4
#define MAX_DOUBLE numeric_limits<double>::max()

//
//struct MoveAndScore
//{
//	Move m;
//	double s;
//};

class Player {

public:
    Player(Side side);
    ~Player();

    Move *doMove(Move *opponentsMove, int msLeft);
    double minimax(Side side, Board* board, int depth);
    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
    Side side;
    Side opponentSide;
    Board* board;
};

#endif
