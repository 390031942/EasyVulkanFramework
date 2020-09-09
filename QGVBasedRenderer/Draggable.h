#pragma once
#include <QGraphicsObject>
#include <QPainter>
class AbstractDraggable :public QGraphicsObject
{
	Q_OBJECT
private:

signals:
	void dragged();
public:
	AbstractDraggable();
	~AbstractDraggable();

	QVariant itemChange(GraphicsItemChange change, const QVariant& value);
	QRectF boundingRect() const
	{
		return QRectF(0, 0, 30, 30);
	}

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* wid)
	{
		painter->save();
		painter->drawRect(boundingRect());
		painter->restore();
	}
};

