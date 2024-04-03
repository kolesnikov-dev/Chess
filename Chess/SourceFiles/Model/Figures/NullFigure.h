#pragma once

#include "BaseFigure.h"
#include "../../Constants.h"

class NullFigure : public BaseFigure
{
public:
	NullFigure(const Position& currentPosition);
	NullFigure();

public:
	FigureType getFigureType() const override;
	void setMovementsParameters() override;
};

