#pragma once

#include "BaseFigure.h"

class Queen : public BaseFigure
{
public:
	Queen(const Position& currentPosition, const Color color);
	Queen();

public:
	FigureType getFigureType() const override;
	void setMovementsParameters() override;
};

