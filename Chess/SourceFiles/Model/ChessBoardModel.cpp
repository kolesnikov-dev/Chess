#include "ChessBoardModel.h"

#include <iostream>
#include <iomanip>

ChessBoardModel::ChessBoardModel(Color firstPlayerColor, Color secondPlayerColor) : movementsHistorySize(0), selectedFigure(new NullFigure)
{
	players[firstPlayerColor] = new ClassicPlayer(ChessBoard, firstPlayerColor, 0, dynamic_cast<CastlingHelper*>(this), dynamic_cast<PawnMovesHelper*>(this));
	players[secondPlayerColor] = new ClassicPlayer(ChessBoard, secondPlayerColor, 7, dynamic_cast<CastlingHelper*>(this), dynamic_cast<PawnMovesHelper*>(this));

	for (short i = 0; i < cagesCount; ++i)
	{
		for (short j = 0; j < cagesCount; ++j)
		{
			ChessBoard[i][j] = new NullFigure({ i, j });
		}
	}

	for (auto& player : players)
	{
		player->initializeFigures();
	}
	currentPlayer = players[Color::White];
}

ChessBoardModel::~ChessBoardModel()
{
	for (const auto& player : players)
	{
		delete player;
	}

	for (auto i = 0; i < cagesCount; ++i)
	{
		for (auto j = 0; j < cagesCount; ++j)
		{
			if (ChessBoard[i][j]->getFigureType() != FigureType::NullFigure)
			{
				delete ChessBoard[i][j];
			}
		}
	}
}

bool ChessBoardModel::isFiguresLackToMate(Player* player)
{
	uint8_t playersFiguresMask = 0;
	const auto& playersFigures = player->getPlayersFigures();
	if (playersFigures.size() <= 3)
	{
		for (auto& figure : playersFigures)
		{
			playersFiguresMask |= static_cast<uint8_t>(figure->getFigureType());
		}
		if (playersFigures.size() == 3 && (playersFiguresMask & (static_cast<uint8_t>(FigureType::King) | static_cast<uint8_t>(FigureType::Knight))))
		{
			auto BishopIterator = std::find(playersFigures.begin(), playersFigures.end(), playersFigures[0]);
			if (BishopIterator == playersFigures.end())
			{
				return !isSingleFieldFigures(playersFigures[1], playersFigures[2]);
			}
			else
			{
				return !isSingleFieldFigures(playersFigures[0], *BishopIterator);
			}
		}
		if (playersFigures.size() == 1 && (playersFiguresMask & static_cast<uint8_t>(FigureType::King)) ||
			playersFigures.size() == 2 && (playersFiguresMask & (static_cast<uint8_t>(FigureType::King) | static_cast<uint8_t>(FigureType::Bishop))) ||
			playersFigures.size() == 2 && (playersFiguresMask & (static_cast<uint8_t>(FigureType::King) | static_cast<uint8_t>(FigureType::Knight))))
		{
			return true;
		}
	}
	return false;
}

bool ChessBoardModel::isStalemate(Player* player)
{
	for (auto& figure : player->getPlayersFigures())
	{
		if (!findValidMovements(figure).empty())
		{
			validMovements.clear();
			return false;
		}
	}
	return true;
}

bool ChessBoardModel::isMate(Player* player)
{
	if (isPlayerCheck(player))
	{
		return isStalemate(player);
	}
	return false;
}

BaseFigure* ChessBoardModel::figuresFabric(FigureType figureType)
{
	BaseFigure* figure;
	switch (figureType)
	{
	case FigureType::NullFigure:
		figure = new NullFigure();
		break;
	case FigureType::Rook:
		figure = new Rook();
		break;
	case FigureType::Knight:
		figure = new Knight();
		break;
	case FigureType::Queen:
		figure = new Queen();
		break;
	case FigureType::Bishop:
		figure = new Bishop();
		break;
	default:
		return nullptr;
		break;
	}
	figure->setMovementsParameters();
	return figure;
}

const std::vector<Move>& ChessBoardModel::getMovementHistory() const
{
	return movementsHistory;
}

Move ChessBoardModel::changePawn(FigureType figureType)
{
	Move updates;
	BaseFigure* newFigure = figuresFabric(figureType);
	newFigure->setColor(replacedPawn->getColor());
	updates.addUpdate(deletePosition, replacedPawn);
	updates.addUpdate(newFigurePosition, newFigure);
	movementsHistory[movementsHistorySize - 1].addUpdate(deletePosition, replacedPawn);
	movementsHistory[movementsHistorySize - 1].addUpdate(newFigurePosition, newFigure);
	doChessBoarUpdates(updates.getUpdates());
	setChessboardState(movementsHistory[movementsHistorySize - 1]);
	replacedPawn = nullptr;

	return movementsHistory[movementsHistorySize - 1];
}

