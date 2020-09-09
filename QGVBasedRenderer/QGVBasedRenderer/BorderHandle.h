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
	PositionFlag pos;
public:
	BorderHandle()
	{
		setFlag(QGraphicsObject::ItemIgnoresTransformations, true);
		setFlag(QGraphicsObject::ItemIgnoresParentOpacity, true);
	}
	~BorderHandle()
	{

	}
	void setPositionFlag(PositionFlag pos)
	{
		this->pos = pos;
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

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* wid)
	{
		painter->save();
		QPen pen = QPen(Qt::black);
		pen.setWidth(2);
		painter->setPen(pen);

		if (pos == Left || pos ==Right)
		{
			painter->drawLine(0, rect.top(), 0, rect.bottom());
		}
		else if (pos == Top || pos == Right)
		{
			painter->drawLine(rect.left(),0,rect.right(),0);
		}
		painter->restore();
	}
};

