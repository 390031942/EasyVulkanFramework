#pragma once
#include "Draggable.h"
#include "CornerHandle.h"
#include "BorderHandle.h"

#include <QGraphicsScene>
#include "Sprite.h"
class HandleManager:public QObject
{
	Q_OBJECT

private:
	CornerHandle* cornerHandle;
	BorderHandle* borderHandle;

	Sprite* attechedObject;
	const int cornerHandleCount = 4;
	const int borderHandleCount = 4;

public:
	void init(QGraphicsScene * scene)
	{
		cornerHandle = new CornerHandle[cornerHandleCount];
		borderHandle = new BorderHandle[borderHandleCount];

		for (int i = 0; i < cornerHandleCount; i++)
		{
			scene->addItem(&(cornerHandle[i]));
			cornerHandle[i].setZValue(100);
			cornerHandle[i].hide();
		}

		for (int i = 0; i < borderHandleCount; i++)
		{
			scene->addItem(&(borderHandle[i]));
			borderHandle[i].setZValue(100);
			borderHandle[i].setPositionFlag((BorderHandle::PositionFlag)i);
			borderHandle[i].hide();
		}
	}
	void attachObject(QGraphicsItem* object)
	{
		for (int i = 0; i < cornerHandleCount; i++)
		{
			cornerHandle[i].show();
		}

		for (int i = 0; i < borderHandleCount; i++)
		{
			borderHandle[i].show();
		}
		
		QRectF rect = object->sceneBoundingRect();
		cornerHandle[0].setPos(rect.topLeft()     + QPointF(-10, -20));
		cornerHandle[1].setPos(rect.bottomLeft()  + QPointF(-10, 0));
		cornerHandle[2].setPos(rect.bottomRight() + QPointF(+10, 0));
		cornerHandle[3].setPos(rect.topRight()    + QPointF(+10, -20));

		borderHandle[0].setPos(0.5 * (cornerHandle[0].pos() + cornerHandle[1].pos()));
		borderHandle[1].setPos(0.5 * (cornerHandle[2].pos() + cornerHandle[3].pos()));
		borderHandle[2].setPos(0.5 * (cornerHandle[0].pos() + cornerHandle[3].pos()));
		borderHandle[3].setPos(0.5 * (cornerHandle[1].pos() + cornerHandle[2].pos()));

		int height = rect.height();
		borderHandle[0].setLine(0, cornerHandle[0].pos().y(), 0, cornerHandle[1].pos().y());
		attechedObject = dynamic_cast<Sprite*>(object);
		QObject::connect(attechedObject, &AbstractDraggable::dragged, this, &HandleManager::updatePosAfterAttachedObjectDragged);
	}
	
	void disattachObject()
	{
		QObject::disconnect(attechedObject, &AbstractDraggable::dragged, this, &HandleManager::updatePosAfterAttachedObjectDragged);

		attechedObject = NULL;

		for (int i = 0; i < cornerHandleCount; i++)
		{
			cornerHandle[i].hide();
		}

		for (int i = 0; i < borderHandleCount; i++)
		{
			borderHandle[i].hide();
		}
	}

	void updatePosAfterAttachedObjectDragged()
	{
		assert(attechedObject != NULL);

		QRectF rect = attechedObject->sceneBoundingRect();
		cornerHandle[0].setPos(rect.topLeft() + QPointF(-10, -20));
		cornerHandle[1].setPos(rect.bottomLeft() + QPointF(-10, 0));
		cornerHandle[2].setPos(rect.bottomRight() + QPointF(+10, 0));
		cornerHandle[3].setPos(rect.topRight() + QPointF(+10, -20));

		borderHandle[0].setPos(0.5 * (cornerHandle[0].pos() + cornerHandle[1].pos()));
		borderHandle[1].setPos(0.5 * (cornerHandle[2].pos() + cornerHandle[3].pos()));
		borderHandle[2].setPos(0.5 * (cornerHandle[0].pos() + cornerHandle[3].pos()));
		borderHandle[3].setPos(0.5 * (cornerHandle[1].pos() + cornerHandle[2].pos()));
	}

	void updatePosAfterCornerObjectDragged()
	{

	}

	void updatePosAfterBorderObjectDragged()
	{

	}
};

