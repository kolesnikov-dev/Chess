#pragma once

#include "Figures/Figures.h"
#include "Figures/King.h"
#include "Figures/Rook.h"

//class King;
//class Rook;

class Player
{
public:
	Player(BaseFigure* (&ChessBoard)[cagesCount][cagesCount], const Color color);

public:
	void addFigure(BaseFigure* figure);
	void removeFigure(BaseFigure* figure, Position oldPosition);

	const Position& getKingCurrentPosition() const;
	const std::vector<BaseFigure*>& getPlayersFigures() const;
	Color getColor() const;

public:
	virtual void initializeFigures() = 0;
	virtual bool isNoMovesForLongCastling() = 0;
	virtual bool isNoMovesForShortCastling() = 0;
	virtual int getBaseLine() = 0;

protected:
	King* king;
	std::vector<BaseFigure*> playersFigures;
	BaseFigure* (&ChessBoard)[cagesCount][cagesCount];
	const Color color;
};

class ClassicPlayer : public Player
{
public:
	ClassicPlayer(BaseFigure* (&ChessBoard)[cagesCount][cagesCount], const Color color, const short firstHorizontalLine, CastlingHelper* castlingHelper, PawnMovesHelper* pawnMovesHelper);

public:
	void initializeFigures() override;
	bool isNoMovesForLongCastling() override;
	bool isNoMovesForShortCastling() override;
    int getBaseLine() override;

private:
	const short firstHorizontalLine;
	CastlingHelper* castlingHelper;
	PawnMovesHelper* pawnMovesHelper;
	Rook* shortCaslingRook, *longCastlingRook;
};

