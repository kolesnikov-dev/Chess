#pragma once

#include "Figures/Figures.h"
#include "Figures/King.h"
#include "Figures/Rook.h"

class Player
{
public:
	Player(std::shared_ptr<BaseFigure> (&ChessBoard)[cagesCount][cagesCount], const Color color);

public:
	void addFigure(BaseFigure* figure);
	void addFigure(const std::shared_ptr<BaseFigure>& figurePtr);
	void removeFigure(const std::shared_ptr<BaseFigure>& figure, Position oldPosition);

	const Position& getKingCurrentPosition() const;
	const std::vector<std::shared_ptr<BaseFigure>>& getPlayersFigures() const;
	Color getColor() const;

public:
	virtual void initializeFigures() = 0;
	virtual bool isNoMovesForLongCastling() = 0;
	virtual bool isNoMovesForShortCastling() = 0;
	virtual int getBaseLine() = 0;

protected:
	King* king;
	std::vector<std::shared_ptr<BaseFigure>> playersFigures;
	std::shared_ptr<BaseFigure>(&ChessBoard)[cagesCount][cagesCount];
	const Color color;
};

class ClassicPlayer : public Player
{
public:
	ClassicPlayer(std::shared_ptr<BaseFigure> (&ChessBoard)[cagesCount][cagesCount], const Color color, const short firstHorizontalLine, CastlingHelper* castlingHelper, PawnMovesHelper* pawnMovesHelper);

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

