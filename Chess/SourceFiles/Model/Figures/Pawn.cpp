#include "Pawn.h"

Pawn::Pawn(const Position& currentPosition, const Color color, const short firstHorizontalLine, PawnMovesHelper* pawnMovesHelper)
	: BaseFigure(currentPosition, color), firstHorizontalLine(firstHorizontalLine), pawnMovesHelper(pawnMovesHelper), isMove(false), isLongMove(false)
{
	initializeMoveDirection();
}

void Pawn::initializeMoveDirection()
{
	if (firstHorizontalLine == 0)
	{
		moveDirection = 1;
	}
	else if (firstHorizontalLine == cagesCount - 1)
	{
		moveDirection = -1;
	}
}

FigureType Pawn::getFigureType() const
{
	return FigureType::Pawn;
}

void Pawn::setNewState(const Position& newPosition)
{
	if (!isMove)
	{
		previousStates.push_back(std::make_pair(moveId, State{ isLongMove, isMove }));
		isMove = true;
		if (abs(currentPosition.Row - newPosition.Row) == 2)
		{
			isLongMove = true;
		}
	}
	BaseFigure::setNewState(newPosition);
}

Move Pawn::findChessBoardUpdates(std::shared_ptr<BaseFigure> const (&ChessBoard)[cagesCount][cagesCount], const Position& newPosition, bool ignoreSafeMoves)
{
	auto updates = BaseFigure::findChessBoardUpdates(ChessBoard, newPosition);
	auto delta = pawnMovesHelper->getENPassantDelta(currentPosition);

	if (!ignoreSafeMoves && newPosition.Row == cagesCount - 1 - firstHorizontalLine)
	{
		pawnMovesHelper->setReplacedPawn(ChessBoard[currentPosition.Row][currentPosition.Column], newPosition);
		updates.setMoveType(MoveType::Promotion, false);
	}
	else if (delta != 0 && currentPosition.Column + delta == newPosition.Column && abs(currentPosition.Row - newPosition.Row) == 1)
	{
		updates.addUpdate(deletePosition, ChessBoard[currentPosition.Row][currentPosition.Column + delta]);
		updates.setMoveType(MoveType::ENPassant);
	}
	return updates;
}

std::vector<Position> Pawn::findPossibleMovementsPositions(std::shared_ptr<BaseFigure> const (&ChessBoard)[cagesCount][cagesCount], bool onlyAttackMovements)
{
	short delta = 0;
	auto movementPositions = BaseFigure::findPossibleMovementsPositions(ChessBoard);

	if (!onlyAttackMovements && !isMove && ChessBoard[currentPosition.Row + 1 * moveDirection][currentPosition.Column]->getFigureType() == FigureType::NullFigure && ChessBoard[currentPosition.Row + 2 * moveDirection][currentPosition.Column]->getFigureType() == FigureType::NullFigure)
	{
		movementPositions.push_back({ static_cast<short>(currentPosition.Row + 2 * moveDirection), currentPosition.Column });
	}
	else if (delta = pawnMovesHelper->getENPassantDelta(currentPosition))
	{
		movementPositions.push_back({ static_cast<short>(currentPosition.Row + 1 * moveDirection), static_cast<short>(currentPosition.Column + delta) });
	}
	return movementPositions;
}

void Pawn::setMovementsParameters()
{
	addMovementParameter({ moveDirection, 0 }, MovementParameters::noAttackMovement);
	addMovementParameter({ moveDirection, 1 }, MovementParameters::onlyAttackMovements);
	addMovementParameter({ moveDirection, -1 }, MovementParameters::onlyAttackMovements);
}

void Pawn::restorePreviousState(const Position& oldPosition)
{
	--moveId;
	if (!previousStates.empty() && previousStates.back().first == moveId)
	{
		isLongMove = previousStates.back().second.isLongMove;
		isMove = previousStates.back().second.isMove;
		previousStates.pop_back();
	}
	BaseFigure::restorePreviousState(oldPosition);
}