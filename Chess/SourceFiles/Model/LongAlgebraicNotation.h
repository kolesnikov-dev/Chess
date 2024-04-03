#pragma once

#include "Figures/BaseFigure.h"

#include <QTextTable>

#include <vector>
#include <string>
#include <map>
#include <queue>

class LongAlgebraicNotation
{
public:
	LongAlgebraicNotation(std::string playerColorStr, int baseLine);

public:
	bool writeMovesInNotations(const QTextTable* movesHistoryTable, const std::string& fileName);
	bool writeMovesInNotations(const std::vector<Move>& movesHistory, const std::string& fileName);
	bool readMoves(const std::string& fileName, std::vector<Position>& moves, std::queue<FigureType>& predefinedTypes) const;

	std::string getFigureName(FigureType type);
	std::string getPositionName(Position position) const;
	Position getPositionCoords(const std::string& position) const;
	FigureType getFigureType(std::string figureName) const;
	std::string getNotationsString(const Move& updatesContainer);

	void addFigureName(FigureType type, const std::string& name);

private:
	int getMoveUpdateIdx(const Move& updatesContainer);
	int getDeleteUpdateIdx(const Move& updatesContainer);
	int getCreationUpdateIdx(const Move& updatesContainer);
	void findMoveNotationRegex();

private:
	const char symb = 'A';
	const char usualMoveSeparator = '-';
	const char captureMoveSeparator = 'x';
	const char promotionSeparator = '=';
	const char checkNotation = '+';
	const char mateNotation = '#';
	const std::string shortCastlingNotation = "0-0";
	const std::string longCastlingNotation = "0-0-0";
	const std::string nullMoveNotation = "empty";

	int deleteUpdateIdx, creationUpdateIdx, simpleUpdateIdx;
	std::pair<std::string, short> currentFirstPlayerData;
	std::string moveNotationRegex;
	std::map<FigureType, std::string> figureNames;
};