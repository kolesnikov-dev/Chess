#include "Knight.h"

Knight::Knight(const Position& currentPosition, const Color color)
	: BaseFigure(currentPosition, color) {}

Knight::Knight() {}

FigureType Knight::getFigureType() const
{
	return FigureType::Knight;
}

void Knight::setMovementsParameters()
{
	uint8_t parametrs = MovementParameters::movementWithOneAttack;
	addMovementParameter({ 2, -1 }, parametrs);
	addMovementParameter({ 2, 1 }, parametrs);
	addMovementParameter({ -2, -1 }, parametrs);
	addMovementParameter({ -2, 1 }, parametrs);
	addMovementParameter({ -1, 2 }, parametrs);
	addMovementParameter({ 1, 2 }, parametrs);
	addMovementParameter({ -1, -2 }, parametrs);
	addMovementParameter({ 1, -2 }, parametrs);
}
