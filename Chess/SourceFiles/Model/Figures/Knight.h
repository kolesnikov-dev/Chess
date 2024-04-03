#pragma once

#include "BaseFigure.h"

class Knight : public BaseFigure
{
public:
	Knight(const Position& currentPosition, const Color color);
	Knight();

public:
	FigureType getFigureType() const override;
	void setMovementsParameters() override;
};

