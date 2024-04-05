#include "ChessBoardController.h"
#include "../Model/ChessBoardModel.h"
#include "../View/ChessBoardView.h"
#include "ControllerStates.h"

#include <iostream>
#include <fstream>
#include <filesystem>

ChessBoardController::ChessBoardController(ChessBoardModel& model)
	: chBModel(model), chBView(ChessBoardView(model, *this)),
	selectFigure(new SelectFigure(chBModel, chBView, *this)),
	selectMovement(new SelectMovement(chBModel, chBView, *this)),
	endGame(new EndGame(chBModel, chBView, *this)), 
	notation(LongAlgebraicNotation(getColorString(Color::White), chBModel.getPlayer(Color::White)->getBaseLine()))
{
	state = getSelectFigureState();
}

ChessBoardController::~ChessBoardController()
{
	if (&chBModel)
		delete& chBModel;
	if (&selectFigure)
		delete selectFigure;
	if (&selectMovement)
		delete selectMovement;
	if (&endGame)
		delete endGame;
}

void ChessBoardController::selectPosition(const QPoint& selectedPosition)
{
	state->selectPosition(selectedPosition);
}

void ChessBoardController::printInfoMessage(const std::string message)
{
	state->printInfoMessage(message);
}

void ChessBoardController::setState(ControllerStates* newState)
{
	state = newState;
}

ControllerStates* ChessBoardController::getSelectFigureState() const
{
	return selectFigure;
}

ControllerStates* ChessBoardController::getEndGameState() const
{
	return endGame;
}

ControllerStates* ChessBoardController::getSelectMovementState() const
{
	return selectMovement;
}

bool ChessBoardController::isNextMove()
{
	return chBModel.isNextMove();
}

bool ChessBoardController::isPreviousMove()
{
	return chBModel.isPreviousMove();
}

bool ChessBoardController::undoMovement()
{
	if (isPreviousMove())
	{
		auto moveUpdates = chBModel.undoMovement();
		chBView.undoMovement(moveUpdates);
		printStatusBarMessage();
		if (!moveUpdates.getUpdates().empty())
		{
			chBModel.changePlayerToMove();
		}
		setState(getSelectFigureState());

		return true;
	}
	return false;
}

bool ChessBoardController::redoMovement()
{
	if (isNextMove())
	{
		auto moveUpdates = chBModel.redoMovement();
		chBView.redoMovement(moveUpdates);
		printStatusBarMessage();
		chBModel.changePlayerToMove();
		setState(getSelectFigureState());
		return true;
	}
	return false;
}

bool ChessBoardController::readMoves(const std::string& fileName)
{
	std::vector<Position> moves;
	std::queue<FigureType> predefinedTypes;
	if (!notation.readMoves(fileName, moves, predefinedTypes))
	{
		return false;
	}

	chBView.setView(false);
	for (const auto& position : moves)
	{
		selectPosition(QPoint(position.Column, position.Row));
		if (chBModel.isNeedToChangePawn())
		{
			changePawn(predefinedTypes.front());
			predefinedTypes.pop();
		}
	}
	chBView.setView(true);
	chBView.update();

	return true;
}

bool ChessBoardController::writeMoves(const QTextTable* movesHistoryTable, const std::string& fileName)
{
	auto path = setNewFilePath(fileName);
	if (movesHistoryTable)
	{
		return notation.writeMovesInNotations(movesHistoryTable, path);
	}
	return notation.writeMovesInNotations(chBModel.getMovementHistory(), path);
}

void ChessBoardController::printStatusBarMessage(const std::string& message)
{
	std::string updatedMsg = message;
	if (updatedMsg == std::string())
	{
		updatedMsg = std::string("It is ") + getColorString(chBModel.getCurrentOpponentPlayer()->getColor()) + " figures turn to move";
		if (chBModel.isPlayerCheck(chBModel.getCurrentOpponentPlayer()))
		{
			updatedMsg += std::string("; The ") + getColorString(chBModel.getCurrentOpponentPlayer()->getColor()) + " king is in check";
		}
	}
	chBView.printStatusBarMessage(updatedMsg);
}

