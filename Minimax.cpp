#include<iostream>
#include"Minimax.h"

struct MinimaxParams
{
	StrategicBoardGame* current_;
	Turn AITurn_;
	int maxLevel_, moveAccuracy_, chooseAccuracy_, alpha_, beta_, level_;
	MinimaxParams(StrategicBoardGame* current, Turn AITurn = Turn::second, int maxLevel = 3,
		int moveAccuracy = 100, int chooseAccuracy = 100, int alpha = -1000, int beta = 1000, int level = 0);
};

namespace
{
	std::pair<StrategicBoardGame*, int> miniMax(MinimaxParams args)
	{
		// we have to store next moves
		StrategicBoardGame** nextLevel = args.current_->getPossibleMove();
		int nextLevelSize = 0;
		// we will count size of nextLevel moves
		for (nextLevelSize = 0; nextLevel[nextLevelSize] != nullptr; nextLevelSize++);
		int* nextLevelScores = new int[nextLevelSize];
		// best move will store index of best move in nextLevel and worst score just stores the worst score
		int bestMove = 0, worstScore = args.beta_;
		// just because we want nextLevelScores[bestMove] to be min, otherwize it wont calculate true maximum
		// next level will be pruned at 0 (;
		nextLevelScores[0] = args.alpha_;

		// move accuracy will proune some nodes before we see them, looks likes we didnt noticed them
		int chanceToProune = rand() % 100;
		// we wrote the code like this you see blow because we want to if we set moveAcc for e.x. 60
		// and maxDepth 8 , it will proune no node in first level , 5 percent nodes in second level ,
		// 10 percent in third and so on
		if (chanceToProune < 100 - ((100 - args.moveAccuracy_)*(args.level_ + 1) / args.maxLevel_))
		{
			for (int i = 0; i < nextLevelSize; i++)
			{
				// if we are at a leaf , we have to evaluate it
				if (nextLevel[i]->checkEnd() || args.maxLevel_ == args.level_ + 1)
				{
					// evaluate the game
					nextLevelScores[i] = -1 * nextLevel[i]->evaluate(Turn(int(args.AITurn_) * -1));
				}
				// otherwize we run minimax an other time
				else
				{
					MinimaxParams nextLevelArgs(nextLevel[i], Turn(-1 * int(args.AITurn_)), args.maxLevel_,
						args.moveAccuracy_, args.chooseAccuracy_, -1 * args.beta_,
						-1 * nextLevelScores[bestMove], args.level_ + 1);
					std::pair<StrategicBoardGame*, int> nextLevelMiniMax = miniMax(nextLevelArgs);
					nextLevelScores[i] = -1 * nextLevelMiniMax.second;

					if (nextLevelMiniMax.first != nextLevel[i])
					{
						delete nextLevelMiniMax.first;
						nextLevelMiniMax.first = nullptr;
					}
				}

				// determines best score
				if (nextLevelScores[i] > nextLevelScores[bestMove])
				{
					bestMove = i;
				}
				// determines worst score
				if (nextLevelScores[i] < worstScore)
				{
					worstScore = nextLevelScores[i];
				}
				// proune nodes if they are out of window
				if (nextLevelScores[i] > args.beta_)
				{
					break;
				}
			}

			if (args.chooseAccuracy_ != 100 && args.level_ == 0)
			{
				// we chose an score acording to best score and scores window and chooseAcc
				float chosenScore = ((nextLevelScores[bestMove] - worstScore) * args.chooseAccuracy_) / 100.0;
				// delta to fined best match to chosen score
				float delta = abs((nextLevelScores[bestMove] - worstScore) - chosenScore);
				for (int i = 0; i < nextLevelSize; i++)
				{
					float tempDelta = abs((nextLevelScores[i] - worstScore) - chosenScore);
					if (delta > tempDelta)
					{
						bestMove = i;
						delta = tempDelta;
					}
				}
			}
		}

		else
		{
			// if we didnt see a node , we set its score to hiest possible score so in upper node it will
			// be lowest possible score and it wont be chosen
			nextLevelScores[bestMove] = args.beta_;
		}
		//hashes[level].push_back(pair<int, int>(gameHash, bestScore));
		StrategicBoardGame* bestGame = nextLevel[bestMove];
		for (int i = 0; i < nextLevelSize; i++)
		{
			if (i != bestMove)
			{
				delete nextLevel[i];
				nextLevel[i] = nullptr;
			}
		}
		int bestScore = nextLevelScores[bestMove];
		delete[] nextLevelScores;
		return std::pair<StrategicBoardGame*, int>(bestGame, bestScore);
	}
}

MinimaxParams::MinimaxParams(StrategicBoardGame * current, Turn AITurn, int maxLevel, int moveAccuracy, int chooseAccuracy, int alpha, int beta, int level)
{
	current_ = current;
	AITurn_ = AITurn;
	maxLevel_ = maxLevel;
	moveAccuracy_ = moveAccuracy;
	chooseAccuracy_ = chooseAccuracy;
	alpha_ = alpha;
	beta_ = beta;
	level_ = level;
}

StrategicBoardGame* runMinimax(const Turn &turn,StrategicBoardGame* game,const int &intelligence,
	const int &moveAccuracy, const int &chooseAccuracy, int &scot, const bool &negaMax)
{
	std::pair<StrategicBoardGame*, int> minimax;// to store outcome of the minimax function
	if (negaMax == true)
	{
		// so we will first run the minimax whit tight window
		minimax = miniMax(MinimaxParams(game, turn, intelligence, moveAccuracy,
			chooseAccuracy, scot - 1000, scot + 1000));

		if (minimax.second > scot + 1000 || minimax.second < scot - 1000)
		{
			// outcome is out of our tight window. so negascot have failed
			delete minimax.first;
			// we have to run minimax whit a wide window
			minimax = miniMax(MinimaxParams(game, turn, intelligence, moveAccuracy, chooseAccuracy));
		}
		scot = minimax.second;// set scot for next time
	}
	else {
		// normal minimax
		minimax = miniMax(MinimaxParams(game, turn, intelligence, moveAccuracy, chooseAccuracy));
	}

	if (minimax.second <= -1000 ||  minimax.second >= 1000)
	{
		// if we will lose whatever we do, so let fight. try not to lose in next put
		delete minimax.first;
		minimax = miniMax(MinimaxParams(game, turn, 2));
	}
	return minimax.first;
}
