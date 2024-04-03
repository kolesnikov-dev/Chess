#include "Queen.h"

Queen::Queen(const Position& currentPosition, const Color color)
		: BaseFigure(currentPosition, color) {}

Queen::Queen() {}

FigureType Queen::getFigureType() const
{
	return FigureType::Queen;
}

void Queen::setMovementsParameters()
{
	uint8_t parametrs = MovementParameters::movementWithOneAttack | MovementParameters::recursiveMovement;
	addMovementParameter({ -1, -1 }, parametrs);
	addMovementParameter({ -1, 0 }, parametrs);
	addMovementParameter({ -1, 1 }, parametrs);
	addMovementParameter({ 0, -1 }, parametrs);
	addMovementParameter({ 0, 1 }, parametrs);
	addMovementParameter({ 1, -1 }, parametrs);
	addMovementParameter({ 1, 0 }, parametrs);
	addMovementParameter({ 1, 1 }, parametrs);
}