#include "ChooseFigureWidget.h"
#include "ChessBoardView.h"

#include <QPainter>
#include <QPointF>

ChooseFigureWidget::ChooseFigureWidget(QWidget* parent) : QWidget(parent)
{
	setWindowFlag(Qt::WindowType::Popup);
}

void ChooseFigureWidget::initialize(const QHash<int, QPixmap>& figuresPixmaps, Color figuresColor)
{
	if (figuresPixmaps.isEmpty())
		return;

	choosedFiguresPixmaps.clear();
	transform.reset();
	invertedTransform.reset();

	auto pixmapSize = (*figuresPixmaps.begin()).size();
	setFixedSize(figuresToChooseCount * pixmapSize.width(), pixmapSize.height());
	transform.scale(static_cast<double>(width() / figuresToChooseCount), height());
	invertedTransform = transform.inverted();

	for (auto i = 0; i < figuresToChooseCount; ++i)
	{
		int key = BaseFigure::generateKey(chooseFiguresTypes[i], figuresColor);
		choosedFiguresPixmaps.push_back(figuresPixmaps.value(key));
	}

	auto palette = this->palette();
	palette.setColor(QPalette::Window, QColorConstants::Svg::darkgoldenrod);
	setPalette(palette);
}

void ChooseFigureWidget::paintEvent(QPaintEvent* event)
{
	if (choosedFiguresPixmaps.size() < figuresToChooseCount)
		return;

	QPainter painter(this);
	for (auto i = 0; i < figuresToChooseCount; ++i)
	{
		if (choosedFiguresPixmaps[i].isNull())
			continue;
		painter.drawPixmap(transform.map(QPoint(i, 0)), choosedFiguresPixmaps[i]);
	}
}

void ChooseFigureWidget::mousePressEvent(QMouseEvent* event)
{
	if (geometry().contains(event->globalPos()))
	{
		QPointF choosedElementPos = invertedTransform.map(static_cast<QPointF>(event->pos()));
		int choosedElementIdx = std::floor(choosedElementPos.x());
		if (choosedElementIdx >= 0 && choosedElementIdx < figuresToChooseCount)
		{
			emit choosedFigure(chooseFiguresTypes[choosedElementIdx]);
		}
	}
}