Move ChessBoardModel::getCurrentMovement() const
{
	return movementsHistorySize > 0 ? movementsHistory[movementsHistorySize - 1] : Move();
}

Move ChessBoardModel::undoMovement()
{
	if (isPreviousMove())
	{
		--movementsHistorySize;
		auto& updates = movementsHistory[movementsHistorySize];
		doChessBoarUpdates(updates.getCancelingUpdates(), true);
		return updates;
	}
	return Move();
}

Move ChessBoardModel::redoMovement()
{
	if (isNextMove())
	{ 
		auto& updates = movementsHistory[movementsHistorySize];
		doChessBoarUpdates(updates.getUpdates());
		++movementsHistorySize;
		return updates;
	}
	return Move();
}

void ChessBoardModel::doChessBoarUpdates(std::vector<Update> updates, bool isUpdatesReverse)
{
	for (const auto& element : updates)
	{
		auto& newPosition = element.position;
		auto oldPosition = element.figure->getCurrentPosition();
		if (isUpdatesReverse)
		{
			element.figure->restorePreviousState(newPosition);
		}
		else
		{
			element.figure->setNewState(newPosition);
		}

		if (oldPosition == deletePosition)
		{
			auto player = getPlayer(element.figure->getColor());
			player->addFigure(element.figure);
		}
		else if (newPosition == deletePosition)
		{
			auto player = getPlayer(element.figure->getColor());
			player->removeFigure(element.figure, oldPosition);
		}
		else
		{
			ChessBoard[oldPosition.Row][oldPosition.Column] = new NullFigure(oldPosition);
			ChessBoard[newPosition.Row][newPosition.Column] = element.figure;
		}
	}
}

bool ChessBoardModel::isRepeatingPosition(int maxRepeatsCount)
{
	short repeatsCount = 0;
	std::vector<Update> movements;
	struct Update element;
	auto isEquivalent = [&element](const Update& updates)
		{
			return element.figure->getColor() == updates.figure->getColor() &&
				element.figure->getFigureType() == updates.figure->getFigureType() &&
				element.position == updates.position;
		};

	for (auto i = movementsHistorySize - 1; i >= 0; --i)
	{
		element = movementsHistory[i].getUpdates()[0];
		if (isEquivalent(movementsHistory.back().getUpdates()[0]))
		{
			++repeatsCount;
			if ((movementsHistorySize - i) * maxRepeatsCount / repeatsCount > movementsHistorySize)
			{
				return false;
			}

			if ((repeatsCount == maxRepeatsCount) && movements.empty())
			{
				return true;
			}
		}

		auto it = std::find_if(movements.begin(), movements.end(), isEquivalent);
		if (it != movements.end())
		{
			movements.erase(it);
		}
		else
		{
			movements.push_back(movementsHistory[i].getCancelingUpdates()[0]);
		}
	}
	return false;
}

boardStateMask ChessBoardModel::getChessboardState() const
{
	return movementsHistorySize > 0 ? movementsHistory[movementsHistorySize - 1].getChessboardState() : boardStateMask();
}

Move ChessBoardModel::moveSelectedFigure(const Position& position)
{
	Move updates;
	if (std::find(validMovements.begin(), validMovements.end(), position) != validMovements.end())
	{
		updates = selectedFigure->findChessBoardUpdates(ChessBoard, position);
		doChessBoarUpdates(updates.getUpdates());
		setChessboardState(updates);
		if (movementsHistory.size() != movementsHistorySize)
		{
			movementsHistory.erase(movementsHistory.begin() + movementsHistorySize, movementsHistory.end());
		}
		movementsHistory.push_back(updates);
		++movementsHistorySize;
	}
	return updates;
}

void ChessBoardModel::setSelectedFigure(const Position& position)
{
	this->selectedFigure = new NullFigure;
	this->validMovements.clear();
	BaseFigure* selectedFigure = ChessBoard[position.Row][position.Column];
	auto& currentPlayersFigures = currentPlayer->getPlayersFigures();
	if (std::find(currentPlayersFigures.begin(), currentPlayersFigures.end(), selectedFigure) != currentPlayersFigures.end())
	{
		this->validMovements = findValidMovements(selectedFigure);
		if (!validMovements.empty())
		{
			this->selectedFigure = selectedFigure;
		}
	}
}

void ChessBoardModel::changePlayerToMove()
{
	currentPlayer = players[(currentPlayer->getColor() + 1) % playersCount];
}

