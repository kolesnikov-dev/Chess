#pragma once

#include "ChooseFigureWidget.h"

#include <string>

#include <QWidget>
#include <QStack>
#include <QPixmap>
#include <QPointF>
#include <QLabel>
#include <QDialog>

class ChessBoardModel;
class ChessBoardController;

struct DeletedFiguresSet
{
	double startPositionY = 0.0;
	QStack<int> figuresKeys;
};

class ChessBoardView : public QWidget
{
	Q_OBJECT

public:
	ChessBoardView(ChessBoardModel& model, ChessBoardController& controller, QWidget* parent = nullptr);

public slots:
	void moveTips(bool state);
	void lastMoveTip(bool state);

signals:
	void infoMessage(const QString& message);
	void showApplicationMenu();
	void notatedLastMove(const QString& notation);

	void undoMove();
	void redoMove();

public:
	void onMoveTipsPosition(const std::vector<Position>& positions);
	void clearMoveTipsPositions(); 

	void notateLastMove(const Move& moveUpdates);
	void printStatusBarMessage(const std::string message);

	void showChooseNewFigureWidget();
	void showEndgameWindow(const std::string message);

	void redrawFigure(const Move& updates, bool cancelMove = false);
	void redrawCage(const QPoint& position);

	void undoMovement(const Move& moveUpdates);
	void redoMovement(const Move& moveUpdates);

	void setView(bool isView);
	void setNewFigureType(const FigureType type);

public:
	QSize sizeHint() const override;

private:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;        

private:
	void scalePixmap(const double scale, QPixmap& pixmap) const;
	bool checkIsPointValid(const QPoint& point) const;

	void updateChooseFigureWidget();

	void onLastMovedFigureCages(const Move& moveUpdates);
	void clearLastMoveTipPositions();

private:
	const QString QFigureImagesPath;
	const QPoint QDeletePostion;

	const double scaleKoef = 10.0;
	const double delFiguresKoef = 0.25;

	ChessBoardModel& ChBModel;
	ChessBoardController& ChBController;

	bool isCancelMovementPressed, isReplaceMovementPressed;
	bool isCoordsTransform, isView;
	bool isMoveTips, isLastMoveTip;

	QSize baseFigureSize;
	QPointF lastChoosedPosition;
	QRegion moveTipsRegion, lastMoveRegion;
	QTransform coordLabelsTransform, chessboardTransform, invertedTransform;

	QDialog* endgameDialog;
	QLabel* messageLabel;
	ChooseFigureWidget* chooseFigureWidget;

	QList<QRectF> moveTipsCages, lastMoveTipCages;
	QMap<Color, DeletedFiguresSet> deletedFigures;
	QHash<int, QPixmap> figuresPixmaps;
};
