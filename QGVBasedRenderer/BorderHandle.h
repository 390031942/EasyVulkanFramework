#pragma once
#include "Draggable.h"
class BorderHandle :public AbstractDraggable
{
public:
	enum PositionFlag {
		Left = 0,
		Right = 1,
		Top = 2,
		Bottom = 3
	};
private:
	QRectF rect; 
	PositionFlag posflag;
	QLine line;
public:
	BorderHandle()
	{
		setFlag(QGraphicsObject::ItemIgnoresTransformations, false);
		setFlag(QGraphicsObject::ItemIgnoresParentOpacity, true);
	}
	~BorderHandle()
	{

	}
	void setPositionFlag(PositionFlag pos)
	{
		this->posflag = pos;
	}
	void setBoundingRect(QRectF rect)
	{
		this->rect = rect;
	}
	void setBoundingRect(int xLeft, int yTop, int width, int height)
	{
		this->rect = QRectF(xLeft, yTop, width, height);
	}
	QRectF boundingRect() const
	{
		return rect;
	}
	void setLine(int x1, int y1, int x2, int y2)
	{
		line = QLine(x1, y1, x2, y2);
	}
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* wid)
	{
		painter->save();
		QPen pen = QPen(Qt::black);
		double scaleFactor = painter->matrix().m11();
		pen.setWidth(3 / scaleFactor); //线段保持原来的线宽
		painter->setPen(pen);
		painter->drawLine(line);
		painter->restore();
	}
};

