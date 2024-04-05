#include "ChessBoardView.h"
#include "../Controller/ChessBoardController.h"
#include "ChooseFigureWidget.h"

#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QRect>
#include <QRectF>
#include <QTransform>
#include <QRegion>
#include <QSizePolicy>

#include <iostream>
#include <cmath>


ChessBoardView::ChessBoardView(ChessBoardModel& ChBModel, ChessBoardController& ChBController, QWidget* parent) : QWidget(parent), ChBModel(ChBModel), ChBController(ChBController),
endgameDialog(nullptr), chooseFigureWidget(nullptr), messageLabel(nullptr),
isReplaceMovementPressed(false), isCancelMovementPressed(false),
isView(true), isMoveTips(true), isLastMoveTip(true),
QFigureImagesPath(figureImagesPath), QDeletePostion(deletePosition.Column, deletePosition.Row)
{
	if (ChBModel.getPlayer(Color::White)->getBaseLine() == 0)
	{
		deletedFigures.insert(Color::White, { ChBModel.getPlayer(Color::Black)->getBaseLine() + 1.75, QStack<int>() });
		deletedFigures.insert(Color::Black, { ChBModel.getPlayer(Color::White)->getBaseLine() - 1.0, QStack<int>() });
		isCoordsTransform = true;
	}
	else
	{
		deletedFigures.insert(Color::White, { ChBModel.getPlayer(Color::Black)->getBaseLine() - 1.0, QStack<int>() });
		deletedFigures.insert(Color::Black, { ChBModel.getPlayer(Color::White)->getBaseLine() + 1.75, QStack<int>() });
		isCoordsTransform = false;
	}

	auto key = BaseFigure::generateKey(FigureType::Bishop, Color::Black);
	QPixmap pixmap(QFigureImagesPath + QString::number(key) + ".png");
	baseFigureSize = pixmap.size();
	setMinimumSize(scaleKoef * baseFigureSize / 4);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	updateGeometry();
}

QSize ChessBoardView::sizeHint() const
{
	return scaleKoef * baseFigureSize / 2;
}

bool ChessBoardView::checkIsPointValid(const QPoint& point) const
{
	return point.x() <= cagesCount - 1 || point.x() >= 0 || point.y() <= cagesCount - 1 || point.y() >= 0;
}

void ChessBoardView::onMoveTipsPosition(const std::vector<Position>& positions)
{
	clearMoveTipsPositions();
	for (const auto& pos : positions)
	{
		QPoint point(pos.Column, pos.Row);
		if (!checkIsPointValid(point))
			continue;

		moveTipsCages.push_back(QRectF(point, point + QPoint(1, 1)));
		moveTipsRegion = moveTipsRegion.united(moveTipsCages.back().toRect());
	}
	if (!isView) return;
	repaint(chessboardTransform.map(moveTipsRegion));
}

void ChessBoardView::clearMoveTipsPositions()
{
	moveTipsCages.clear();
	if (isView) 
		repaint(chessboardTransform.map(moveTipsRegion));
	moveTipsRegion = QRegion();
}

void ChessBoardView::onLastMovedFigureCages(const Move& moveUpdates)
{
	clearLastMoveTipPositions();
	for (auto i = 0; i < moveUpdates.getUpdates().size(); ++i)
	{
		QPoint newPosition(moveUpdates.getUpdates()[i].position.Column, moveUpdates.getUpdates()[i].position.Row);
		QPoint oldPosition(moveUpdates.getCancelingUpdates()[moveUpdates.getUpdates().size() - 1 - i].position.Column, moveUpdates.getCancelingUpdates()[moveUpdates.getUpdates().size() - 1 - i].position.Row);
		if (QDeletePostion != oldPosition && QDeletePostion != newPosition)
		{
			lastMoveTipCages.push_back(QRectF(oldPosition, oldPosition + QPoint(1, 1)));
			lastMoveTipCages.push_back(QRectF(newPosition, newPosition + QPoint(1, 1)));
		}
	}
	for (const auto& cage : lastMoveTipCages)
	{
		lastMoveRegion = lastMoveRegion.united(cage.toRect());
	}
	if (!isView) return;
	repaint(chessboardTransform.map(lastMoveRegion));
}

