#include "player.hpp"

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
	testingMinimax = false;
    board = new Board();
    this->side = side;
    this->opponentSide = (side == BLACK)? WHITE : BLACK;
    this->heuristic_coeffs = new double[NUMCOEFFS]{39.2928, 12.6684, 70.5414, 58.5276, 70.9759, 53.456, 58.71, 52.0131, 68.3668, 87.6974, 46.347, 36.8787, 40.8381, 47.001, 36.7967, 58.9839, 23.5245, 1.765, 62.9877, 49.6765, 38.5423, 68.1263, 37.8112, 52.8047, 47.0454, 46.0186, 47.9441, 37.6486, 70.2449, 28.2655, 41.3637, 53.5852, 45.1353, 32.1717, 80.1915, 27.7968, 56.6192, 44.0209, 31.7398, 68.6455, 57.3332, 11.9536, 48.2346, 28.3595, 13.663, 35.9712, 70.3972, 10.9688, 59.5487, 25.9196, 5.37993, 72.2764, 56.764, 54.0235, 48.776, 44.0339, 67.5034, 57.6998, 48.3722, 10.9963, 48.7062, 81.0067, 62.7486, 56.7311};
	num_wins = 0;
    /*
     * TODO: Do any initialization you need to do here (setting up the board,
     * precalculating things, etc.) However, remember that you will only have
     * 30 seconds.
     */
}

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side side, bool random) {
    // Will be set to true in test_minimax.cpp.
	testingMinimax = false;
    board = new Board();
    this->side = side;
    this->opponentSide = (side == BLACK)? WHITE : BLACK;
    this->heuristic_coeffs = new double[NUMCOEFFS];
	for(int i = 0; i < NUMCOEFFS; i++)
	{
		heuristic_coeffs[i] = fRand(LOWER, UPPER);
	}
    num_wins = 0;
    /*
     * TODO: Do any initialization you need to do here (setting up the board,
     * precalculating things, etc.) However, remember that you will only have
     * 30 seconds.
     */
}

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side side, double* heuristic_coeffs, int size) {
    // Will be set to true in test_minimax.cpp.
	testingMinimax = false;
    board = new Board();
    this->side = side;
    this->opponentSide = (side == BLACK)? WHITE : BLACK;
    this->heuristic_coeffs = heuristic_coeffs;
    num_wins = 0;
    /*
     * TODO: Do any initialization you need to do here (setting up the board,
     * precalculating things, etc.) However, remember that you will only have
     * 30 seconds.
     */
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be
 * nullptr.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return nullptr.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {

	if(opponentsMove != nullptr)
		board->doMove(opponentsMove, opponentSide);
	vector<Move*>* avail_moves = board->getAvailableMoves(side);
	int size = (int) avail_moves->size();
	if(size == 0)
		return nullptr; // must pass turn
	Move* best_move = nullptr;

	double ab[2] {-MAX_DOUBLE, MAX_DOUBLE};

	double best_score = -MAX_DOUBLE;
	for(vector<Move*>::iterator it = avail_moves->begin();
		it != avail_moves->end(); it++)
	{
		Board* copy = board->copy();
		Move* temp_move = *it;
		copy->doMove(temp_move, side);
		double score = -nalphabeta(opponentSide, copy, MAX_DEPTH - 1, ab);
		if(score > best_score)
	 	{
	 		best_score = score;
	 		best_move = temp_move;
	 	}
		delete copy;
	}
	for(int i = 0; i < avail_moves->size(); i++)
	{
		if(avail_moves->at(i) != best_move)
			delete avail_moves->at(i);
	}
	delete avail_moves;
	board->doMove(best_move, side);
    return best_move;
	// double best_score = -MAX_DOUBLE;
	// for(vector<Move*>::iterator it = avail_moves->begin();
	// 		it != avail_moves->end(); ++it)
	// {
	// 	Board* copy = board->copy();
	// 	Move* temp_move = *it;
	// 	cerr << temp_move->x << " " << temp_move->y << endl;
	// 	copy->doMove(temp_move, side);
	// 	double score = -negamax(opponentSide, copy, MAX_DEPTH - 1);
	// 	if(score > best_score)
	// 	{
	// 		best_score = score;
	// 		best_move = temp_move;
	// 	}
	// }
}

void Player::setSide(Side side)
{
	this->side = side;
	this->opponentSide = (side == BLACK)? WHITE : BLACK;
}

double Player::nalphabeta(Side cside, Board* board, int depth, double ab[])
{
	if(depth == 0 && this->testingMinimax)
		return board->getScoreSimple(cside);
	else if(depth == 0 && !this->testingMinimax)
		return cside == WHITE ? -board->getScore(heuristic_coeffs, NUMCOEFFS)
				: board->getScore(heuristic_coeffs, NUMCOEFFS);

	vector<Move*>* avail_moves = board->getAvailableMoves(cside);
	double better = -MAX_DOUBLE;
	if (avail_moves->size() != 0) 
	{
		for(vector<Move*>::iterator it = avail_moves->begin();
					it != avail_moves->end(); it++)
		{
			Board* copy = board->copy();
			copy->doMove(*it, cside);
			double score = -nalphabeta((Side)!cside, copy, depth-1, ab);
			better = max (better, score);
			if (cside == this->side)
			{
				ab[0] = max (ab[0], better);
			} else
			{
				ab[1] = max (ab[1], better);
			}
			delete copy;
			if (ab[1] <= ab[0])
				break;
		}
		for(int i = 0; i < avail_moves->size(); i++)
			delete avail_moves->at(i);
		delete avail_moves;
//		cerr << "better = " << better << endl;
		return better;
	} else {
		delete avail_moves;
		return nalphabeta((Side)!cside, board, depth-1, ab);
	}
}

//double Player::alphabeta(Side cside, Board* board, int depth,
//	double alpha, double beta)
//{
//	if(depth == 0) /*&& this->testingMinimax)*/
//		return board->getScoreSimple(cside);
//	/*else if(depth == 0 && !this->testingMinimax)
//		return cside == WHITE ? -board->getScore() : board->getScore();
//*/
//	vector<Move*>* avail_moves = board->getAvailableMoves(cside);
//	if (avail_moves->size() != 0)
//	{
//		if (cside == this->side)
//		{
//			double better = -MAX_DOUBLE;
//			for(vector<Move*>::iterator it = avail_moves->begin();
//				it != avail_moves->end(); it++)
//			{
//				Board* copy = board->copy();
//				copy->doMove(*it, cside);
//				double score = alphabeta((Side)!cside, copy, depth-1, alpha, beta);
//				better = max (better, score);
//				alpha = max(alpha, better);
//				if (beta <= alpha)
//					break;
//
//			}
//			return better;
//		} else
//		{
//			double better = MAX_DOUBLE;
//			for(vector<Move*>::iterator it = avail_moves->begin();
//					it != avail_moves->end(); it++)
//			{
//				Board* copy = board->copy();
//				copy->doMove(*it, cside);
//				double score = -alphabeta((Side)!cside, copy, depth-1, alpha, beta);
//				better = min (better, score);
//				beta = min (beta, better);
//				if (beta <= alpha)
//					break;
//			}
//			return better;
//		}
	

//double Player::minimax(Side curr_side, Board* board, int depth)
//{
//	this->testingMinimax = true;
//	if(depth == 0 && this->testingMinimax)
//		return board->getScoreSimple(curr_side);
//	else if(depth == 0 && !this->testingMinimax)
//		return curr_side == WHITE ? -board->getScore() : board->getScore();
//	vector<Move*>* avail_moves = board->getAvailableMoves(curr_side);
//	// TODO NOTE: if our heuristic for Black = -White, we can simplify
//	// the below code.
//	if(avail_moves->size() == 0)
//		return minimax((Side)!curr_side, board, depth - 1);
//	if(curr_side == this->side) // Choose the highest scoring option
//	{
//		double max_score = -MAX_DOUBLE;
//		for(vector<Move*>::iterator it = avail_moves->begin();
//				it != avail_moves->end(); it++)
//		{
//			Board* copy = board->copy();
//			copy->doMove(*it, curr_side);
//			double score = minimax((Side)!curr_side, copy, depth - 1);
////			cerr << "In max, score = " << score << endl;
//			max_score = max(max_score, score);
//		}
////		cerr << "max_score = " << max_score << endl;
//		return max_score;
//	}
//	else // Choose the lowest scoring option
//	{
//		double min_score = MAX_DOUBLE;
//
//		for(vector<Move*>::iterator it = avail_moves->begin();
//				it != avail_moves->end(); it++)
//		{
//			Board* copy = board->copy();
//			copy->doMove(*it, curr_side);
//			double score = minimax((Side)!curr_side, copy, depth - 1);
////			cerr << "In min, score = " << score << endl;
//			min_score = min(min_score, score);
//		}
////		cerr << "min_score = " << min_score << endl;
//		return min_score;
//	}
//}
