#pragma once

#include <QGraphicsView>
#include <QMouseEvent>
#include <QTimer>

#include "HandleManager.h"
#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()

class WorldEditorView : public QGraphicsView
{
	Q_OBJECT

public:
	WorldEditorView(QWidget *parent = nullptr);
	~WorldEditorView();

	//辅助画布
	QPixmap bufferPixmap;
	//当前画布
	QPixmap currentPixmap;
	//鼠标点击开始的位置
	QPoint  beginPoint;
	//鼠标点击结束的位置
	QPoint  endPoint;
	//标志是否单击
	bool    isMousePressed;
	//是否是双击
	bool    isDoubleClick; 
	//是否是双重缓冲
	bool    doubleBufferMode;

	void    wheelEvent(QWheelEvent* e)
	{
		QPoint scrollAmount = e->angleDelta();
		if (scrollAmount.y() > 0)
		{
			setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
			zoomIn();
			setTransformationAnchor(QGraphicsView::AnchorViewCenter);
		}
		else
		{
			zoomOut();
		}
	}
	void    mousePressEvent(QMouseEvent* event);
	void    mouseDoubleClickEvent(QMouseEvent* event);
	void    mouseMoveEvent(QMouseEvent* event);
	void    mouseReleaseEvent(QMouseEvent* event);
	void    paintEvent(QPaintEvent* event);

	/*场景 放大和鼠标平移相关*/
	void				zoomIn();
	void				zoom(float scaleFactor);
	void				zoomOut();
	void				translate(QPointF delta)
	{
		// 根据当前 zoom 缩放平移数
		delta *= scaleFactor;
		delta *= translateSpeed;
		// view 根据鼠标下的点作为锚点来定位 scene
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		QPoint newCenter(VIEW_WIDTH / 2 - delta.x(), VIEW_HEIGHT / 2 - delta.y());
		centerOn(mapToScene(newCenter));

		// scene 在 view 的中心点作为锚点
		setTransformationAnchor(QGraphicsView::AnchorViewCenter);
	}

	/*场景 操作相关 变量*/
	Qt::MouseButton		translateButton;  // 平移按钮
	Qt::MouseButton		selectButton;     // 选择按钮
	qreal				translateSpeed;   // 平移速度
	qreal				zoomDelta;        // 缩放的增量
	bool				bMouseTranslate;  // 平移标识
	QPoint				lastMousePos;     // 鼠标最后按下的位置
	qreal				scaleFactor;      // 缩放系数

	HandleManager*      handleMgr;

	QTimer*             timer;
};
