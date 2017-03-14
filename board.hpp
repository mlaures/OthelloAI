#ifndef __BOARD_H__
#define __BOARD_H__

#define BSINDEX(x, y) (y + 8 * x)

#include <bitset>
#include "common.hpp"
#include <vector>
#define BOARD_SIZE 64
using namespace std;

class Player;

class Board {

    friend class Player;

private:
    bitset<BOARD_SIZE> black;
    bitset<BOARD_SIZE> taken;

    bool occupied(int x, int y);
    bool get(Side side, int x, int y);
    void set(Side side, int x, int y);
    bool onBoard(int x, int y);

public:
    Board();
    ~Board();
    Board *copy();

    bool isDone();
    bool hasMoves(Side side);
    bool checkMove(Move *m, Side side);
    vector<Move*>* getAvailableMoves(Side side);
    void doMove(Move *m, Side side);
    int count(Side side);
    int countBlack();
    int countWhite();

    double getScoreSimple(Side side);
    double getScore();
    double getScore(double* heuristic_coeffs, int size);

    void setBoard(char data[]);
};

#endif
