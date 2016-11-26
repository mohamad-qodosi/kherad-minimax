#pragma once

enum class Turn : std::int8_t
{
	first = 1,
	second = -1
};

//your game must inherit this game class
class StrategicBoardGame
{
protected:
	int hash_;
	Turn turn_;
public:
	virtual int checkEnd() const = 0;
	virtual bool move(int argc, int* argv) = 0;
	virtual StrategicBoardGame** getPossibleMove() = 0;
	virtual void getGameBoard(int* destination) const = 0;
	virtual int getCell(int x, int y) const = 0;
	virtual int evaluate(Turn turn) const = 0; // -1000<...<1000
	virtual void reset() = 0;
	int getGameHash() const { return hash_; }
	Turn getTurn() const { return turn_; }
};

StrategicBoardGame* runMinimax(const Turn &turn, StrategicBoardGame* game, const int &intelligence,
	const int &moveAccuracy, const int &chooseAccuracy, int &scot, const bool &negaMax);

template<typename baseGame> 
class AiPlayer
{
public:
	AiPlayer(Turn AiTurn, baseGame* game, short int intelligence = 100,
		short int moveAccuracy = 100, short int chooseAccuracy = 100, bool negaMax = true);
	void play();

private:
	Turn turn_;
	baseGame* game_;
	int intelligence_;//levels that go throw.
	int moveAccuracy_;	//chance to make best performance. max = 100
	int chooseAccuracy_;
	int scot_;
	bool negaMax_;
};

template<typename baseGame>
AiPlayer<typename baseGame>::AiPlayer(Turn AiTurn, baseGame* game, short int intelligence = 100,
	short int moveAccuracy = 100, short int chooseAccuracy = 100, bool negaMax = true)
{
	turn_ = AiTurn;
	game_ = game;
	intelligence_ = intelligence;
	moveAccuracy_ = moveAccuracy;
	chooseAccuracy_ = chooseAccuracy;
	negaMax_ = negaMax;
	scot_ = 0;
}

template<typename baseGame>
void AiPlayer<typename baseGame>::play()
{
	StrategicBoardGame* chosenGame = runMinimax(turn_, game_, intelligence_, moveAccuracy_, chooseAccuracy_,
		scot_, negaMax_);

	// let do the move now.
	*game_ = *(baseGame*)chosenGame;
	delete chosenGame;
}
