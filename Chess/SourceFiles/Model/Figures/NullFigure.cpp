#include "NullFigure.h"

FigureType NullFigure::getFigureType() const
{
	return FigureType::NullFigure;
}

void NullFigure::setMovementsParameters() {}

NullFigure::NullFigure(const Position& currentPosition)
	: BaseFigure(currentPosition, Color()) {};

NullFigure::NullFigure() {}