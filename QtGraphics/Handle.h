#pragma once
#include <qgraphicsitem.h>
#include "Sprite.h"
#include "SizeHandleRect.h"
#include <QtWidgets\qmessagebox.h>

class Handle
{
public:
	static void attachItem(QGraphicsItem* item)
	{
		lastPosition = item->pos();
		if (!ctrlPressed)
		{
			//attachBorder(item);
			items.clear();
			items.push_back(item);
			//view->scene()->addRect(item->sceneBoundingRect());
			group_Handle_Position->setPos(item->sceneBoundingRect().center().x(), item->sceneBoundingRect().center().y());
		}
		else
		{
			if (!checkAttachItemExists(item))
			{
				items.push_back(item);
				QPointF pt = group_Handle_Position->scenePos();

				pt = (pt * (items.size() - 1) + item->sceneBoundingRect().center()) / (items.size());
				group_Handle_Position->setPos(pt);
			}			
		}
		group_Handle_Position->show();
	}

	static bool checkAttachItemExists(QGraphicsItem* item)
	{
		for (int i = 0; i < items.size(); i++)
			if (item == items[i])
				return true;
		return false;
	}
	static void attachItems(std::vector<QGraphicsItem*> items)
	{
		Handle::items = items;
		QPointF position = calculatorPosition();
		lastPosition = position;

		group_Handle_Position->setPos(lastPosition);
		group_Handle_Position->show();
	}
	static void disAttach()
	{
		group_Handle_Position->hide();
		items.clear();
	}

	static QPointF calculatorPosition()
	{
		QPointF result;
		int size = items.size();
		for (int i = 0; i < size; i++)
		{
			result += (items[i]->scenePos() / size);
		}
		return result;
	}
	static void updatePosition(QPointF pos,Qt::Axis axis) {
		int size = items.size();	
		QPointF deltaPos = view->mapToScene(pos.x(), pos.y()) - view->mapToScene(lastMousePosition.x(), lastMousePosition.y());

		if (axis == Qt::Axis::XAxis)
		{
			deltaPos.setY(0);
		}
		else if (axis == Qt::Axis::YAxis)
		{
			deltaPos.setX(0);
		}
		QPointF tpos = group_Handle_Position->scenePos();
		tpos += deltaPos;
		group_Handle_Position->setPos(tpos);

		for (int i = 0; i < size; i++)
		{
			QPointF tpos = items[i]->scenePos();
			tpos += deltaPos;
			items[i]->setPos(tpos);
		}
	}

	static void updateRotation(float deltaAngle) {
		int size = items.size();
		for (int i = 0; i < size; i++)
		{
			QTransform transform = items[i]->transform();
			transform.rotate(deltaAngle, Qt::ZAxis);
			items[i]->setTransform(transform);
		}
	}
	static void updateScale(QPointF pos) {
		int size = items.size();

		QRectF rect = rectItem->sceneBoundingRect();

		for (int i = 0; i < size; i++)
		{
			if (border == SizeHandleRect::Border::Left)
			{
				qreal deltaX = (view->mapToScene(pos.x(),pos.y()) - rect.topLeft()).x();
				rect.setWidth(rect.width() + deltaX);
				items[0]->setPos(items[0]->scenePos() - QPointF(deltaX, 0));

				QTransform transform = rectItem->transform();
				transform = transform * transform.scale(rect.width()+deltaX,1);
				items[0]->setTransform(transform);
			}
			else if (border == SizeHandleRect::Border::Right)
			{
				qreal deltaX = (view->mapToScene(pos.x(), pos.y()) - view->mapToScene(rect.right(), rect.top())).x();
				rect.setWidth(rect.width() + deltaX);
			}
			else if (border == SizeHandleRect::Border::Top)
			{
				view->setCursor(Qt::CursorShape::SizeVerCursor);
				hoverStatus = HOVER_BORDER_TOP;
				hoveredItem = rectItem;
			}
			else if (border == SizeHandleRect::Border::Bottom)
			{
				view->setCursor(Qt::CursorShape::SizeVerCursor);
				hoverStatus = HOVER_BORDER_BOTTOM;
				hoveredItem = rectItem;
			}
			else if (border == SizeHandleRect::Border::TopLeft)
			{
				view->setCursor(Qt::CursorShape::SizeFDiagCursor);
				hoverStatus = HOVER_POINT_TOPLEFT;
				hoveredItem = rectItem;
			}
			else if (border == SizeHandleRect::Border::BottomRight)
			{
				view->setCursor(Qt::CursorShape::SizeFDiagCursor);
				hoverStatus = HOVER_POINT_BOTTOMRIGHT;
				hoveredItem = rectItem;
			}
			else if (border == SizeHandleRect::Border::BottomLeft)
			{
				view->setCursor(Qt::CursorShape::SizeBDiagCursor);
				hoverStatus = HOVER_POINT_BOTTOMLEFT;
				hoveredItem = rectItem;
			}
			else if (border == SizeHandleRect::Border::TopRight)
			{
				view->setCursor(Qt::CursorShape::SizeBDiagCursor);
				hoverStatus = HOVER_POINT_TOPRIGHT;
				hoveredItem = rectItem;
			}
		}
	}

