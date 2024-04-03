#include "PawnMovesHelper.h"
#include "Figures/Pawn.h"

PawnMovesHelper::PawnMovesHelper() : replacedPawn(nullptr) {}

void PawnMovesHelper::showChooseNewFigureWidget(Pawn* pawn, const Position newPosition)
{
	replacedPawn = pawn;
	newFigurePosition = newPosition;
}

bool PawnMovesHelper::isNeedToChangePawn()
{
	return replacedPawn == nullptr ? false : true;
}

const short PawnMovesHelper::getENPassantDelta(const Position& currentPosition)
{
	const short rightENPassantDelta = 1;
	const short leftENPassantDelta = -1;
	if (isENPassant(currentPosition, rightENPassantDelta))
	{
		return rightENPassantDelta;
	}
	else if (isENPassant(currentPosition, leftENPassantDelta))
	{
		return leftENPassantDelta;
	}
	return 0;
}

bool PawnMovesHelper::isENPassant(const Position& currentPosition, short delta)
{
	return (currentPosition.Column + delta >= 0 && currentPosition.Column + delta < cagesCount && isLastPawnLongMove({ currentPosition.Row, static_cast<short>(currentPosition.Column + delta) }));
}