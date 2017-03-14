Brendan:
Contributed code for base board and player functions; minimax; genetic evolution
Mei-Ling:
Contributed code for negamax; alphabeta; table branch (on github repo)

Neither of us worked completely on our own - 90% of the code was done when we
were both in the room, working together on the couch, constantly bouncing ideas
off of each other and helping each other debug. For example, Brendan started
off the minimax code, but was unable to get it fully working, so Mei-Ling 
stepped in, and transformed it into a working implementation of negamax. We also
worked on many things piecemeal; i.e. Mei-Ling worked on implementing an 
opening table, while Brendan worked on reading and writing the table to a file
(note: this was not fully implemented, as we ran out of time).
All in all, we worked very well together, and it was a ton of fun!


Improvements:
Negamax (more concise version of minimax):
	Negamax finds the "best" move aka the one 
	that yields the best possible score. It looks ahead a given number of moves,
	trying to find the highest scoring outcome given that the opponent plays
	optimally. The "nega" vs "mini" portion of Negamax just means that our
	heuristic is a zero-sum heuristic, i.e. the score for black = the negative
	of the score for white. This lets the code be made more concise.
AlphaBeta (for the negamax function) 
	This algorithm reduces the time taken to compute minimax
	by excluding the possible scores that are too large or too small. It will
	reduce the run time of the minimax such that the AI can look farther and 
	therefore choose better moves within the time-limited tournament.
Genetic Evolution (to tune the heuristic):
	At a high level, our program works by first creating a set of players
	with randomized heuristic values. Then, it plays these players against
	each other over many generations, and at each generation, it creates
	the next generation by doing a weighted-random selection of heuristic
	values from the current generation.
	Our genetic algorithm has the following properties:
	It is elitist: 
		It always keeps the best player from the previous generation, to ensure
		that no optimal solutions are accidentally thrown away due to 
		randomness.
	It implements genetic recombination:
		Based on the number of wins that a player has, it is assigned a 
		probability that its "genes", aka its heuristic parameters, will 
		be passed on to the next generation. 
		Our program will either take a heuristic from one of the parents
		and pass it directly to the child, or will mix (via averaging)
		two parents' "genes" and then pass it to the child.
		This is done for each parameter of the heuristic. In our case,
		we had one value for each tile of the othello board.
	It implements mutations:
		There is a small probability that instead of inheriting from one
		of the parent generation, a given heuristic parameter could be 
		randomly generated instead.
		Also, whenever new heuristic parameters are generated, a small
		variance is added in (aka +- 2% or so) with a high probability, 
		to simulate small mutations within an organism.
Hash Tables (an attempt to make an opening book by storing the best moves) 
	This makes a map of each board state to a score associated with the board state 
	and the best move for that board state such that it can be stored and 
	accessed at a later time, so computation of the best move to make next does 
	not take as much time, especially in the opening stages. In short, it keeps
	a record of what the best move for a given board is, allowing the opening of 
	the game to progress quickly as the AI could very quickly choose the best move
	to play based on the opening book's best-move "cheat sheet" (that would 
	be calculated ahead of time).
	NOTE: this was not fully implemented