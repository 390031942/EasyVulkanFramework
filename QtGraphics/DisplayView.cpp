#include "DisplayView.h"

void DisplayView::mouseMoveEvent(QMouseEvent* event)
{
	QGraphicsView::mouseMoveEvent(event);
	if (m_bMouseTranslate) {
		QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
		translate(mouseDelta);
	}

	m_lastMousePos = event->pos();
	Handle::injectMouseMove(event->pos());
}
void DisplayView::mousePressEvent(QMouseEvent* event)
{
	QGraphicsView::mousePressEvent(event);
	if (event->button() == m_translateButton) {
		// 当光标底下没有 item 时，才能移
		m_bMouseTranslate = true;
		m_lastMousePos = event->pos();
	}
	if (event->button() == m_selectButton)
	{
		Handle::injectMousePressed(event->pos());
	}
}
void DisplayView::wheelEvent(QWheelEvent* e)
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

void DisplayView::zoomIn()
{
	zoom(1 + m_zoomDelta);
}

void DisplayView::zoom(float scaleFactor)
{
	// 防止过小或过大
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;
	scale(scaleFactor, scaleFactor);
	m_scale *= scaleFactor;
}

void DisplayView::zoomOut()
{
	zoom(1 - m_zoomDelta);
}
void DisplayView::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == m_translateButton)
		m_bMouseTranslate = false;

	QGraphicsView::mouseReleaseEvent(event);
	Handle::injectMouseRelease(event->pos());
}

void DisplayView::mouseDoubleClickEvent(QMouseEvent* event)
{
	QGraphicsItem* item = itemAt(event->pos());
	QGraphicsView::mouseDoubleClickEvent(event);
}