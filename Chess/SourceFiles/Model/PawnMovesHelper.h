#pragma once

#include "Figures/BaseFigure.h"

class Pawn;

class PawnMovesHelper
{
public:
	PawnMovesHelper();

public:
	void setReplacedPawn(std::shared_ptr<BaseFigure> pawn, const Position newPosition);
	bool isNeedToChangePawn();
	const short getENPassantDelta(const Position& currentPosition);

private:
	bool isENPassant(const Position& currentPosition, short delta);

private:
	virtual std::shared_ptr<BaseFigure> (&getChessBoard())[cagesCount][cagesCount] = 0;
	virtual bool isLastPawnLongMove(const Position& position) = 0;
	virtual Move changePawn(FigureType figureType) = 0;

protected:
	std::shared_ptr<BaseFigure> replacedPawn;
	Position newFigurePosition;
};

