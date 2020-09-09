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

		/*ȡ��ˮƽ�ʹ�ֱ ������*/
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		/*���ó����߿�*/
		setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);

		/*�������Ĭ��ָ��*/
		setCursor(Qt::ArrowCursor);

		/*�����*/
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
		// ���ݵ�ǰ zoom ����ƽ����
		delta *= m_scale;
		delta *= m_translateSpeed;
		// view ��������µĵ���Ϊê������λ scene
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		QPoint newCenter(VIEW_WIDTH / 2 - delta.x(), VIEW_HEIGHT / 2 - delta.y());
		centerOn(mapToScene(newCenter));

		// scene �� view �����ĵ���Ϊê��
		setTransformationAnchor(QGraphicsView::AnchorViewCenter);
	}

	/*�����¼� ��д*/
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
	/*���� �Ŵ�����ƽ�����*/
	void				zoomIn();
	void				zoom(float scaleFactor);
	void				zoomOut();
private:
	/*���� ������� ����*/
	Qt::MouseButton		m_translateButton;  // ƽ�ư�ť
	Qt::MouseButton		m_selectButton;     // ѡ��ť
	qreal				m_translateSpeed;   // ƽ���ٶ�
	qreal				m_zoomDelta;        // ���ŵ�����
	bool				m_bMouseTranslate;  // ƽ�Ʊ�ʶ
	QPoint				m_lastMousePos;     // �������µ�λ��
	qreal				m_scale;		    // ����ֵ
};
