#include "player.hpp"
#include "board.hpp"
#include "common.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

#define NUMPLAYERS 10
#define VARIABILITY 2
#define PROB_MUTATE 0.15
#define PROB_MIX 0.15
#define NUMGENERATIONS 100


using namespace std;

//double fRand(double fMin, double fMax)
//{
//    double f = (double)rand() / RAND_MAX;
//    return fMin + f * (fMax - fMin);
//}


/**
 * @return 1 if white wins, 0 if black wins, -1 on tie.
 */
int play(Player* p1, Player* p2)
{
	p1->setSide(BLACK);
	p2->setSide(WHITE);
	Move* move = nullptr;
//	int count = 0;
	while(!p2->board->isDone())
	{
		move = p1->doMove(move, 0);
//		count++;
		if(p1->board->isDone())
			break;
		move = p2->doMove(move, 0);
//		count++;
	}
	int score = p1->board->getScoreSimple(WHITE);
//	cout << "count: " << count << endl;
	return score < 0 ? 0 : score > 0 ? 1 : -1;
}

double getRandomPlayersHeuristic(vector<Player*> &players, double weights[],
									int heuristic_index)
{
	double random = fRand(0, 1);
	int i = -1;
	while(random > 0 && i < (int)(players.size() - 1))
	{
		random -= weights[++i];
	}
	return players[i]->heuristic_coeffs[heuristic_index];
}

vector<Player*> createNewPlayers(vector<Player*> &players)
{
	vector<Player*> ret_players;
	sort(players.begin(), players.end());
	double *player_weights = new double[NUMPLAYERS];
	double total_wins = (NUMPLAYERS - 1) * (NUMPLAYERS);
	for(int i = 0; i < NUMPLAYERS; i++)
		player_weights[i] = players[i]->num_wins / total_wins;

	for(int i = 0; i < NUMPLAYERS; i++)
	{
		//TODO: rework to make heuristic a double array, and don't reallocate players
		double* heuristics = new double[NUMCOEFFS];
		for(int j = 0; j < NUMCOEFFS; j++)
		{
			double random = fRand(0, 1);
			// MUTATE
			if(random < PROB_MUTATE)
			{
				heuristics[j] = fRand(LOWER, UPPER);
				continue;
			}
			// Genetic mixing
			else if(random < PROB_MUTATE + PROB_MIX)
			{
				double coeff1 = getRandomPlayersHeuristic(players, player_weights, j);
				double coeff2 = getRandomPlayersHeuristic(players, player_weights, j);
				heuristics[j] = (coeff1 + coeff2) / 2.0;
				continue;
			}
			else
			{
				heuristics[j] = getRandomPlayersHeuristic(players, player_weights, j);
			}
			if(random < 0.5)
				heuristics[j] += fRand(-VARIABILITY, VARIABILITY);
		}
		Player* player = new Player(BLACK, heuristics, NUMCOEFFS);
		ret_players.push_back(player);
	}
	for(int i = 0; i < NUMPLAYERS; i++)
		delete players[i];
	delete[] player_weights;
	return ret_players;
}

int main(int argc, char** argv)
{
	srand(time(NULL));
	Board* reset = new Board();
	vector<Player*> players;

	// Generate random players for initialization
	for(int i = 0; i < NUMPLAYERS; i++)
	{
		players.push_back(new Player(BLACK));
	}

	int num_generations = 0;
	while(num_generations++ < NUMGENERATIONS)
	{
		for(int i = 0; i < NUMPLAYERS; i++)
		{
			for(int j = 0; j < NUMPLAYERS; j++)
			{
				if(i == j)
					continue;
				int win = play(players[i], players[j]);
				delete players[i]->board;
				delete players[j]->board;
				players[i]->board = reset->copy();
				players[j]->board = reset->copy();
				// player[i] = black, player[j] = white
				if(win == 0)
				{
					players[i]->num_wins++;
				}
				else if(win == 1)
				{
					players[j]->num_wins++;
				}
			}
		}
		for(int i = 0; i < NUMPLAYERS; i++)
		{
			cout << "Player " << i << " has " << players[i]->num_wins << " wins. " << endl;
			cout << "Parameters: ";
			for(int j = 0; j < NUMCOEFFS; j++)
			{
				cout << players[i]->heuristic_coeffs[j] << " ";
			}
			cout << endl;
		}
		players = createNewPlayers(players);
	}
	for(int i = 0; i < NUMPLAYERS; i++)
		delete players[i];
	delete reset;
}
