#include "LongAlgebraicNotation.h"

#include <fstream>
#include <regex>

LongAlgebraicNotation::LongAlgebraicNotation(std::string playerColorStr, int baseLine) : currentFirstPlayerData(std::make_pair(playerColorStr, baseLine)),
deleteUpdateIdx(-1), creationUpdateIdx(-1), simpleUpdateIdx(-1)
{
	addFigureName(FigureType::Bishop, "B");
	addFigureName(FigureType::Pawn, "P");
	addFigureName(FigureType::Queen, "Q");
	addFigureName(FigureType::Rook, "R");
	addFigureName(FigureType::Knight, "K");
	addFigureName(FigureType::King, "Kg");

	findMoveNotationRegex();
}

void LongAlgebraicNotation::findMoveNotationRegex()
{
	moveNotationRegex.clear();

	moveNotationRegex = "^\\s*";
	moveNotationRegex += '(';
	moveNotationRegex += longCastlingNotation + ")|";
	moveNotationRegex += '(';
	moveNotationRegex += shortCastlingNotation + ")|";
	moveNotationRegex += '(';
	moveNotationRegex += nullMoveNotation + ")|";
	moveNotationRegex += '(';
	for (const auto& name : figureNames)
	{
		moveNotationRegex += name.second;
		moveNotationRegex += '|';
	}
	moveNotationRegex.pop_back();
	moveNotationRegex += ')';

	short offset = cagesCount - 1 - currentFirstPlayerData.second;
	auto maxCoord = getPositionName(Position{ offset, static_cast<short>(cagesCount - 1 - offset) });
	auto minCoord = getPositionName(Position{ static_cast<short>(cagesCount - 1 - offset), offset });
	std::string range1 = "(";
	range1 += '[' + std::string(1, minCoord[0]) + '-' + std::string(1, maxCoord[0]) + ']';
	range1 += ")";
	std::string range2 = "(";
	range2 += '[' + std::string(1, minCoord[1]) + '-' + std::string(1, maxCoord[1]) + ']';
	range2 += ")";
	moveNotationRegex += range1 + range2;
	moveNotationRegex += '(' + std::string(1, usualMoveSeparator) + '|' + std::string(1, captureMoveSeparator) + ')';
	moveNotationRegex += range1 + range2;

	moveNotationRegex += "(=(";
	for (auto i = 0; i < figuresToChooseCount; ++i)
	{
		moveNotationRegex += figureNames[chooseFiguresTypes[i]];
		moveNotationRegex += '|';
	}
	moveNotationRegex.pop_back();

	moveNotationRegex += "))?";
	moveNotationRegex += "(";
	moveNotationRegex += mateNotation;
	moveNotationRegex += "|\\";
	moveNotationRegex += checkNotation;
	moveNotationRegex += ")?\\s*$";
}

int LongAlgebraicNotation::getCreationUpdateIdx(const Move& updatesContainer)
{
	auto cancelingUpdates = updatesContainer.getCancelingUpdates();
	for (auto i = creationUpdateIdx + 1; i < cancelingUpdates.size(); ++i)
	{
		if (cancelingUpdates[i].position == deletePosition)
		{
			creationUpdateIdx = i;
			return cancelingUpdates.size() - 1 - i;
		}
	}
	return creationUpdateIdx = -1;
}

int LongAlgebraicNotation::getDeleteUpdateIdx(const Move& updatesContainer)
{
	auto updates = updatesContainer.getUpdates();
	for (auto i = deleteUpdateIdx + 1; i < updates.size(); ++i)
	{
		if (updates[i].position == deletePosition)
		{
			return deleteUpdateIdx = i;
		}
	}
	return deleteUpdateIdx = -1;
}

int LongAlgebraicNotation::getMoveUpdateIdx(const Move& updatesContainer)
{
	auto cancelingUpdates = updatesContainer.getCancelingUpdates();
	for (auto i = simpleUpdateIdx + 1; i < updatesContainer.getUpdates().size(); ++i)
	{
		if (updatesContainer.getUpdates()[i].position != deletePosition && cancelingUpdates[cancelingUpdates.size() - 1 - i].position != deletePosition)
		{
			return simpleUpdateIdx = i;
		}
	}
	return simpleUpdateIdx = -1;
}

void LongAlgebraicNotation::addFigureName(FigureType type, const std::string& name)
{
	figureNames.insert(std::make_pair(type, name));
}

std::string LongAlgebraicNotation::getFigureName(FigureType type)
{
	return figureNames[type];
}

FigureType LongAlgebraicNotation::getFigureType(std::string figureName) const
{
	for (const auto& element : figureNames)
	{
		if (element.second == figureName)
		{
			return element.first;
		}
	}
	return FigureType::NullFigure;
}

std::string LongAlgebraicNotation::getPositionName(Position position) const
{
	short offset = cagesCount - 1 - currentFirstPlayerData.second;
	return static_cast<char>(symb + abs(offset - position.Column)) + std::to_string(cagesCount - abs(offset - position.Row));
}

Position LongAlgebraicNotation::getPositionCoords(const std::string& position) const
{
	short offset = currentFirstPlayerData.second + 1;
	return position.size() >= 2 ? Position(abs(offset - std::stoi(position.c_str() + 1)), static_cast<int>(abs(cagesCount - offset - (position[0] - symb)))) : deletePosition;
}

