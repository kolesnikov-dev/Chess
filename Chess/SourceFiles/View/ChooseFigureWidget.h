#pragma once

#include "../Model/Figures/BaseFigure.h"

#include <QWidget>
#include <QPixmap>
#include <QWidget>
#include <QList>
#include <QTransform>
#include <QPaintEvent>
#include <QMouseEvent>

class ChooseFigureWidget : public QWidget
{
	Q_OBJECT;

public:
	ChooseFigureWidget(QWidget* parent = nullptr);

signals:
	void choosedFigure(const FigureType type);

public:
	void initialize(const QHash<int, QPixmap>& figuresPixmaps, Color figuresColor);

	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;

private:
	QTransform transform, invertedTransform;
	QList<QPixmap> choosedFiguresPixmaps;
};

