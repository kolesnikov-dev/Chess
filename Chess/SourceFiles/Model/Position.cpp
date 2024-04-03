#include "Position.h"

Position Position::operator+(const Position& other)
{
	return Position(Row + other.Row, Column + other.Column);
}

Position Position::operator-(const Position& other)
{
	return Position(Row - other.Row, Column - other.Column);
}

bool Position::isValidPosition(const short minPosition, const short maxPosition)
{
	return Row <= maxPosition && Row >= minPosition && Column <= maxPosition && Column >= minPosition;
}

bool operator==(const Position& position1, const Position& position2)
{
	return position1.Row == position2.Row && position1.Column == position2.Column;
}

bool operator!=(const Position& position1, const Position& position2)
{
	return !operator==(position1, position2);
}