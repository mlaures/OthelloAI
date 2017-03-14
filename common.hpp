#ifndef __COMMON_H___
#define __COMMON_H___

#include <queue>
#include <cstdlib>
//#include <mutex>
//#include <thread>
#define NUMCOEFFS 64


enum Side { 
    WHITE = 0, BLACK = 1
};

class Move {
   
public:
    int x, y;
    Move(int x, int y) {
        this->x = x;
        this->y = y;        
    }
    ~Move() {}

    int getX() { return x; }
    int getY() { return y; }

    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
};

#endif
