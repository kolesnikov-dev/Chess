#pragma once

#include "Figures/BaseFigure.h"

class King;
class Player;

class CastlingHelper
{
public:
	CastlingHelper();

public:
	bool isShortCastlingMovement(const King* king);
	bool isLongCastlingMovement(const King* king);

private:
	bool isValidCastling(const BaseFigure* king, bool isNoMoves, const Direction delta);

private:
	virtual BaseFigure* (&getChessBoard())[cagesCount][cagesCount] = 0;
	virtual Player* getPlayer(Color color) const = 0;
	virtual bool isPositionAttacked(const Position& position, Color attackedPositionColor) = 0;
};

