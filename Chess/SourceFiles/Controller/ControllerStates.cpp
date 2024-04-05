#include "ControllerStates.h"

SelectMovement::SelectMovement(ChessBoardModel& chBModel, ChessBoardView& chBView, ChessBoardController& chBController)
	: ControllerStates(chBModel, chBView, chBController) {}

void SelectMovement::printInfoMessage(const std::string message) {};

void SelectMovement::selectPosition(const QPoint& movementPosition)
{
	if (movementPosition.x() == chBModel.getSelectedFigure()->getCurrentPosition().Column &&
		movementPosition.y() == chBModel.getSelectedFigure()->getCurrentPosition().Row)
	{
		return;
	}

	chBView.clearMoveTipsPositions();
	auto moveUpdates = chBModel.moveSelectedFigure(Position(movementPosition.y(), movementPosition.x()));
	if (!moveUpdates.getUpdates().empty())
	{
		chBView.redrawFigure(moveUpdates);

		if (chBModel.isNeedToChangePawn())
		{
			chBController.printStatusBarMessage("It is need to choose a figure");
			chBView.setReplacedPawn();
			chBController.setState(chBController.getSelectFigureState());
			return;
		}
		else
		{
			chBController.printStatusBarMessage();
			chBView.notateLastMove(moveUpdates);
		}
		chBModel.changePlayerToMove();
		chBController.checkChesboardState();
	}

	chBController.setState(chBController.getSelectFigureState());
	if (moveUpdates.getUpdates().empty())
	{
		chBController.selectPosition(movementPosition);
	}
}

EndGame::EndGame(ChessBoardModel& chBModel, ChessBoardView& chBView, ChessBoardController& chBController)
	: ControllerStates(chBModel, chBView, chBController) {}

void EndGame::printInfoMessage(const std::string message)
{
	chBView.showEndgameWindow(message);
	chBController.setState(chBController.getSelectFigureState());
}

void EndGame::selectPosition(const QPoint& selectedPosition) {}

SelectFigure::SelectFigure(ChessBoardModel& chBModel, ChessBoardView& chBView, ChessBoardController& chBController)
	: ControllerStates(chBModel, chBView, chBController) {}

void SelectFigure::printInfoMessage(const std::string message) {}

void SelectFigure::selectPosition(const QPoint& selectedPosition)
{
	chBModel.setSelectedFigure(Position(selectedPosition.y(), selectedPosition.x()));
	if (chBModel.isSelectedFigure())
	{
		auto& movementsIdxs = chBModel.getSelectedFigureValidMovements();
		chBView.onMoveTipsPosition(movementsIdxs);
		chBController.setState(chBController.getSelectMovementState());
	}
}