void ChessBoardView::clearLastMoveTipPositions()
{
	lastMoveTipCages.clear();
	if (isView)
		repaint(chessboardTransform.map(lastMoveRegion));
	lastMoveRegion = QRegion();
}

void ChessBoardView::moveTips(bool state)
{
	clearMoveTipsPositions();
	isMoveTips = state;
}

void ChessBoardView::lastMoveTip(bool state)
{
	clearLastMoveTipPositions();
	isLastMoveTip = state;
}

void ChessBoardView::setView(bool isView)
{
	this->isView = isView;
}

void ChessBoardView::setNewFigureType(const FigureType type)
{
	chooseFigureWidget->hide();
	ChBController.changePawn(type);
}

void ChessBoardView::showEndgameWindow(const std::string message)
{
	if (!isView) return;

	if (!endgameDialog)
	{
		endgameDialog = new QDialog(this);
		endgameDialog->setModal(true);

		messageLabel = new QLabel;
		messageLabel->setFont(QFont(qApp->font().family(), 16));
		messageLabel->setAlignment(Qt::AlignCenter);

		QPushButton* backButton = new QPushButton("Back to game");
		QPushButton* menuButton = new QPushButton("Menu");
		menuButton->setMinimumSize(backButton->minimumSizeHint());

		QGridLayout* popupLayout = new QGridLayout;
		popupLayout->addWidget(messageLabel, 0, 0, 1, 2, Qt::AlignHCenter);
		popupLayout->addWidget(backButton, 1, 0);
		popupLayout->addWidget(menuButton, 1, 1);
		endgameDialog->setLayout(popupLayout);

		connect(menuButton, &QPushButton::clicked, this, &ChessBoardView::showApplicationMenu);
		connect(menuButton, &QPushButton::clicked, endgameDialog, &ChessBoardView::hide);
		connect(backButton, &QPushButton::clicked, endgameDialog, &ChessBoardView::hide);
	}
	messageLabel->setText(message.c_str());
	messageLabel->layout()->update();
	endgameDialog->show();
}

void ChessBoardView::setReplacedPawn()
{
	if (!isView) return;

	if (!chooseFigureWidget)
	{
		chooseFigureWidget = new ChooseFigureWidget(this);
		connect(chooseFigureWidget, &ChooseFigureWidget::choosedFigure, this, &ChessBoardView::setNewFigureType);
	}
	updateChooseFigureWidget();
	chooseFigureWidget->show();
}

void ChessBoardView::updateChooseFigureWidget()
{
	if (chooseFigureWidget)
	{
		chooseFigureWidget->initialize(figuresPixmaps, ChBModel.getCurrentPlayer()->getColor());
		auto coordY = deletedFigures.value(ChBModel.getCurrentPlayer()->getColor()).startPositionY < 0 ? -chooseFigureWidget->height() : 1 + chooseFigureWidget->height();
		QPoint offset = chessboardTransform.map(lastChoosedPosition + QPointF(0.5, 0)).toPoint();
		offset = this->mapToGlobal(QPoint(offset + QPoint(-chooseFigureWidget->width() / 2, coordY)));
		offset.setX(qMax(0, offset.x()));
		offset.setY(qMax(0, offset.y()));
		chooseFigureWidget->move(offset);
	}
}

void ChessBoardView::undoMovement(const Move& moveUpdates)
{
	isCancelMovementPressed = true;
	clearMoveTipsPositions();
	lastMoveTip(false);
	redrawFigure(moveUpdates, true);
	lastMoveTip(true);
	onLastMovedFigureCages(ChBModel.getCurrentMovement());
	notateLastMove(moveUpdates);
	isCancelMovementPressed = false;
	emit undoMove();
}

void ChessBoardView::redoMovement(const Move& moveUpdates)
{
	isReplaceMovementPressed = true;
	clearMoveTipsPositions();
	redrawFigure(moveUpdates);
	notateLastMove(moveUpdates);
	isReplaceMovementPressed = false;
	emit redoMove();
}

void ChessBoardView::scalePixmap(const double scale, QPixmap& pixmap) const
{
	if (scale <= 0 || pixmap.isNull())
		return;

	QPoint point((chessboardTransform.map(QPointF(scale, 0)) - chessboardTransform.map(QPointF(0, 0))).toPoint());
	QSize size(point.x(), point.x());
	pixmap = pixmap.scaled(size);
}