	static const int STATUS_HANDLE_NONE = 1;
	static const int STATUS_HANDLE_POSITION = 2;
	static const int STATUS_HANDLE_ROTATION = 3;
	static const int STATUS_HANDLE_SCALE = 4;

	static const int HOVER_NONE = 0x10;
	static const int HOVER_POSITION_X = 0x11;
	static const int HOVER_POSITION_Y = 0x12;
	static const int HOVER_ROTATION = 0x13;
	static const int HOVER_SCALE = 0x14;

	static const int HOVER_BORDER_LEFT = 0x15;
	static const int HOVER_BORDER_RIGHT = 0x16;
	static const int HOVER_BORDER_TOP = 0x17;
	static const int HOVER_BORDER_BOTTOM = 0x18;

	static const int HOVER_POINT_TOPLEFT = 0x19;
	static const int HOVER_POINT_TOPRIGHT = 0x1a;
	static const int HOVER_POINT_BOTTOMLEFT = 0x1b;
	static const int HOVER_POINT_BOTTOMRIGHT = 0x1c;

	static void rotate(float angle)
	{
		float deltaAngle = angle - lastRotation;
		updateRotation(deltaAngle);
		lastRotation = angle;
	}
	static void scale(QSize scale)
	{

	}
	static void setPositionEnable()
	{
		status = STATUS_HANDLE_POSITION;
	}
	static void setRotationEnable()
	{
		status = STATUS_HANDLE_ROTATION;
	}
	static void setScaleEnable()
	{
		status = STATUS_HANDLE_SCALE;
	}
	static void setDisable()
	{
		status = STATUS_HANDLE_NONE;
	}

	static std::vector<QGraphicsItem*> items;

	static QPointF lastPosition;
	static QPointF currentPosition;
	static float lastRotation;
	static float currentRotation;
	static QSize currentScale;
	
	static QPointF lastMousePosition;
	static int status;

	static Sprite* sprite_Handle_PosX;
	static Sprite* sprite_Handle_PosY;
	static QGraphicsItemGroup* group_Handle_Position;

	static QGraphicsPathItem* sprite_Handle_Rotate_Inner;
	static QGraphicsPathItem* sprite_Handle_Rotate_Middle;
	static QGraphicsPathItem* sprite_Handle_Rotate_Outer;
	static QGraphicsItemGroup* group_Handle_Rotation;

	static QGraphicsLineItem* sprite_Border_Left_Outer;
	static QGraphicsLineItem* sprite_Border_Left_Middle;
	static QGraphicsLineItem* sprite_Border_Left_Inner;
	static QGraphicsItemGroup* group_Handle_Border_Left;

	static QGraphicsLineItem* sprite_Border_Right_Outer;
	static QGraphicsLineItem* sprite_Border_Right_Middle;
	static QGraphicsLineItem* sprite_Border_Right_Inner;
	static QGraphicsItemGroup* group_Handle_Border_Right;

	static QGraphicsLineItem* sprite_Border_Top_Outer;
	static QGraphicsLineItem* sprite_Border_Top_Middle;
	static QGraphicsLineItem* sprite_Border_Top_Inner;
	static QGraphicsItemGroup* group_Handle_Border_Top;

	static QGraphicsLineItem* sprite_Border_Bottom_Outer;
	static QGraphicsLineItem* sprite_Border_Bottom_Middle;
	static QGraphicsLineItem* sprite_Border_Bottom_Inner;
	static QGraphicsItemGroup* group_Handle_Border_Bottom;

	static QGraphicsPathItem* sprite_Point_TopLeft_Outer;
	static QGraphicsPathItem* sprite_Point_TopLeft_Middle;
	static QGraphicsPathItem* sprite_Point_TopLeft_Inner;
	static QGraphicsItemGroup* group_Point_TopLeft;

	static QGraphicsPathItem* sprite_Point_TopRight_Outer;
	static QGraphicsPathItem* sprite_Point_TopRight_Middle;
	static QGraphicsPathItem* sprite_Point_TopRight_Inner;
	static QGraphicsItemGroup* group_Point_TopRight;

