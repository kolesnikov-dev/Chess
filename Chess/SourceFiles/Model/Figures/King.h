#pragma once

#include "BaseFigure.h"
#include "../CastlingHelper.h"

class CastlingHelper;

class King : public BaseFigure
{
public:
	King(const Position& currentPosition, const Color color, CastlingHelper* castlingHelper);

public:
	short getShortCastlingRookColumn() const;
	short getLongCastlingRookColumn() const;
	Direction getLongCastlingDirection() const;
	Direction getShortCastlingDirection() const;
	bool getIsMove() const;

	FigureType getFigureType() const override;
	void setMovementsParameters() override;
	void setNewState(const Position& newPosition) override;
	void restorePreviousState(const Position& oldPosition) override;

	Move findChessBoardUpdates(BaseFigure* const (&ChessBoard)[cagesCount][cagesCount], const Position& newPosition, bool ignoreSafeMoves = false) override;
	std::vector<Position> findPossibleMovementsPositions(BaseFigure* const (&ChessBoard)[cagesCount][cagesCount], bool onlyAttackMovements = false) override;

private:
	struct State
	{
		bool isMove;
	};

private:
	std::vector<std::pair<int, State>> previousStates;
	CastlingHelper* castlingHelper;
	const Position startPosition;
	short shortCastlingRookColumn, longCastlingRookColumn;
	Direction shortCastlingDirection, longCastlingDirection;
	bool isMove;
};