void ChessBoardView::redrawCage(const QPoint& position)
{
	if (checkIsPointValid(position))
	{
		repaint(chessboardTransform.mapRect(QRectF(position, position + QPoint(1, 1))).toRect());
	}
}

void ChessBoardView::redrawFigure(const Move& moveUpdates, bool cancelMove)
{
	onLastMovedFigureCages(moveUpdates);

	auto updates = cancelMove ? moveUpdates.getUpdates() : moveUpdates.getCancelingUpdates();
	for (const auto& update : updates)
	{
		auto figure = update.figure;
		QPoint newPosition(figure->getCurrentPosition().Column, figure->getCurrentPosition().Row);
		QPoint oldPosition(update.position.Column, update.position.Row);
		if (!checkIsPointValid(newPosition) || !checkIsPointValid(oldPosition))
			continue;

		if (QDeletePostion == newPosition && !isCancelMovementPressed && (!moveUpdates.getMoveType().test(MoveType::Promotion) || update.figure->getFigureType() != FigureType::Pawn))
		{
			auto startPositionY = deletedFigures.value(figure->getColor()).startPositionY;
			auto offsetX = delFiguresKoef * deletedFigures.value(figure->getColor()).figuresKeys.size();
			deletedFigures[figure->getColor()].figuresKeys.push(figure->getKey());
			if (!isView) return;
			redrawCage(oldPosition);
			repaint(chessboardTransform.mapRect(QRectF(QPointF(offsetX, startPositionY), QPointF(offsetX + delFiguresKoef, startPositionY + delFiguresKoef))).toRect());
		}
		else if (QDeletePostion == oldPosition && isCancelMovementPressed && (!moveUpdates.getMoveType().test(MoveType::Promotion) || update.figure->getFigureType() != FigureType::Pawn))
		{
			auto startPositionY = deletedFigures.value(figure->getColor()).startPositionY;
			deletedFigures[figure->getColor()].figuresKeys.pop();
			auto offsetX = delFiguresKoef * deletedFigures.value(figure->getColor()).figuresKeys.size();
			if (!isView) return;
			redrawCage(newPosition);
			repaint(chessboardTransform.mapRect(QRectF(QPointF(offsetX, startPositionY), QPointF(offsetX + delFiguresKoef, startPositionY + delFiguresKoef))).toRect());
		}
		else
		{
			QList<QPoint> updateCagesCoords;
			if (QDeletePostion != oldPosition)
				updateCagesCoords.push_back(oldPosition);
			if (QDeletePostion != newPosition)
				updateCagesCoords.push_back(newPosition);

			if (!isView) continue;
			for (const auto& cageCoords : updateCagesCoords)
			{
				redrawCage(cageCoords);
			}
		}
	}
}

