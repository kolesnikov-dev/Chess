#include "Players.h"

Player::Player(BaseFigure* (&ChessBoard)[cagesCount][cagesCount], const Color color)
		: ChessBoard(ChessBoard), color(color), king(nullptr) {}

void Player::addFigure(BaseFigure* figure)
{
	ChessBoard[figure->getCurrentPosition().Row][figure->getCurrentPosition().Column] = figure;
	if (figure->getFigureType() != FigureType::NullFigure)
	{
		playersFigures.push_back(figure);
	}
}

void Player::removeFigure(BaseFigure* figure, Position oldPosition)
{
	if (figure->getFigureType() != FigureType::NullFigure)
	{
		playersFigures.erase(std::find(playersFigures.begin(), playersFigures.end(), figure));
		ChessBoard[oldPosition.Row][oldPosition.Column] = new NullFigure(figure->getCurrentPosition());
	}
}

const Position& Player::getKingCurrentPosition() const
{
	return king->getCurrentPosition();
}

const std::vector<BaseFigure*>& Player::getPlayersFigures() const
{
	return playersFigures;
}

Color Player::getColor() const
{
	return color;
}

ClassicPlayer::ClassicPlayer(BaseFigure* (&ChessBoard)[cagesCount][cagesCount], const Color color, const short firstHorizontalLine, CastlingHelper* castlingHelper, PawnMovesHelper* pawnMovesHelper) : 
	Player(ChessBoard, color), 
	castlingHelper(castlingHelper), pawnMovesHelper(pawnMovesHelper), firstHorizontalLine(firstHorizontalLine),
    longCastlingRook(nullptr), shortCaslingRook(nullptr) {}

void ClassicPlayer::initializeFigures()
{
	short secondHorizontalLine = firstHorizontalLine + 1 < cagesCount ? firstHorizontalLine + 1 : firstHorizontalLine - 1;
	short kingColumn, queenColumn;
	if ((color == Color::White && firstHorizontalLine == 0) || (color == Color::Black && firstHorizontalLine == 7))
	{
		queenColumn = 4;
		kingColumn = 3;
	}
	else
	{
		queenColumn = 3;
		kingColumn = 4;
	}

	king = new King({ firstHorizontalLine, kingColumn }, color, castlingHelper);
	shortCaslingRook = new Rook({ firstHorizontalLine, king->getShortCastlingRookColumn() }, color, castlingHelper);
	longCastlingRook = new Rook({ firstHorizontalLine, king->getLongCastlingRookColumn() }, color, castlingHelper);
	addFigure(king);
	addFigure(shortCaslingRook);
	addFigure(longCastlingRook);
	addFigure(new Pawn({ secondHorizontalLine, 0 }, color, firstHorizontalLine, pawnMovesHelper));
	addFigure(new Pawn({ secondHorizontalLine, 1 }, color, firstHorizontalLine, pawnMovesHelper));
	addFigure(new Pawn({ secondHorizontalLine, 2 }, color, firstHorizontalLine, pawnMovesHelper));
	addFigure(new Pawn({ secondHorizontalLine, 3 }, color, firstHorizontalLine, pawnMovesHelper));
	addFigure(new Pawn({ secondHorizontalLine, 4 }, color, firstHorizontalLine, pawnMovesHelper));
	addFigure(new Pawn({ secondHorizontalLine, 5 }, color, firstHorizontalLine, pawnMovesHelper));
	addFigure(new Pawn({ secondHorizontalLine, 6 }, color, firstHorizontalLine, pawnMovesHelper));
	addFigure(new Pawn({ secondHorizontalLine, 7 }, color, firstHorizontalLine, pawnMovesHelper));
	addFigure(new Knight({ firstHorizontalLine, 1 }, color));
	addFigure(new Knight({ firstHorizontalLine, 6 }, color));
	addFigure(new Bishop({ firstHorizontalLine, 2 }, color));
	addFigure(new Bishop({ firstHorizontalLine, 5 }, color));
	addFigure(new Queen({ firstHorizontalLine, queenColumn }, color));

	for (auto& figure : playersFigures)
	{
		figure->setMovementsParameters();
	}
}

bool ClassicPlayer::isNoMovesForLongCastling()
{
	return !king->getIsMove() && !longCastlingRook->getIsMove();
}

bool ClassicPlayer::isNoMovesForShortCastling()
{
	return !king->getIsMove() && !shortCaslingRook->getIsMove();
}

int ClassicPlayer::getBaseLine()
{
	return firstHorizontalLine;
}
