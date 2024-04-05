#pragma once

#include "BaseFigure.h"
#include "NullFigure.h"
#include "../PawnMovesHelper.h"

class Pawn : public BaseFigure
{
	friend class pawnMovesHelper;

public:
	Pawn(const Position& currentPosition, const Color color, const short firstHorizontalLine, PawnMovesHelper* pawnMovesHelper);

public:
	void initializeMoveDirection();

	FigureType getFigureType() const override;
	void setMovementsParameters() override;
	void setNewState(const Position& newPosition) override;
	void restorePreviousState(const Position& oldPosition) override;

	Move findChessBoardUpdates(std::shared_ptr<BaseFigure> const (&ChessBoard)[cagesCount][cagesCount], const Position& newPosition, bool ignoreSafeMoves = false) override;
	std::vector<Position> findPossibleMovementsPositions(std::shared_ptr<BaseFigure> const (&ChessBoard)[cagesCount][cagesCount], bool onlyAttackMovements = false) override;

private:
	struct State
	{
		bool isLongMove;
		bool isMove;
	};

private:
	std::vector<std::pair<int, State>> previousStates;
	PawnMovesHelper* pawnMovesHelper;
	bool isMove;
	bool isLongMove;
	short firstHorizontalLine;
	short moveDirection;
};

