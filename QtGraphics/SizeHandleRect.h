#pragma once
#include <qgraphicsitem.h>
#include <QPen>
#include <QPainter>
class SizeHandleRect :public QGraphicsRectItem
{
public:
	SizeHandleRect(QRectF rect) :QGraphicsRectItem(rect)
	{

	}
protected:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr)
	{
		QPen pen;
		double scaleFactorX = painter->matrix().m11();
		double scaleFactorY = painter->matrix().m22();
		pen.setWidth(pen.width() / scaleFactorX); //线段保持原来的线宽

	//    线宽也可以设置为固定值
	//    pen.setWidth(0.5);
		painter->setPen(pen);
		painter->drawRect(sceneBoundingRect());

		pen.setColor(QColor(243, 106, 36));
		//pen.setWidth(10/ scaleFactor); //线段保持原来的线宽
		pen.setBrush(QBrush(QColor(243, 106, 36)));
		painter->setPen(pen);
		painter->setBrush(QBrush(QColor(243, 106, 36)));
		painter->drawEllipse(sceneBoundingRect().topLeft(), 4/ scaleFactorX, 4/ scaleFactorY);
		painter->drawEllipse(sceneBoundingRect().topRight(), 4 / scaleFactorX, 4 / scaleFactorY);
		painter->drawEllipse(sceneBoundingRect().bottomLeft(), 4 / scaleFactorX, 4 / scaleFactorY);
		painter->drawEllipse(sceneBoundingRect().bottomRight(), 4 / scaleFactorX, 4 / scaleFactorY);

		painter->setBrush(QBrush(QColor(255, 255, 255)));

		painter->drawEllipse(sceneBoundingRect().topLeft(), 2 / scaleFactorX, 2 / scaleFactorY);
		painter->drawEllipse(sceneBoundingRect().topRight(), 2 / scaleFactorX, 2 / scaleFactorY);
		painter->drawEllipse(sceneBoundingRect().bottomLeft(), 2 / scaleFactorX, 2 / scaleFactorY);
		painter->drawEllipse(sceneBoundingRect().bottomRight(), 2 / scaleFactorX, 2 / scaleFactorY);
		
		auto opacity = 0.5 * (scaleFactorX - 0.3);
		opacity = std::max(0.01, opacity);
		//setOpacity(opacity);

		qreal left = sceneBoundingRect().left();
		qreal right = sceneBoundingRect().right();
		qreal top = sceneBoundingRect().top();
		qreal bottom = sceneBoundingRect().bottom();

		painter->drawLine(left, top, left, bottom);

		painter->drawText(sceneBoundingRect().topLeft(),QString::number(left));

		painter->drawText(sceneBoundingRect().topRight(), QString::number(right));

		painter->drawText(sceneBoundingRect().bottomLeft(), QString::number(top));

		painter->drawText(sceneBoundingRect().bottomRight(), QString::number(bottom));
	}
public:
	enum Border {None,TopLeft,TopRight,BottomLeft,BottomRight,Left,Right,Top,Bottom};

	Border testBorderHit(QPointF pos)
	{
		qreal posX = pos.x();
		qreal posY = pos.y();

		qreal left = sceneBoundingRect().left();
		qreal right  = sceneBoundingRect().right();
		qreal top    = sceneBoundingRect().top();
		qreal bottom = sceneBoundingRect().bottom();

		if (posX >= left-1 && posX <= left + 1)
		{		
			if ((posX - left) * (posX - left) + (posY - top) * (posY - top) < 16)
			{
				return Border::TopLeft;
			}
			else if ((posX - left) * (posX - left) + (posY - bottom) * (posY - bottom) < 16)
			{
				return Border::BottomLeft;
			}
			else if (posY >= top + 1.8 && posY <= bottom - 1.8)
			{
				return Border::Left;
			}
		}

		else if (posX >= right-1 && posX <= right+1)
		{
			if ((posX - right) * (posX - right) + (posY - top) * (posY - top) < 16)
			{
				return Border::TopRight;
			}
			else if ((posX - right) * (posX - right) + (posY - bottom) * (posY - bottom) < 16)
			{
				return Border::BottomRight;
			}
			else if (posY >= top + 1.8 && posY <= bottom - 1.8)
			{
				return Border::Right;
			}
		}

		else if (posY >= top-1  && posY <= top +1)
		{
			if (posX >= left + 1.8 && posX <= right - 1.8)
			{
				return Border::Top;
			}
		}

		else if (posY >= bottom-1 && posY <= bottom+1)
		{
			if (posX >= left + 1.8 && posX <= right - 1.8)
			{
				return Border::Bottom;
			}
		}
		return Border::None;
	}
};