	static QGraphicsPathItem* sprite_Point_BottomLeft_Outer;
	static QGraphicsPathItem* sprite_Point_BottomLeft_Middle;
	static QGraphicsPathItem* sprite_Point_BottomLeft_Inner;
	static QGraphicsItemGroup* group_Point_BottomLeft;

	static QGraphicsPathItem* sprite_Point_BottomRight_Outer;
	static QGraphicsPathItem* sprite_Point_BottomRight_Middle;
	static QGraphicsPathItem* sprite_Point_BottomRight_Inner;
	static QGraphicsItemGroup* group_Point_BottomRight;

	static QGraphicsView* view;
	static bool itemHovered;
	static bool itemPressed;
	static int  hoverStatus;
	static bool ctrlPressed;
	static SizeHandleRect* rectItem;
	static QGraphicsItem* hoveredItem;
	static SizeHandleRect::Border border;

	static void attachBorder(QGraphicsItem* item)
	{
		/*sprite_Border_Left_Outer  = new QGraphicsLineItem;
		sprite_Border_Left_Middle = new QGraphicsLineItem;;
		sprite_Border_Left_Inner  = new QGraphicsLineItem;;
		group_Handle_Border_Left  = new QGraphicsItemGroup;

		sprite_Border_Right_Outer = new QGraphicsLineItem;
		sprite_Border_Right_Middle = new QGraphicsLineItem;;
		sprite_Border_Right_Inner = new QGraphicsLineItem;;
		group_Handle_Border_Right = new QGraphicsItemGroup;

		sprite_Border_Top_Outer = new QGraphicsLineItem;
		sprite_Border_Top_Middle = new QGraphicsLineItem;;
		sprite_Border_Top_Inner = new QGraphicsLineItem;;
		group_Handle_Border_Top = new QGraphicsItemGroup;

		sprite_Border_Bottom_Outer = new QGraphicsLineItem;
		sprite_Border_Bottom_Middle = new QGraphicsLineItem;
		sprite_Border_Bottom_Inner = new QGraphicsLineItem;
		group_Handle_Border_Bottom = new QGraphicsItemGroup;

		sprite_Border_Left_Outer->setLine(QLine(item->sceneBoundingRect().left()-5, item->sceneBoundingRect().top()-5, item->sceneBoundingRect().left()-5,item->sceneBoundingRect().bottom()+5));
		sprite_Border_Left_Middle->setLine(QLine(item->sceneBoundingRect().left() - 4.5, item->sceneBoundingRect().top() - 5, item->sceneBoundingRect().left() - 4.5, item->sceneBoundingRect().bottom() + 5));
		sprite_Border_Left_Inner->setLine(QLine(item->sceneBoundingRect().left() - 4, item->sceneBoundingRect().top() - 5, item->sceneBoundingRect().left() - 4, item->sceneBoundingRect().bottom() + 5));

		QPen pen = QPen(QColor(62, 62, 66));
		pen.setWidth(1);
		sprite_Border_Left_Outer->setPen(pen);
		pen = QPen(QColor(243, 106, 36));
		pen.setWidth(2);
		sprite_Border_Left_Middle->setPen(pen);
		pen = QPen(QColor(62, 62, 66));
		sprite_Border_Left_Inner->setPen(pen);

		group_Handle_Border_Left->addToGroup(sprite_Border_Left_Outer);
		group_Handle_Border_Left->addToGroup(sprite_Border_Left_Middle);
		group_Handle_Border_Left->addToGroup(sprite_Border_Left_Inner);
		//group_Handle_Border_Left->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		group_Handle_Border_Left->show();

		view->scene()->addItem(group_Handle_Border_Left);*/

		qreal left = item->sceneBoundingRect().left()-10;
		qreal right = item->sceneBoundingRect().right()+10;
		qreal top = item->sceneBoundingRect().top()-10;
		qreal bottom = item->sceneBoundingRect().bottom()+10;
		QRect rect = QRect(QPoint(left,top), QPoint(right,bottom));

		rectItem = new SizeHandleRect(rect);
		rectItem->setPos(item->scenePos());
		//rectItem->contains();
		//rectItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);

		view->scene()->addItem(rectItem);

	}
	static void init(QGraphicsView* view)
	{
		Handle::view = view;
		QTransform transform;
		QPainterPath path;
		sprite_Handle_PosX = new Sprite;
		//sprite_Handle_PosX->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		sprite_Handle_PosX->SetSprite("PositionHandleArrow.png");
		sprite_Handle_PosX->setScale(0.2f);
		sprite_Handle_PosX->SetColor(255, 0, 0);
		sprite_Handle_PosX->moveBy(sprite_Handle_PosX->sceneBoundingRect().width() / 2.0, 0);
		sprite_Handle_PosX->setObjectName("X");
		//sprite_Handle_PosX->hide();
		group_Handle_Position = new QGraphicsItemGroup;
		group_Handle_Position->addToGroup(sprite_Handle_PosX);

		sprite_Handle_PosY = new Sprite;
		//sprite_Handle_PosY->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		sprite_Handle_PosY->SetSprite("PositionHandleArrow.png");
		sprite_Handle_PosY->SetColor(0, 255, 0);
		sprite_Handle_PosY->setScale(0.2f);
		sprite_Handle_PosY->setObjectName("Y");
		transform = sprite_Handle_PosY->transform();
		transform.translate(-sprite_Handle_PosX->sceneBoundingRect().width() / 2, -sprite_Handle_PosX->sceneBoundingRect().height() *4.1);
		//transform.rotate(-90, Qt::ZAxis);
		sprite_Handle_PosY->setPos(0, -sprite_Handle_PosX->sceneBoundingRect().height() * 4.1);
		sprite_Handle_PosY->setRotation(-90);
		group_Handle_Position->addToGroup(sprite_Handle_PosY);
		group_Handle_Position->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		group_Handle_Position->hide();
		group_Handle_Position->setZValue(10);

		view->scene()->addItem(group_Handle_Position);
		sprite_Handle_Rotate_Inner = new QGraphicsPathItem;
		sprite_Handle_Rotate_Inner->setPen(QPen(QBrush(QColor(62, 62, 66)), 2));
		path.addEllipse(QPointF(0, 0), 30, 30);
		sprite_Handle_Rotate_Inner->setPath(path);
		sprite_Handle_Rotate_Inner->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		sprite_Handle_Rotate_Inner->hide();

		sprite_Handle_Rotate_Middle = new QGraphicsPathItem;
		sprite_Handle_Rotate_Middle->setPen(QPen(QBrush(QColor(243, 106, 36)), 3));
		path.addEllipse(QPointF(0, 0), 28, 28);
		sprite_Handle_Rotate_Middle->setPath(path);
		sprite_Handle_Rotate_Middle->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		sprite_Handle_Rotate_Middle->hide();

		sprite_Handle_Rotate_Outer = new QGraphicsPathItem;
		sprite_Handle_Rotate_Outer->setPen(QPen(QBrush(QColor(62, 62, 66)), 2));
		path.addEllipse(QPointF(0, 0), 32, 32);
		sprite_Handle_Rotate_Outer->setPath(path);
		sprite_Handle_Rotate_Outer->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		sprite_Handle_Rotate_Outer->hide();

		status = STATUS_HANDLE_POSITION;
		hoverStatus = HOVER_NONE;
		itemPressed = false;
		itemHovered = false;
		ctrlPressed = false;

		qreal left = -10;
		qreal right =10;
		qreal top = -10;
		qreal bottom=10;
		QRect rect = QRect(QPoint(left, top), QPoint(right, bottom));

		rectItem = new SizeHandleRect(rect);

		//rectItem->setPos(item->scenePos()); 
		//rectItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);

		view->scene()->addItem(rectItem);
	}
	static void injectCtrlPressed()
	{
		ctrlPressed = true;
	}
	static void injectCtrlReleased()
	{
		ctrlPressed = false;
	}
	static void injectMousePressed(const QPointF pos)
	{
		auto item = view->itemAt(pos.x(), pos.y());

		switch (hoverStatus)
		{
		case HOVER_NONE:	
			if (item&&!isHandleItem(item))
				attachItem(item);
			else
				disAttach();
			break;
		default:
			itemPressed = true;
		}
	}
	static bool isHandleItem(QGraphicsItem* item)
	{
		return item == group_Handle_Position;
	}
	static void injectMouseMove(const QPointF pos)
	{
		auto currentPos = pos;
		auto item = view->itemAt(pos.x(), pos.y());
		auto scenePos = view->mapToScene(pos.x(), pos.y());

		//((QWidget*)(view->parent()))->setWindowTitle(QString("X=") + QString::number(scenePos.x()) + " " + "Y=" + QString::number(scenePos.y()));
		//((QWidget*)(view->parent()))->setWindowTitle();
		if (itemPressed == false && hoverStatus != HOVER_NONE)
		{
			hoveredItem->setOpacity(1);
			hoverStatus = HOVER_NONE;
			hoveredItem = NULL;
		}
		if (itemPressed)
		{
			if (hoveredItem == sprite_Handle_PosY)
			{
				updatePosition(pos,Qt::Axis::YAxis);
			}
			else if (hoveredItem == sprite_Handle_PosX)
			{
				updatePosition(pos, Qt::Axis::XAxis);
			}
			else if(border != SizeHandleRect::Border::None)
			{
				//updateScale(pos);
			}
		}
		else
		{

			border = rectItem->testBorderHit(view->mapToScene(pos.x(), pos.y()));
			if (border == SizeHandleRect::Border::Left) 
			{
				view->setCursor(Qt::CursorShape::SizeHorCursor);
				hoverStatus = HOVER_BORDER_LEFT;
				hoveredItem = rectItem;

				QRectF rect = rectItem->sceneBoundingRect();
				qreal deltaX = -1;
				rect.setWidth(rect.width() + deltaX);

				QTransform transform = rectItem->transform();
				transform.scale((rect.width() + deltaX)/rect.width(), 1);

				rectItem->setTransform(transform);
				rectItem->setPos(rectItem->scenePos() - QPointF(deltaX / 2, 0));
			}
			else if (border == SizeHandleRect::Border::Right)
			{
				view->setCursor(Qt::CursorShape::SizeHorCursor);
				hoverStatus = HOVER_BORDER_RIGHT;
				hoveredItem = rectItem;

				QRectF rect = rectItem->sceneBoundingRect();
				static const qreal originRight = rect.right();
				qreal deltaX = 1;
				rect.setWidth(rect.width() + deltaX);
				auto pos = rectItem->scenePos();

				QTransform transform = rectItem->transform();
				transform.scale((rect.width() + deltaX) / rect.width(), 1);
				rectItem->setTransform(transform);

				qreal offsetX = rectItem->sceneBoundingRect().right()-originRight;
				((QWidget*)(view->parent()))->setWindowTitle((QString("X=") + QString::number(scenePos.x()) + " " + "Y=" + QString::number(scenePos.y())+" "+QString::number(rectItem->sceneBoundingRect().right())));
				rectItem->setPos(pos - QPointF(offsetX,0));
			}
			else if (border == SizeHandleRect::Border::Top)
			{
				view->setCursor(Qt::CursorShape::SizeVerCursor);
				hoverStatus = HOVER_BORDER_TOP;
				hoveredItem = rectItem;
			}
			else if (border == SizeHandleRect::Border::Bottom)
			{
				view->setCursor(Qt::CursorShape::SizeVerCursor);
				hoverStatus = HOVER_BORDER_BOTTOM;
				hoveredItem = rectItem;
			}
			else if (border == SizeHandleRect::Border::TopLeft)
			{
				view->setCursor(Qt::CursorShape::SizeFDiagCursor);
				hoverStatus = HOVER_POINT_TOPLEFT;
				hoveredItem = rectItem;
			}
			else if (border == SizeHandleRect::Border::BottomRight)
			{
				view->setCursor(Qt::CursorShape::SizeFDiagCursor);
				hoverStatus = HOVER_POINT_BOTTOMRIGHT;
				hoveredItem = rectItem;
			}
			else if (border == SizeHandleRect::Border::BottomLeft)
			{
				view->setCursor(Qt::CursorShape::SizeBDiagCursor);
				hoverStatus = HOVER_POINT_BOTTOMLEFT;
				hoveredItem = rectItem;
			}
			else if (border == SizeHandleRect::Border::TopRight)
			{
				view->setCursor(Qt::CursorShape::SizeBDiagCursor);
				hoverStatus = HOVER_POINT_TOPRIGHT;
				hoveredItem = rectItem;
			}
			else
			{
				view->setCursor(Qt::CursorShape::ArrowCursor);
				switch (status)
				{
				case STATUS_HANDLE_NONE:
					return;
				case STATUS_HANDLE_POSITION:
					if (item == sprite_Handle_PosX)
					{
						hoverStatus = HOVER_POSITION_X;
						hoveredItem = item;
						item->setOpacity(0.5);
						break;
					}
					else if (item == sprite_Handle_PosY)
					{
						hoverStatus = HOVER_POSITION_Y;
						hoveredItem = item;
						item->setOpacity(0.5);
						break;
					}
					break;
				case STATUS_HANDLE_ROTATION:

					break;
				default:
					break;
				}
			}
		}	
		lastMousePosition = currentPos;
	}
	static void injectMouseRelease(const QPointF pos)
	{
		itemPressed = false;
	}
};