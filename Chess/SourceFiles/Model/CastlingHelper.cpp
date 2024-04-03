#include "CastlingHelper.h"
#include "Players.h"

CastlingHelper::CastlingHelper() {}

bool CastlingHelper::isShortCastlingMovement(const King* king)
{
	return isValidCastling(king, getPlayer(king->getColor())->isNoMovesForShortCastling(), king->getShortCastlingDirection());
}

bool CastlingHelper::isLongCastlingMovement(const King* king)
{
	return isValidCastling(king, getPlayer(king->getColor())->isNoMovesForLongCastling(), king->getLongCastlingDirection());
}

bool CastlingHelper::isValidCastling(const BaseFigure* king, bool isNoMoves, const Direction direction)
{
	auto& kingPosition = king->getCurrentPosition();
	if (isNoMoves)
	{
		std::vector<Position> positionsToCheck;
		for (short i = kingPosition.Column + 1 * direction; i > 0 && i < cagesCount - 1; i += 1 * direction)
		{
			if (getChessBoard()[kingPosition.Row][i]->getFigureType() != FigureType::NullFigure)
			{
				return false;
			}
			positionsToCheck.push_back({ kingPosition.Row, i });
		}
		positionsToCheck.push_back({ kingPosition.Row, kingPosition.Column });

		for (const auto& position : positionsToCheck)
		{
			if (isPositionAttacked(position, king->getColor()))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}