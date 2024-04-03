#pragma once

#include "Model/Position.h"

#include <array>
#include <bitset>
#include <string>

constexpr short columnsCount = 8;
constexpr short rowsCount = 8;
constexpr short cagesCount = 8;
constexpr short playersCount = 2;
constexpr short uniqueFiguresCount = 6;

using moveTypeMask = std::bitset<8>;
using boardStateMask = std::bitset<8>;

enum class FigureType {
	NullFigure, //0
	Pawn,       //1
	King,       //2
	Rook,       //3
	Knight,     //4
	Queen,      //5
	Bishop      //6
};

constexpr int figuresToChooseCount = 4;
const FigureType chooseFiguresTypes[figuresToChooseCount]
{
	FigureType::Bishop,
	FigureType::Knight,
	FigureType::Queen,
	FigureType::Rook
};

enum ChessboardState { BaseState = 0, Check, Mate, Stalemate, FiguresLack, RepeatingPosition, GiveUp };

enum MoveType { SimpleMove, Capture, Promotion, ENPassant, LongCastling, ShortCastling };

enum Color { Black = 0, White };

enum Direction { Left = -1, Right = 1 };	

constexpr Position deletePosition{ -1, -1 };

const std::string gameArchiveDirectory = "GamesArchive/";
const std::string appDataFilename = gameArchiveDirectory + ".appdata";
const std::string figureImagesPath = "FigureImages/";
const std::string iconsFolderName = "Icons/";