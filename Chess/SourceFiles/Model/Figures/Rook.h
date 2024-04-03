#pragma once

#include "BaseFigure.h"
#include "../CastlingHelper.h"

class CastlingHelper;

class Rook : public BaseFigure
{
public:
	Rook(const Position& currentPosition, const Color color, CastlingHelper* castlingHelper);
	Rook();

public:
	bool getIsMove();
	FigureType getFigureType() const override;
	void setMovementsParameters() override;
	void setNewState(const Position& newPosition) override;
	void restorePreviousState(const Position& oldPosition) override;

private:
	struct State
	{
		bool isMove;
	};

private:
	std::vector<std::pair<int, State>> previousStates;
	CastlingHelper* castlingHelper;
	bool isMove;
};