void ChessBoardView::paintEvent(QPaintEvent* event)
{
	if (!isView) return;

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	static QColor colorArr[2]{ QColorConstants::Svg::white, QColorConstants::Svg::brown };
	auto penWidth = painter.pen().width();
	for (auto i = 0; i < cagesCount; ++i)
	{
		for (auto j = 0; j < cagesCount; ++j)
		{
			painter.setPen(colorArr[(i + j) % 2]);
			painter.setBrush(colorArr[(i + j) % 2]);
			painter.drawRect(chessboardTransform.mapRect(QRectF(QPointF(i, j), QPointF(i + 1, j + 1))).marginsRemoved(QMargins(penWidth, penWidth, penWidth, penWidth)));
		}
	}

	auto font = painter.font();
	font.setBold(true);
	font.setPointSizeF(delFiguresKoef * chessboardTransform.m22());
	painter.setFont(font);

	int num = 1;
	for (auto i = cagesCount; i > 0; --i)
	{
		painter.drawText(QRectF(coordLabelsTransform.map(QPointF(-1, i - 1)), coordLabelsTransform.map(QPointF(0, i))), Qt::AlignCenter, QString::number(num++));
	}

	char symb = 'A';
	for (auto i = 0; i < cagesCount; ++i)
	{
		painter.drawText(QRectF(coordLabelsTransform.map(QPointF(i, cagesCount)), coordLabelsTransform.map(QPointF(i + 1, cagesCount + 1))), Qt::AlignCenter, QChar(symb++));
	}

	for (auto i = 0; i < cagesCount; ++i)
	{
		for (auto j = 0; j < cagesCount; ++j)
		{
			auto figure = ChBModel.getChessBoard()[i][j];
			if (figure->getFigureType() != FigureType::NullFigure)
			{
				QPixmap pixmap = figuresPixmaps.value(figure->getKey());
				painter.drawPixmap(chessboardTransform.map(QPointF(j, i)), pixmap);
			}
		}
	}

	if (isLastMoveTip)
	{
		painter.save();
		painter.setBrush(Qt::NoBrush);
		painter.setPen(QPen(QBrush(QColorConstants::DarkBlue), 2));
		for (const auto& cage : lastMoveTipCages)
		{
			auto boundingRect = chessboardTransform.mapRect(cage).toRect();
			boundingRect = boundingRect.adjusted(2, 2, -2, -2);
			painter.drawRect(boundingRect);
		}
		painter.restore();
	}

	if (isMoveTips)
	{
		QColor highlightColor;
		for (auto& cage : moveTipsCages)
		{
			QPoint coord = cage.topLeft().toPoint();
			if (ChBModel.getChessBoard()[coord.y()][coord.x()]->getFigureType() != FigureType::NullFigure)
			{
				highlightColor.setRgb(QColorConstants::Red.rgb());
			}
			else
			{
				highlightColor.setRgb(QColorConstants::DarkGreen.rgb());
			}
			highlightColor.setAlpha(255 / 2);
			painter.setBrush(highlightColor);
			painter.drawRect(chessboardTransform.mapRect(cage));
		}
	}

	for (const auto& figuresSet : deletedFigures)
	{
		double offsetX = 0;
		double startY = figuresSet.startPositionY;
		for (const auto& key : figuresSet.figuresKeys)
		{
			QPixmap pixmap = figuresPixmaps.value(key);
			scalePixmap(delFiguresKoef, pixmap);
			painter.drawPixmap(chessboardTransform.map(QPointF(offsetX, startY)), pixmap);
			offsetX += delFiguresKoef;
		}
	}
}

void ChessBoardView::resizeEvent(QResizeEvent* event)
{
	auto side = qMin(event->size().width(), event->size().height());

	chessboardTransform.reset();
	chessboardTransform.scale(side / scaleKoef, side / scaleKoef);
	chessboardTransform.translate(1, 1);
	invertedTransform.reset();
	invertedTransform = chessboardTransform.inverted();

	coordLabelsTransform.reset();
	if (isCoordsTransform)
	{
		coordLabelsTransform.scale(-1 * chessboardTransform.m11(), -1 * chessboardTransform.m22());
		coordLabelsTransform.translate(-9, -9);
	}
	else
	{
		coordLabelsTransform = chessboardTransform;
	}

	for (auto i = 0; i < cagesCount; ++i)
	{
		for (auto j = 0; j < cagesCount; ++j)
		{
			auto figure = ChBModel.getChessBoard()[i][j];
			if (figure->getFigureType() != FigureType::NullFigure)
			{
				int key = figure->getKey();
				QPixmap pixmap(QFigureImagesPath + QString::number(key) + ".png");
				scalePixmap(1, pixmap);
				figuresPixmaps.insert(figure->getKey(), pixmap);
			}
		}
	}
	updateChooseFigureWidget();
}

void ChessBoardView::mousePressEvent(QMouseEvent* event)
{
	QPointF coords = invertedTransform.map(static_cast<QPointF>(event->pos()));
	if (coords.x() < cagesCount && coords.y() < cagesCount)
	{
		lastChoosedPosition = QPoint(std::floor(coords.x()), std::floor(coords.y()));
		ChBController.selectPosition(lastChoosedPosition.toPoint());
	}
}

void ChessBoardView::notateLastMove(const Move& moveUpdates)
{
	if (!isCancelMovementPressed && !isReplaceMovementPressed)
	{
		emit notatedLastMove(ChBController.getNotationsString(moveUpdates).c_str());
	}
}

void ChessBoardView::printStatusBarMessage(const std::string message)
{
	emit infoMessage(message.c_str());
}