#include "player.hpp"
#include "board.hpp"
#include "common.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>

#define NUMPLAYERS 128
#define NTHREADS 16
#define VARIABILITY 2
#define PROB_VARIANCE .7
#define PROB_MUTATE 0.005
#define PROB_MIX 0.35
#define NUMGENERATIONS 10000


using namespace std;

mutex qMutex;


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
	double *player_weights = new double[NUMPLAYERS];
	double total_wins = (NUMPLAYERS - 1) * (NUMPLAYERS);
	for(int i = 0; i < NUMPLAYERS; i++)
		player_weights[i] = players[i]->num_wins / total_wins;


	/**
	 * Elitism: keep the best player of the previous generation
	 */
	int max_wins = 0;
	Player* max_player = nullptr;
	for(int i = 0; i < NUMPLAYERS; i++)
	{
		if(players[i]->num_wins > max_wins)
		{
			max_wins = players[i]->num_wins;
			max_player = players[i];
		}
	}
	ret_players.push_back(max_player);

	/**
	 * Genetic recombination: generate the new players as a weighted-random
	 * combination of the old.
	 */
	for(int i = 1; i < NUMPLAYERS; i++)
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
			if(random < PROB_VARIANCE)
				heuristics[j] += fRand(-VARIABILITY, VARIABILITY);
		}
		Player* player = new Player(BLACK, heuristics, NUMCOEFFS);
		ret_players.push_back(player);
	}
	for(int i = 0; i < NUMPLAYERS; i++)
	{
		if(players[i] != ret_players[0])
			delete players[i];
	}
	delete[] player_weights;
	max_player->num_wins = 0;
	return ret_players;
}

void playOneRound(queue<pair<Player*, Player*>>* queue,
						pair<Player*, Player*> pair , Board* reset) {
	Player* player1 = pair.first;
	Player* player2 = pair.second;

	bool p1, p2;
	p1 = player1->playing.try_lock();
	p2 = player2->playing.try_lock();

	if (p1 && p2)
	{
		int win = play(player1, player2);
		delete player1->board;
		delete player2->board;
		player1->board = reset->copy();
		player2->board = reset->copy();
		if(win == 0)
			player1->num_wins++;
		else if(win == 1)
			player2->num_wins++;
		// any other stuff needed after finished playing
		player1->playing.unlock();
		player2->playing.unlock();
	} else
	{
		if (p1)
		{
			player1->playing.unlock();
		} else if (p2)
		{
			player2->playing.unlock();
		}

		qMutex.lock();
		queue->push(pair);
		qMutex.unlock();
	}
//	cout << "q size: " << queue->size() << endl;
}

void threadfunc(queue<pair<Player*, Player*>>* queue, Board* reset)
{
	while(true)
	{
		qMutex.lock();
		if(queue->empty())
		{
			qMutex.unlock();
			break;
		}
		pair<Player*, Player*> pair = queue->front();
		queue->pop();
		qMutex.unlock();
		playOneRound(queue, pair, reset);
	}
}

int main(int argc, char** argv)
{
	srand(time(NULL));
	Board* reset = new Board();
	vector<Player*> players;
	thread **t = new thread *[NTHREADS];


	// Generate random players for initialization
	for(int i = 0; i < NUMPLAYERS; i++)
	{
		players.push_back(new Player(BLACK, true));
	}
	int num_generations = 0;
	while(num_generations++ < NUMGENERATIONS)
	{
		queue<pair<Player*, Player*>>* q = new queue<pair<Player*, Player*>>();
		for(int i = 0; i < NUMPLAYERS; i++)
		{
			for(int j = 0; j < NUMPLAYERS; j++)
			{
				if(i == j)
					continue;
				q->push(make_pair(players[i], players[j]));
			}
		}
		for(int i = 0; i < NTHREADS; i++)
		{
			t[i] = new thread(threadfunc, q, reset);
		}
		for(int i = 0; i < NTHREADS; i++)
		{
			t[i]->join();
			delete t[i];
		}
		int max_wins = 0;
		Player* max_player = nullptr;
		for(int i = 0; i < NUMPLAYERS; i++)
		{
			if(players[i]->num_wins > max_wins)
			{
				max_wins = players[i]->num_wins;
				max_player = players[i];
			}
		}
		cout << "Current epoch: " << num_generations;
		cout << " Max wins: " << max_wins << endl;
		cout << "{";
		for(int j = 0; j < NUMCOEFFS; j++)
		{
			cout << max_player->heuristic_coeffs[j];
			if(j != NUMCOEFFS - 1)
				cout << ", ";
		}
		cout << "}" << endl;
		/*int sum = 0;
		for(int i = 0; i < NUMPLAYERS; i++)
		{
			cout << "Number of wins of player " << i << ": " << players[i]->num_wins << endl;
			sum += players[i]->num_wins;
		}
		cout << "sum = " << sum;*/
		cout << endl << endl << endl;
		players = createNewPlayers(players);
	}
	for(int i = 0; i < NUMPLAYERS; i++)
		delete players[i];
	delete reset;
}
