#pragma once

#include "ChessBoardController.h"

#include <string>

class ControllerStates
{
public:
	ControllerStates(ChessBoardModel& chBModel, ChessBoardView& chBView, ChessBoardController& chBController)
		: chBModel(chBModel), chBView(chBView), chBController(chBController) {}
public:
	virtual void selectPosition(const QPoint& selectedPosition) = 0;
	virtual void printInfoMessage(const std::string message) = 0;

protected:
	ChessBoardModel& chBModel;
	ChessBoardView& chBView;
	ChessBoardController& chBController;
};

class SelectMovement : public ControllerStates
{
public:
	SelectMovement(ChessBoardModel& chBModel, ChessBoardView& chBView, ChessBoardController& chBController);

public:
	void printInfoMessage(const std::string message) override;
	void selectPosition(const QPoint& movementPosition) override;
};

class EndGame : public ControllerStates
{
public:
	EndGame(ChessBoardModel& chBModel, ChessBoardView& chBView, ChessBoardController& chBController);

public:
	void printInfoMessage(const std::string message) override;
	void selectPosition(const QPoint& selectedPosition) override;
};

class SelectFigure : public ControllerStates
{
public:
	SelectFigure(ChessBoardModel& chBModel, ChessBoardView& chBView, ChessBoardController& chBController);

public:
	void printInfoMessage(const std::string message) override;
	void selectPosition(const QPoint& selectedPosition) override;
};