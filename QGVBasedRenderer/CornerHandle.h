#pragma once
#include "Draggable.h"


class CornerHandle :public AbstractDraggable
{
public:
	CornerHandle()
	{
		setFlag(QGraphicsObject::ItemIgnoresTransformations, false);
		setFlag(QGraphicsObject::ItemIgnoresParentOpacity, true);
	}
	~CornerHandle()
	{

	}
	enum Position {
		TopLeft = 1,
		TopRight = 2,
		BottomLeft =3,
		BottomRight =5
	};

	QRectF boundingRect() const
	{
		return QRectF(-5, 0, 10, 10);

	}

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* wid)
	{
		painter->save();
		QPen pen = QPen(Qt::blue);
		double scaleFactor = 1.0f/painter->matrix().m11();
		pen.setWidth(3);
		painter->scale(scaleFactor,scaleFactor);
		painter->setPen(pen);
		painter->drawEllipse(boundingRect());
		painter->restore();

	}

};