std::string LongAlgebraicNotation::getNotationsString(const Move& updatesContainer)
{
	creationUpdateIdx = -1;
	deleteUpdateIdx = -1;
	simpleUpdateIdx = -1;

	auto updates = updatesContainer.getUpdates();
	auto cancelingUpdates = updatesContainer.getCancelingUpdates();
	auto MoveType = updatesContainer.getMoveType();
	std::string notation;

	if (MoveType.test(MoveType::SimpleMove))
	{
		auto simpleMoveFigureIdx = getMoveUpdateIdx(updatesContainer);
		auto figureName = getFigureName(updates[simpleMoveFigureIdx].figure->getFigureType());
		notation = figureName + getPositionName(cancelingUpdates[cancelingUpdates.size() - 1 - simpleMoveFigureIdx].position) + usualMoveSeparator + getPositionName(updates[simpleMoveFigureIdx].position);
	}
	else if (MoveType.test(MoveType::Capture) || MoveType.test(MoveType::ENPassant))
	{
		auto simpleMoveFigureIdx = getMoveUpdateIdx(updatesContainer);
		auto figureName = getFigureName(updates[simpleMoveFigureIdx].figure->getFigureType());
		notation = figureName + getPositionName(cancelingUpdates[cancelingUpdates.size() - 1 - simpleMoveFigureIdx].position) + captureMoveSeparator + getPositionName(updates[simpleMoveFigureIdx].position);
	}
	else if (MoveType.test(MoveType::ShortCastling))
	{
		notation = shortCastlingNotation;
	}
	else if (MoveType.test(MoveType::LongCastling))
	{
		notation = longCastlingNotation;
	}

	if (MoveType.test(MoveType::Promotion))
	{
		notation += promotionSeparator + getFigureName(updates[getCreationUpdateIdx(updatesContainer)].figure->getFigureType());
	}

	auto ChessboardState = updatesContainer.getChessboardState();
	if (ChessboardState.test(ChessboardState::Mate))
	{
		notation += mateNotation;
	}
	else if (ChessboardState.test(ChessboardState::Check))
	{
		notation += checkNotation;
	}
	return notation;
}

bool LongAlgebraicNotation::writeMovesInNotations(const QTextTable* movesHistoryTable, const std::string& fileName)
{
	if (fileName == std::string())
		return false;

	std::ofstream outNotationFile(fileName, std::ios::out);
	if (!outNotationFile.is_open())
	{
		return false;
	}

	for (int row = 1; row < movesHistoryTable->rows(); ++row) {
		for (int col = 1; col < movesHistoryTable->columns(); ++col) { 
			QTextTableCell cell = movesHistoryTable->cellAt(row, col);
			std::string notation = cell.firstCursorPosition().block().text().toStdString();
			outNotationFile << notation << std::endl;
		}
	}
	outNotationFile.close();
	return true;
}

bool LongAlgebraicNotation::writeMovesInNotations(const std::vector<Move>& movesHistory, const std::string& fileName)
{
	if (fileName == std::string())
		return false;

	std::ofstream outNotationFile(fileName, std::ios::out);
	if (!outNotationFile.is_open())
	{
		return false;
	}

	for (const auto& updatesContainer : movesHistory)
	{
		auto notation = getNotationsString(updatesContainer);
		outNotationFile << notation << std::endl;
	}
	outNotationFile.close();
	return true;
}

bool LongAlgebraicNotation::readMoves(const std::string& fileName, std::vector<Position>& moves, std::queue<FigureType>& predefinedTypes) const
{
	if (fileName == std::string())
		return false;

	std::string fileLine;
	std::smatch match;
	std::regex moveRegex(moveNotationRegex);
	std::ifstream inNotationFile(fileName);
	bool isFirstPLayerMove = true;

	if (!inNotationFile.is_open())
	{
		return false;
	}

	while (std::getline(inNotationFile, fileLine))
	{
		if (std::regex_match(fileLine, match, moveRegex) && match.size() >= 7)
		{
			if (match[1].str() == longCastlingNotation || match[2].str() == shortCastlingNotation)
			{
				std::string castlingCoordX = (match[1].str() == longCastlingNotation ? "C" : "G");

				if (isFirstPLayerMove)
				{
					moves.push_back({ getPositionCoords("E1")});
					moves.push_back({ getPositionCoords(castlingCoordX + "1") });
				}
				else
				{
					moves.push_back({ getPositionCoords("E8") });
					moves.push_back({ getPositionCoords(castlingCoordX + "8") });
				}
			}
			else
			{
				auto startPosition = match[5].str() + match[6].str();
				auto newPosition = match[8].str() + match[9].str();
				auto startCoords = getPositionCoords(startPosition);
				auto newCoords = getPositionCoords(newPosition);
				moves.push_back(startCoords);
				moves.push_back(newCoords);
				if (match[10].matched)
				{
					predefinedTypes.push(getFigureType(match[11].str()));
				}
			}
		}
		isFirstPLayerMove = !isFirstPLayerMove;
	}
	return true;
}
