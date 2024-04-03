#include "Rook.h"

Rook::Rook(const Position& currentPosition, const Color color, CastlingHelper* castlingHelper)
	: BaseFigure(currentPosition, color), castlingHelper(castlingHelper), isMove(false) {}

Rook::Rook() : castlingHelper(nullptr), isMove(false) {}

void Rook::setNewState(const Position& newPosition)
{
	if (!isMove)
	{
		previousStates.push_back(std::make_pair(moveId, State{ isMove }));
		isMove = true;
	}
	BaseFigure::setNewState(newPosition);
}

bool Rook::getIsMove()
{
	return isMove;
}

FigureType Rook::getFigureType() const
{
	return FigureType::Rook;
}

void Rook::setMovementsParameters()
{
	uint8_t parametrs = MovementParameters::movementWithOneAttack | MovementParameters::recursiveMovement;
	addMovementParameter({ 1, 0 }, parametrs);
	addMovementParameter({ -1, 0 }, parametrs);
	addMovementParameter({ 0, 1 }, parametrs);
	addMovementParameter({ 0, -1 }, parametrs);
}

void Rook::restorePreviousState(const Position& oldPosition)
{
	--moveId;
	if (!previousStates.empty() && previousStates.back().first == moveId)
	{
		isMove = previousStates.back().second.isMove;
		previousStates.pop_back();
	}
	BaseFigure::restorePreviousState(oldPosition);
}