void ChessBoardController::changePawn(const FigureType type)
{
	auto updates = chBModel.changePawn(type);
	auto changePawnUpdate = updates.getUpdates().back();

	printStatusBarMessage();
	chBModel.changePlayerToMove();
	chBView.redrawCage(QPoint(changePawnUpdate.position.Column, changePawnUpdate.position.Row));
	chBView.notateLastMove(updates);
	setState(getSelectFigureState());
}

void ChessBoardController::checkChesboardState(boardStateMask ChessboardState)
{
	auto state = ChessboardState | chBModel.getChessboardState();
	if (state.test(ChessboardState::Mate))
	{
		setState(getEndGameState());
		printInfoMessage(std::string("Victory goes to the ") + getColorString(chBModel.getCurrentOpponentPlayer()->getColor()) + " figures!");
		return;
	}
	else if (state.test(ChessboardState::Stalemate))
	{
		setState(getEndGameState());
		printInfoMessage(std::string("Draw! There is a stalemate"));
		return;
	}
	else if (state.test(ChessboardState::RepeatingPosition))
	{
		setState(getEndGameState());
		printInfoMessage(std::string("Draw! There is a repeating position"));
		return;
	}
	else if (state.test(ChessboardState::FiguresLack))
	{
		setState(getEndGameState());
		printInfoMessage(std::string("Draw! There is no enought figures to mate"));
		return;
	}
	else if (state.test(ChessboardState::GiveUp))
	{
		setState(getEndGameState());
		printInfoMessage(std::string("The opponent gave up. Victory goes to the ") + getColorString(chBModel.getCurrentOpponentPlayer()->getColor()) + " figures!");;
		return;
	}
}

void ChessBoardController::clearView()
{
	chBView.setView(false);
	while (undoMovement());
	chBView.setView(true);
	chBView.infoMessage("It is white figures turn to move");
	chBView.update();
}

void ChessBoardController::giveUp()
{
	chBView.clearMoveTipsPositions();
	boardStateMask state;
	state.set(ChessboardState::GiveUp);
	checkChesboardState(state);
}

std::string ChessBoardController::setNewFilePath(const fs::path& path)
{
	auto lastFileNum = 0;
	std::string processedFilename = path.stem().string();

	std::ifstream inAppData(appDataFilename, std::ios::in);
	if (inAppData.is_open())
	{
		std::string line;
		std::getline(inAppData, line);
		if (!line.empty())
		{
			lastFileNum = std::stoi(line);
		}
		inAppData.close();
	}
	else
	{
		fs::create_directories(fs::path(appDataFilename).parent_path());
	}

	std::ofstream outAppData(appDataFilename, std::ios::out);
	if (!outAppData.is_open())
	{
		return std::string();
	}

	if (processedFilename.empty())
	{
		do
		{
			processedFilename = std::to_string(++lastFileNum);
		} while (fs::exists(getFilePath(processedFilename)));
		outAppData << processedFilename << std::endl;
		outAppData << processedFilename << std::endl;
	}
	else
	{
		outAppData << std::to_string(lastFileNum) << std::endl;
		outAppData << processedFilename << std::endl;
	}
	outAppData.close();

	return getFilePath(processedFilename);
}

std::string ChessBoardController::getLastFilePath()
{
	std::ifstream inAppData(appDataFilename, std::ios::in);
	std::string line;
	if (inAppData.is_open())
	{
		std::getline(inAppData, line);
		std::getline(inAppData, line);
		auto filePath = getFilePath(line);
		if (fs::exists(filePath))
		{
			return filePath;
		}
	}
	return std::string();
}

std::string ChessBoardController::getFilePath(const std::string& filename)
{
	return gameArchiveDirectory + filename + std::string(".txt");
}

std::string ChessBoardController::getNotationsString(const Move& updatesContainer)
{
	return notation.getNotationsString(updatesContainer);
}

std::string ChessBoardController::getColorString(const Color color) const
{
	switch (color)
	{
	case Color::White:
		return "white";
		break;
	case Color::Black:
		return "black";
		break;
	default:
		return "";
		break;
	}
}

ChessBoardView* ChessBoardController::getChessboardView()
{
	return &chBView;
}