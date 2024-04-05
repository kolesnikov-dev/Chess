#pragma once

#include "../Constants.h"
#include "Figures/BaseFigure.h"
#include "CastlingHelper.h"
#include "PawnMovesHelper.h"
#include "Players.h"

class ChessBoardModel : public PawnMovesHelper, public CastlingHelper
{
public:
	ChessBoardModel(Color firstPlayerColor, Color secondPlayerColor);
	~ChessBoardModel();

public:
	bool isFiguresLackToMate(Player* player);
	bool isStalemate(Player* player);
	bool isMate(Player* player);
	bool isRepeatingPosition(int maxRepeatsCount = 3);

	Move undoMovement();
	Move redoMovement();

	bool isPlayerCheck(const Player* player);
	void setSelectedFigure(const Position& Position);
	void doChessBoarUpdates(std::vector<Update> updates, bool isUpdatesReverse = false);
	std::vector<Position> findValidMovements(const std::shared_ptr<BaseFigure>& selectedFigure);
	Move moveSelectedFigure(const Position& newPosition);
	void changePlayerToMove();

public:
	Player* getCurrentPlayer() const;
	Player* getCurrentOpponentPlayer() const; 
	Player* getOpponentPlayer(Color color) const;
	boardStateMask getChessboardState() const;
	const std::vector<Move>& getMovementHistory() const;
	Move getCurrentMovement() const;
	std::shared_ptr<BaseFigure> getSelectedFigure() const;
	const std::vector<Position>& getSelectedFigureValidMovements() const;

	bool isSelectedFigure() const;
	bool isPreviousMove() const;
	bool isNextMove() const;

public:
	bool isPositionAttacked(const Position& position, Color attackedPositionColor) override;
	bool isLastPawnLongMove(const Position& position) override;

	std::shared_ptr<BaseFigure> (&getChessBoard())[cagesCount][cagesCount] override;
	Player* getPlayer(Color color) const override;

	Move changePawn(FigureType figureType) override;

private:
	bool isSingleFieldFigures(const std::shared_ptr<BaseFigure>& firstBishop, const std::shared_ptr<BaseFigure>& secondBishop);
	void setChessboardState(Move& updateContainer);
	std::shared_ptr<BaseFigure> figuresFabric(FigureType figureType);

private:
	std::shared_ptr<BaseFigure> ChessBoard[cagesCount][cagesCount];
	std::shared_ptr<BaseFigure> selectedFigure;

	int movementsHistorySize;
	std::vector<Move> movementsHistory;
	std::vector<Position> validMovements;

	Player* currentPlayer;
	std::array<Player*, playersCount> players;
};