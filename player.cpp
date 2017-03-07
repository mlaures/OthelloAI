#include "player.hpp"

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
	double best_score = -MAX_DOUBLE;
	for(vector<Move*>::iterator it = avail_moves->begin();
			it != avail_moves->end(); ++it)
	{
		Board* copy = board->copy();
		Move* temp_move = *it;
//		cerr << temp_move->x << " " << temp_move->y << endl;
		copy->doMove(temp_move, side);
		double score = minimax(opponentSide, copy, MAX_DEPTH);
		if(score > best_score)
		{
			best_score = score;
			best_move = temp_move;
		}
	}
	board->doMove(best_move, side);
    return best_move;
}

double Player::minimax(Side curr_side, Board* board, int depth)
{
	this->testingMinimax = true;
	if(depth == 0 && this->testingMinimax)
		return board->getScoreSimple(this->side);
	else if(depth == 0 && !this->testingMinimax)
		return curr_side == WHITE ? -board->getScore() : board->getScore();
	vector<Move*>* avail_moves = board->getAvailableMoves(curr_side);
	// TODO NOTE: if our heuristic for Black = -White, we can simplify
	// the below code.
	if(avail_moves->size() == 0)
		return minimax((Side)!curr_side, board, depth - 1);
	if(curr_side == this->side) // Choose the highest scoring option
	{
		double max_score = -MAX_DOUBLE;
		for(vector<Move*>::iterator it = avail_moves->begin();
				it != avail_moves->end(); it++)
		{
			Board* copy = board->copy();
			copy->doMove(*it, curr_side);
			double score = minimax((Side)!curr_side, copy, depth - 1);
//			cerr << "In max, score = " << score << endl;
			max_score = max(max_score, score);
		}
//		cerr << "max_score = " << max_score << endl;
		return max_score;
	}
	else // Choose the lowest scoring option
	{
		double min_score = MAX_DOUBLE;

		for(vector<Move*>::iterator it = avail_moves->begin();
				it != avail_moves->end(); it++)
		{
			Board* copy = board->copy();
			copy->doMove(*it, curr_side);
			double score = minimax((Side)!curr_side, copy, depth - 1);
//			cerr << "In min, score = " << score << endl;
			min_score = min(min_score, score);
		}
//		cerr << "min_score = " << min_score << endl;
		return min_score;
	}
}
