#include "Bishop.h"

Bishop::Bishop(const Position& currentPosition, const Color color)
	: BaseFigure(currentPosition, color) {}

Bishop::Bishop() {}

FigureType Bishop::getFigureType() const
{
	return FigureType::Bishop;
}

void Bishop::setMovementsParameters()
{
	uint8_t parametrs = MovementParameters::movementWithOneAttack | MovementParameters::recursiveMovement;
	addMovementParameter({ 1, 1 }, parametrs);
	addMovementParameter({ 1, -1 }, parametrs);
	addMovementParameter({ -1, 1 }, parametrs);
	addMovementParameter({ -1, -1 }, parametrs);
}