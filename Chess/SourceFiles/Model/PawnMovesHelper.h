#pragma once

#include "Figures/BaseFigure.h"

class Pawn;

class PawnMovesHelper
{
public:
	PawnMovesHelper();

public:
	void showChooseNewFigureWidget(Pawn* pawn, const Position newPosition);
	bool isNeedToChangePawn();
	const short getENPassantDelta(const Position& currentPosition);

private:
	bool isENPassant(const Position& currentPosition, short delta);

private:
	virtual BaseFigure* (&getChessBoard())[cagesCount][cagesCount] = 0;
	virtual bool isLastPawnLongMove(const Position& position) = 0;
	virtual Move changePawn(FigureType figureType) = 0;

protected:
	Pawn* replacedPawn;
	Position newFigurePosition;
};

