#pragma once

#include "BaseFigure.h"

class Bishop : public BaseFigure
{
public:
	Bishop(const Position& currentPosition, const Color color);
	Bishop();

public:
	FigureType getFigureType() const override;

	void setMovementsParameters() override;
};

