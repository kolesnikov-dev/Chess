#include "BaseFigure.h"

void Move::setMoveType(MoveType type, bool resetPreviousStates)
{
	if (resetPreviousStates)
	{
		this->type.reset();
	}
	this->type.set(static_cast<int>(type));
}

moveTypeMask Move::getMoveType() const
{
	return type;
}

void Move::setChessboardState(ChessboardState state)
{
	this->state.set(state);
}

boardStateMask Move::getChessboardState() const
{
	return state;
}

void Move::addUpdate(const Position& position, const std::shared_ptr<BaseFigure>& figure)
{
	updates.push_back({ position, figure });
	cancelingUpdates.insert(cancelingUpdates.begin(), { figure->getCurrentPosition(), figure });
}

const std::vector<Update> Move::getUpdates() const
{
	return updates;
}

const std::vector<Update> Move::getCancelingUpdates() const
{
	return cancelingUpdates;
}

void Move::clear()
{
	updates.clear();
	cancelingUpdates.clear();
}

BaseFigure::BaseFigure(const Position& currentPosition, const Color color)
	: currentPosition(currentPosition), color(color), moveId(0) {}

BaseFigure::BaseFigure()
	: currentPosition(deletePosition), color(Color()), moveId(0) {}

BaseFigure::~BaseFigure() {}

int BaseFigure::generateKey(FigureType type, Color color)
{
	return 10 * static_cast<int>(color) + static_cast<int>(type);
}

int BaseFigure::getKey() const
{
	return generateKey(getFigureType(), getColor());
}

void BaseFigure::setNewState(const Position& newPosition)
{
	++moveId;
	currentPosition = newPosition;
}

void BaseFigure::setColor(const Color color)
{
	this->color = color;
}

Color BaseFigure::getColor() const
{
	return color;
}

const Position& BaseFigure::getCurrentPosition() const
{
	return currentPosition;
}

void BaseFigure::addMovementParameter(const Position& movementDelta, uint8_t movementParametrs)
{
	movementParameters.push_back(std::make_pair(movementDelta, movementParametrs));
}

void BaseFigure::restorePreviousState(const Position& oldPosition)
{
	currentPosition = oldPosition;
}

Move BaseFigure::findChessBoardUpdates(std::shared_ptr<BaseFigure> const (&ChessBoard)[cagesCount][cagesCount], const Position& newPosition, bool ignoreSafeMoves)
{
	Move updates;
	if (ChessBoard[newPosition.Row][newPosition.Column]->getFigureType() != FigureType::NullFigure)
	{
		updates.addUpdate(deletePosition, ChessBoard[newPosition.Row][newPosition.Column]);
		updates.setMoveType(MoveType::Capture);
	}
	else
	{
		updates.setMoveType(MoveType::SimpleMove);
	}
	updates.addUpdate(newPosition, ChessBoard[currentPosition.Row][currentPosition.Column]);
	return updates;
}

std::vector<Position> BaseFigure::findPossibleMovementsPositions(std::shared_ptr<BaseFigure> const (&ChessBoard)[cagesCount][cagesCount], bool onlyAttackMovements)
{
	std::vector<Position> movementsPositions;
	for (auto idx = 0; idx < movementParameters.size(); ++idx)
	{
		auto delta = movementParameters[idx].first;
		auto parameters = movementParameters[idx].second;
		auto newPosition = currentPosition;
		do
		{
			newPosition = newPosition + delta;
			if (newPosition.isValidPosition(0, cagesCount - 1))
			{
				auto newFigure = ChessBoard[newPosition.Row][newPosition.Column];
				if (newFigure->getFigureType() != FigureType::NullFigure && newFigure->getColor() == color)
				{
					break;
				}
				else if (newFigure->getFigureType() != FigureType::NullFigure && newFigure->getColor() != color)
				{
					if (parameters & MovementParameters::onlyAttackMovements)
					{
						movementsPositions.push_back(newPosition);
					}

					if (parameters & MovementParameters::movementWithOneAttack)
					{
						movementsPositions.push_back(newPosition);
						break;
					}
				}
				else if (parameters & MovementParameters::onlyAttackMovements)
				{
					break;
				}
				else if (!onlyAttackMovements)
				{
					movementsPositions.push_back(newPosition);
				}
			}
			else
			{
				break;
			}
		} while (parameters & MovementParameters::recursiveMovement);
	}
	return movementsPositions;
}