#pragma once

#include "../../Constants.h"

#include <vector>
#include <memory>

class BaseFigure;

struct Update
{
	Position position;
	std::shared_ptr<BaseFigure> figure;
};

class Move
{
public:
	void addUpdate(const Position& position, const std::shared_ptr<BaseFigure>& figure);
	const std::vector<Update> getUpdates() const;
	const std::vector<Update> getCancelingUpdates() const;

	void setMoveType(MoveType type, bool resetPreviousStates = true);
	moveTypeMask getMoveType() const;

	void setChessboardState(ChessboardState state);
	boardStateMask getChessboardState() const;

	void clear();

private:
	moveTypeMask type;
	boardStateMask state;

	std::vector<Update> updates;
	std::vector<Update> cancelingUpdates;
};

class BaseFigure
{
public:
	BaseFigure(const Position& currentPosition, const Color color);
	BaseFigure();

	virtual ~BaseFigure();

public:
	static int generateKey(FigureType type, Color color);

public: 
	virtual void setMovementsParameters() = 0;
	virtual FigureType getFigureType() const = 0;

	virtual void setNewState(const Position& newPosition);
	virtual void restorePreviousState(const Position& oldPosition);

	virtual std::vector<Position> findPossibleMovementsPositions(std::shared_ptr<BaseFigure> const (&ChessBoard)[cagesCount][cagesCount], bool onlyAttackMovements = false);
	virtual Move findChessBoardUpdates(std::shared_ptr<BaseFigure> const (&ChessBoard)[cagesCount][cagesCount], const Position& newPosition, bool ignoreSafeMoves = false);

public:
	const Position& getCurrentPosition() const;
	int getKey() const;

	void setColor(const Color color);
	Color getColor() const;

protected:
	void addMovementParameter(const Position& movementDelta, uint8_t movementParametrs);

protected:
	enum MovementParameters : uint8_t
	{
		noAttackMovement = 0x00,
		recursiveMovement = 0x01,
		movementWithOneAttack = 0x02,
		onlyAttackMovements = 0x04
	};

protected:
	Position currentPosition;
	Color color;
	int moveId;

private:
	std::vector<std::pair<Position, uint8_t>> movementParameters;
};
