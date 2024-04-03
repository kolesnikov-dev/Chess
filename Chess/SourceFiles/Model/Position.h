#pragma once

struct Position
{
	constexpr Position(short Row, short Column) : Row(Row), Column(Column) {}
	constexpr Position() : Row(0), Column(0) {}

	Position operator+(const Position& other);
	Position operator-(const Position& other);
	friend bool operator==(const Position& position1, const Position& position2);
	friend bool operator!=(const Position& position1, const Position& position2);

	bool isValidPosition(const short minPosition, const short maxPosition);

	short Row;
	short Column;
};

