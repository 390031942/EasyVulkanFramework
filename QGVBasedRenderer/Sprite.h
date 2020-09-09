#pragma once

#include "Draggable.h"

class Sprite:public AbstractDraggable
{
private:
	QImage image;
public:
	Sprite()
	{
		setFlags(flags() | QGraphicsItem::ItemIsSelectable);
	}
	~Sprite()
	{

	}
	bool loadSprite(QString path)
	{
		if (!image.load(path))
			return false;
		return true;
	}

	QRectF boundingRect() const
	{
		return QRectF(0, 0, image.width(), image.height());
	}
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* wid)
	{
		painter->save();
		if(isSelected())
			painter->setOpacity(0.8f);
		else
			painter->setOpacity(1.0f);

		painter->drawImage(boundingRect(), image);
		painter->restore();
	}
};

