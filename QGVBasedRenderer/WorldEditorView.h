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

	//��������
	QPixmap bufferPixmap;
	//��ǰ����
	QPixmap currentPixmap;
	//�������ʼ��λ��
	QPoint  beginPoint;
	//�����������λ��
	QPoint  endPoint;
	//��־�Ƿ񵥻�
	bool    isMousePressed;
	//�Ƿ���˫��
	bool    isDoubleClick; 
	//�Ƿ���˫�ػ���
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

	/*���� �Ŵ�����ƽ�����*/
	void				zoomIn();
	void				zoom(float scaleFactor);
	void				zoomOut();
	void				translate(QPointF delta)
	{
		// ���ݵ�ǰ zoom ����ƽ����
		delta *= scaleFactor;
		delta *= translateSpeed;
		// view ��������µĵ���Ϊê������λ scene
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		QPoint newCenter(VIEW_WIDTH / 2 - delta.x(), VIEW_HEIGHT / 2 - delta.y());
		centerOn(mapToScene(newCenter));

		// scene �� view �����ĵ���Ϊê��
		setTransformationAnchor(QGraphicsView::AnchorViewCenter);
	}

	/*���� ������� ����*/
	Qt::MouseButton		translateButton;  // ƽ�ư�ť
	Qt::MouseButton		selectButton;     // ѡ��ť
	qreal				translateSpeed;   // ƽ���ٶ�
	qreal				zoomDelta;        // ���ŵ�����
	bool				bMouseTranslate;  // ƽ�Ʊ�ʶ
	QPoint				lastMousePos;     // �������µ�λ��
	qreal				scaleFactor;      // ����ϵ��

	HandleManager*      handleMgr;

	QTimer*             timer;
};
