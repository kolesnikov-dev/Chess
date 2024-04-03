#include "King.h"

King::King(const Position& currentPosition, const Color color, CastlingHelper* castlingHelper)
	: BaseFigure(currentPosition, color), castlingHelper(castlingHelper), isMove(false), startPosition(currentPosition) 
{
	shortCastlingDirection = Direction::Left;
	longCastlingDirection = Direction::Right;
	if (startPosition.Column == 4)
	{
		std::swap(shortCastlingDirection, longCastlingDirection);
	}
	longCastlingRookColumn = currentPosition.Column + 4 * longCastlingDirection;
	shortCastlingRookColumn = currentPosition.Column + 3 * shortCastlingDirection;
};

short King::getShortCastlingRookColumn() const
{
	return shortCastlingRookColumn;
}

short King::getLongCastlingRookColumn() const
{
	return longCastlingRookColumn;
}

Direction King::getLongCastlingDirection() const
{
	return longCastlingDirection;
}

Direction King::getShortCastlingDirection() const
{
	return shortCastlingDirection;
}

FigureType King::getFigureType() const
{
	return FigureType::King;
}

Move King::findChessBoardUpdates(BaseFigure* const (&ChessBoard)[cagesCount][cagesCount], const Position& newPosition, bool ignoreSafeMoves)
{
	auto updates = BaseFigure::findChessBoardUpdates(ChessBoard, newPosition);
	if (!ignoreSafeMoves)
	{
		if (newPosition.Column == currentPosition.Column + 2 * longCastlingDirection)
		{
			auto longCastlingRook = ChessBoard[currentPosition.Row][longCastlingRookColumn];
			auto rookNewPosition = Position{ longCastlingRook->getCurrentPosition().Row, static_cast<short>(longCastlingRook->getCurrentPosition().Column - 3 * longCastlingDirection) };
			updates.addUpdate(rookNewPosition, longCastlingRook);
			updates.setMoveType(MoveType::LongCastling);
		}
		else if (newPosition.Column == currentPosition.Column + 2 * shortCastlingDirection)
		{
			auto shortCastlingRook = ChessBoard[currentPosition.Row][shortCastlingRookColumn];
			auto rookNewPosition = Position{ shortCastlingRook->getCurrentPosition().Row, static_cast<short>(shortCastlingRook->getCurrentPosition().Column - 2 * shortCastlingDirection) };
			updates.addUpdate(rookNewPosition, shortCastlingRook);
			updates.setMoveType(MoveType::ShortCastling);
		}
	}
	return updates;
}

std::vector<Position> King::findPossibleMovementsPositions(BaseFigure* const (&ChessBoard)[cagesCount][cagesCount], bool onlyAttackMovements)
{
	auto movementPositions = BaseFigure::findPossibleMovementsPositions(ChessBoard);
	if (!onlyAttackMovements)
	{
		if (castlingHelper->isLongCastlingMovement(this))
		{
			movementPositions.push_back({ currentPosition.Row, static_cast<short>(currentPosition.Column + 2 * longCastlingDirection) });
		}
		if (castlingHelper->isShortCastlingMovement(this))
		{
			movementPositions.push_back({ currentPosition.Row,static_cast<short>(currentPosition.Column + 2 * shortCastlingDirection) });
		}
	}
	return movementPositions;
}

void King::setMovementsParameters()
{
	uint8_t parametrs = MovementParameters::movementWithOneAttack;
	addMovementParameter({ -1, -1 }, parametrs);
	addMovementParameter({ -1, 0 }, parametrs);
	addMovementParameter({ -1, 1 }, parametrs);
	addMovementParameter({ 0, -1 }, parametrs);
	addMovementParameter({ 0, 1 }, parametrs);
	addMovementParameter({ 1, -1 }, parametrs);
	addMovementParameter({ 1, 0 }, parametrs);
	addMovementParameter({ 1, 1 }, parametrs);
}

bool King::getIsMove() const
{
	return isMove;
}

void King::setNewState(const Position& newPosition)
{
	if (!isMove)
	{
		previousStates.push_back(std::make_pair(moveId, State{ isMove }));
		isMove = true;
	}
	BaseFigure::setNewState(newPosition);
}

void King::restorePreviousState(const Position& oldPosition)
{
	--moveId;
	if (!previousStates.empty() && previousStates.back().first == moveId)
	{
		isMove = previousStates.back().second.isMove;
		previousStates.pop_back();
	}
	BaseFigure::restorePreviousState(oldPosition);
}