bool ChessBoardModel::isSelectedFigure() const
{
	return selectedFigure->getFigureType() != FigureType::NullFigure;
}

bool ChessBoardModel::isLastPawnLongMove(const Position& position)
{
	if (movementsHistorySize > 1)
	{
		auto update = movementsHistory[movementsHistorySize - 1].getCancelingUpdates()[0];
		return update.figure->getCurrentPosition() == position && update.figure->getFigureType() == FigureType::Pawn && abs(update.figure->getCurrentPosition().Row - update.position.Row) == 2;
	}
	return false;
}

Player* ChessBoardModel::getCurrentPlayer() const
{
	return currentPlayer;
}

BaseFigure* ChessBoardModel::getSelectedFigure() const
{
	return selectedFigure;
}

const std::vector<Position>& ChessBoardModel::getSelectedFigureValidMovements() const
{
	return validMovements;
}

Player* ChessBoardModel::getPlayer(Color color) const
{
	return players[color];
}

bool ChessBoardModel::isPlayerCheck(const Player* player)
{
	return isPositionAttacked(player->getKingCurrentPosition(), player->getColor());
}

bool ChessBoardModel::isPositionAttacked(const Position& position, Color attackedPositionColor)
{
	auto attackPlayer = getOpponentPlayer(attackedPositionColor);

	for (const auto& opponentFigure : attackPlayer->getPlayersFigures())
	{
		auto attackMovements = opponentFigure->findPossibleMovementsPositions(ChessBoard, true);
		if (std::find(attackMovements.begin(), attackMovements.end(), position) != attackMovements.end())
		{
			return true;
		}
	}
	return false;
}

void ChessBoardModel::setChessboardState(Move& updateContainer)
{
	auto opponentPlayer = getCurrentOpponentPlayer(); 
	if (isPlayerCheck(opponentPlayer))
	{
		updateContainer.setChessboardState(ChessboardState::Check);
	}
	if (isMate(opponentPlayer))
	{
		updateContainer.setChessboardState(ChessboardState::Mate);
	}
	else if (isStalemate(opponentPlayer))
	{
		updateContainer.setChessboardState(ChessboardState::Stalemate);
	}
	else if (isRepeatingPosition())
	{
		updateContainer.setChessboardState(ChessboardState::RepeatingPosition);
	}
	else if (isFiguresLackToMate(opponentPlayer) && isFiguresLackToMate(currentPlayer))
	{
		updateContainer.setChessboardState(ChessboardState::FiguresLack);
	}
}

bool ChessBoardModel::isSingleFieldFigures(BaseFigure* firstBishop, BaseFigure* secondBishop)
{
	return (firstBishop->getCurrentPosition().Row + firstBishop->getCurrentPosition().Column) % 2 == (secondBishop->getCurrentPosition().Row + secondBishop->getCurrentPosition().Column) % 2;
}

std::vector<Position> ChessBoardModel::findValidMovements(BaseFigure* selectedFigure)
{
	std::vector<Position> validMovements;
	auto selectedFigureMovements = selectedFigure->findPossibleMovementsPositions(ChessBoard);
	auto player = getPlayer(selectedFigure->getColor());
	auto& kingPosition = player->getKingCurrentPosition();
	auto& selectedFigurePosition = selectedFigure->getCurrentPosition();

	if (kingPosition.Row == selectedFigurePosition.Row || kingPosition.Column == selectedFigurePosition.Column
		|| abs(kingPosition.Row - selectedFigurePosition.Row) == abs(kingPosition.Column - selectedFigurePosition.Column)
		|| isPlayerCheck(player))
	{
		for (const auto& movementPosition : selectedFigureMovements)
		{
			auto updates = selectedFigure->findChessBoardUpdates(ChessBoard, movementPosition, true);
			doChessBoarUpdates(updates.getUpdates());
			if (!isPlayerCheck(player))
			{
				validMovements.push_back(movementPosition);
			}
			doChessBoarUpdates(updates.getCancelingUpdates(), true);
		}
	}
	else
	{
		validMovements = selectedFigureMovements;
	}
	return validMovements;
}

Player* ChessBoardModel::getCurrentOpponentPlayer() const 
{
	return players[(currentPlayer->getColor() + 1) % playersCount];
}

Player* ChessBoardModel::getOpponentPlayer(Color color) const
{
	return players[(color + 1) % playersCount];
}

BaseFigure* (&ChessBoardModel::getChessBoard())[cagesCount][cagesCount]
{
	return ChessBoard;
}

bool ChessBoardModel::isPreviousMove() const 
{
	return movementsHistorySize > 0;
}

bool ChessBoardModel::isNextMove() const 
{
	return movementsHistorySize < movementsHistory.size();
}
