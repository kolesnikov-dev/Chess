#pragma once

#include "../Model/ChessBoardModel.h"
#include "../View/ChessBoardView.h"
#include "../Model/LongAlgebraicNotation.h"

#include <string>
#include <filesystem>

class ControllerStates;
class SelectFigure;
class SelectMovement;
class EndGame;

namespace fs = std::filesystem;

class ChessBoardController
{
public:
	ChessBoardController(ChessBoardModel& model);
	~ChessBoardController();

public:
	static std::string getFilePath(const std::string& filename);
	static std::string getLastFilePath();
	static std::string setNewFilePath(const fs::path& path);

	void selectPosition(const QPoint& selectedPosition);
	void printInfoMessage(const std::string message);

	bool readMoves(const std::string& fileName);
	bool writeMoves(const QTextTable* movesHistoryTable = nullptr, const std::string& fileName = std::string());

	bool undoMovement();
	bool redoMovement();

	bool isNextMove();
	bool isPreviousMove();

	void clearView();
	void changePawn(const FigureType type);
	void giveUp();
	void checkChesboardState(boardStateMask ChessboardState = boardStateMask());
	void printStatusBarMessage(const std::string& message = std::string());
	std::string getNotationsString(const Move& updatesContainer);
	ChessBoardView* getChessboardView();

private:
	void setState(ControllerStates* newState);
	ControllerStates* getSelectFigureState() const;
	ControllerStates* getSelectMovementState() const;
	ControllerStates* getEndGameState() const;

	std::string getColorString(const Color color) const;

private:
	ChessBoardView chBView;
	ChessBoardModel& chBModel;
	LongAlgebraicNotation notation;

	SelectFigure* selectFigure;
	SelectMovement* selectMovement;
	EndGame* endGame;
	ControllerStates* state;

	friend class SelectFigure;
	friend class SelectMovement;
	friend class EndGame;
};