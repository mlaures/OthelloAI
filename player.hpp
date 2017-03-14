#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.hpp"
#include "board.hpp"
#include <limits>
#include <cstdlib>
#include <mutex>
//#include <random>
using namespace std;

#define MAX_DEPTH 5
#define MAX_DOUBLE numeric_limits<double>::max()
#define LOWER 0
#define UPPER 100

double fRand(double fMin, double fMax);

//
//struct MoveAndScore
//{
//	Move m;
//	double s;
//};

class Player {

public:
    Player(Side side);
    Player(Side side, bool random);
    Player(Side side, double* heuristic_coeffs, int size);
    ~Player();

    Move *doMove(Move *opponentsMove, int msLeft);
    //double minimax(Side side, Board* board, int depth);
    double negamax(Side side, Board* board, int depth);
    double nalphabeta(Side cside, Board* board, int depth, double* ab);
    void setSide(Side side);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
    Side side;
    Side opponentSide;
    Board* board;
    double* heuristic_coeffs;
    // Used for PlayerEvolution
    int num_wins;
    mutex playing;

    bool operator < (const Player& player) const
	{
		return (num_wins < player.num_wins);
	}
};

#endif
