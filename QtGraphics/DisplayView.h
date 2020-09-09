#pragma once
#include <qgraphicsview.h>
#include <QMouseEvent>
#include <QTimer>
#include "Handle.h"
#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()

class DisplayView :public QGraphicsView
{
	Q_OBJECT
public:
	DisplayView(QWidget* parent):
		QGraphicsView(parent)
	{
		m_selectButton = (Qt::LeftButton);
		m_translateButton = (Qt::RightButton);
		m_scale = (0.5);
		m_zoomDelta = (0.1);
		m_translateSpeed = (1.0);
		m_bMouseTranslate = (false);

		setStyleSheet("background-color:#cfcfcf;");
		setScene(new QGraphicsScene);
		auto pScene = scene();
		pScene->addRect(0, 0, 10, 10);
		pScene->addRect(20, 20, 5, 20);

		/*取消水平和垂直 滚动条*/
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		/*设置场景边框*/
		setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);

		/*设置鼠标默认指针*/
		setCursor(Qt::ArrowCursor);

		/*抗锯齿*/
		setRenderHint(QPainter::Antialiasing);
		setMouseTracking(true);

		scale(0.5, 0.5);

		QTimer* timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, [=]() {this->update(); });
		timer->start(1000 / 30.0f);

		Handle::init(this);
	}
protected:
	void translate(QPointF delta)
	{
		//((QWidget*)parent())->setWindowTitle(QString("X=")+QString::number(delta.x())+" Y="+QString::number(delta.y()));
		// 根据当前 zoom 缩放平移数
		delta *= m_scale;
		delta *= m_translateSpeed;
		// view 根据鼠标下的点作为锚点来定位 scene
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		QPoint newCenter(VIEW_WIDTH / 2 - delta.x(), VIEW_HEIGHT / 2 - delta.y());
		centerOn(mapToScene(newCenter));

		// scene 在 view 的中心点作为锚点
		setTransformationAnchor(QGraphicsView::AnchorViewCenter);
	}

	/*窗口事件 重写*/
	void				wheelEvent(QWheelEvent* e);
	void				mousePressEvent(QMouseEvent* event);
	void				mouseMoveEvent(QMouseEvent* event);
	void				mouseReleaseEvent(QMouseEvent* event);
	void				mouseDoubleClickEvent(QMouseEvent* event);
	void				keyPressEvent(QKeyEvent* event)
	{
		if (event->key() == 16777249)
			Handle::injectCtrlPressed();
	}
	void keyReleaseEvent(QKeyEvent* event)
	{
		if (event->key() == 16777249)
			Handle::injectCtrlReleased();
	}
private:
	/*场景 放大和鼠标平移相关*/
	void				zoomIn();
	void				zoom(float scaleFactor);
	void				zoomOut();
private:
	/*场景 操作相关 变量*/
	Qt::MouseButton		m_translateButton;  // 平移按钮
	Qt::MouseButton		m_selectButton;     // 选择按钮
	qreal				m_translateSpeed;   // 平移速度
	qreal				m_zoomDelta;        // 缩放的增量
	bool				m_bMouseTranslate;  // 平移标识
	QPoint				m_lastMousePos;     // 鼠标最后按下的位置
	qreal				m_scale;		    // 缩放值
};
