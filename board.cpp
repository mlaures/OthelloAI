#include "board.hpp"

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() {
    taken.set(3 + 8 * 3);
    taken.set(3 + 8 * 4);
    taken.set(4 + 8 * 3);
    taken.set(4 + 8 * 4);
    black.set(4 + 8 * 3);
    black.set(3 + 8 * 4);
}

/*
 * Destructor for the board.
 */
Board::~Board() {
}

/*
 * Returns a copy of this board.
 */
Board *Board::copy() {
    Board *newBoard = new Board();
    newBoard->black = black;
    newBoard->taken = taken;
    return newBoard;
}

bool Board::occupied(int x, int y) {
    return taken[x + 8*y];
}

bool Board::get(Side side, int x, int y) {
    return occupied(x, y) && (black[x + 8*y] == (side == BLACK));
}

void Board::set(Side side, int x, int y) {
    taken.set(x + 8*y);
    black.set(x + 8*y, side == BLACK);
}

bool Board::onBoard(int x, int y) {
    return(0 <= x && x < 8 && 0 <= y && y < 8);
}


/*
 * Returns true if the game is finished; false otherwise. The game is finished
 * if neither side has a legal move.
 */
bool Board::isDone() {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}

/*
 * Returns true if there are legal moves for the given side.
 */
bool Board::hasMoves(Side side) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move move(i, j);
            if (checkMove(&move, side)) return true;
        }
    }
    return false;
}

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(Move *m, Side side) {
    // Passing is only legal if you have no moves.
    if (m == nullptr) return !hasMoves(side);

    int X = m->getX();
    int Y = m->getY();

    // Make sure the square hasn't already been taken.
    if (occupied(X, Y)) return false;

    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (onBoard(x, y) && get(other, x, y)) {
                do {
                    x += dx;
                    y += dy;
                } while (onBoard(x, y) && get(other, x, y));

                if (onBoard(x, y) && get(side, x, y)) return true;
            }
        }
    }
    return false;
}

/**
 * @return: a vector of all possible moves for Side side.
 */
vector<Move*>* Board::getAvailableMoves(Side side)
{
	vector<Move*>* ret = new vector<Move*>();
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			Move* move = new Move(i, j);
			if (checkMove(move, side))
			{
				ret->push_back(move);
			}
			else
				delete move;
		}
	}
	return ret;
}

/*
 * Modifies the board to reflect the specified move.
 */
void Board::doMove(Move *m, Side side) {
    // A nullptr move means pass.
    if (m == nullptr) return;

    // Ignore if move is invalid.
    if (!checkMove(m, side)) return;

    int X = m->getX();
    int Y = m->getY();
    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (onBoard(x, y) && get(other, x, y));

            if (onBoard(x, y) && get(side, x, y)) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (onBoard(x, y) && get(other, x, y)) {
                    set(side, x, y);
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    set(side, X, Y);
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return black.count();
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return taken.count() - black.count();
}

/*
 * Sets the board state given an 8x8 char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken.reset();
    black.reset();
    for (int i = 0; i < 64; i++) {
        if (data[i] == 'b') {
            taken.set(i);
            black.set(i);
        } if (data[i] == 'w') {
            taken.set(i);
        }
    }
}

double Board::getScoreSimple(Side side)
{
	return (side == BLACK) ? countBlack() - countWhite() :
												countWhite() - countBlack();
}

double Board::getScore() // Assume black
{
	double sum = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			int color = !(occupied(i, j))? 0 : (black[i + 8*j] ? 1 : -1);
			if (color != 0)
			{
				if((i == 0 || i == 7) && (j == 0 || j == 7)) // corners
					sum += 5 * color;
				else if(i == 0 || i == 7 || j == 0 || j == 7) // edges
					sum += 3 * color;
				else
					sum += color;
			}
		}
	}
	return sum;
}

double Board::getScore(double* heuristic_coeffs, int size) // Assume black
{
	double sum = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		int color = !(taken[i])? 0 : (black[i] ? 1 : -1);
		if (color != 0)
		{
			sum += heuristic_coeffs[i] * color;
		}
	}
	return sum;
}

//double Board::getScore(double* heuristic_coeffs, int size) // Assume black
//{
//	double sum = 0;
//	double x1 = *(heuristic_coeffs);
//	double x2 = *(++heuristic_coeffs);
//	double x3 = *(++heuristic_coeffs);
//	for (int i = 0; i < 8; i++) {
//		for (int j = 0; j < 8; j++) {
//			int color = !(occupied(i, j))? 0 : (black[i + 8*j] ? 1 : -1);
//			if (color != 0)
//			{
//				if((i == 0 || i == 7) && (j == 0 || j == 7)) // corners
//					sum += x1 * color;
//				else if(i == 0 || i == 7 || j == 0 || j == 7) // edges
//					sum += x2 * color;
//				else
//					sum += x3 * color;
//			}
//		}
//	}
//	return sum;
//}
