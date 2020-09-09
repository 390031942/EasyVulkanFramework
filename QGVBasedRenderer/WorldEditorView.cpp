#include "WorldEditorView.h"

WorldEditorView::WorldEditorView(QWidget *parent)
	: QGraphicsView(parent), isMousePressed(false), isDoubleClick(false),doubleBufferMode(true),
	selectButton(Qt::LeftButton),
	translateButton(Qt::RightButton),
	scaleFactor(0.5),
	zoomDelta(0.1),
	translateSpeed(1.0),
	handleMgr(new HandleManager)
{
	setCacheMode(QGraphicsView::CacheBackground);
	setTransformationAnchor(QGraphicsView::AnchorViewCenter);
	setResizeAnchor(QGraphicsView::AnchorViewCenter);

	setRenderHint(QPainter::Antialiasing, true);
	setRenderHint(QPainter::TextAntialiasing, false);

	setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

	setDragMode(QGraphicsView::RubberBandDrag);
	setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);

	/*取消水平和垂直 滚动条*/
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	/*设置场景边框*/
	setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);

	scale(scaleFactor, scaleFactor);

	//设置背景颜色
	setStyleSheet("background-color:#cfcfcf;");

	//初始化场景
	QGraphicsScene* scene = new QGraphicsScene;
	setScene(scene);


	//画面刷新的定时器
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [=]() {this->update(); });
	timer->start(1000 / 60.0f);

	//初始化 操作柄管理器
	handleMgr->init(scene);

	Sprite* sprite = new Sprite;
	sprite->loadSprite("test.png");
	scene->addItem(sprite);
}

WorldEditorView::~WorldEditorView()
{
	timer->stop();
	delete timer;
	timer = NULL;

	delete handleMgr;
	handleMgr = NULL;
}
void WorldEditorView::zoomIn()
{
	zoom(1 + zoomDelta);
}

void WorldEditorView::zoom(float _scaleFactor)
{
	// 防止过小或过大
	qreal factor = transform().scale(_scaleFactor, _scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;
	scale(_scaleFactor, _scaleFactor);
	scaleFactor *= _scaleFactor;
}

void WorldEditorView::zoomOut()
{
	zoom(1 - zoomDelta);
}
void WorldEditorView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == selectButton)
	{
		auto obj = itemAt(event->pos());
		if (obj)
		{
			obj->setSelected(true);
			handleMgr->attachObject(obj);
		}
		else
		{
			for (auto & item :this->items())
			{
				item->setSelected(false);
			}
		}
	}
	if (event->button() == translateButton)
	{
		beginPoint = event->pos();
		isMousePressed = true;
		isDoubleClick = false;

		setDragMode(QGraphicsView::NoDrag);
	}

	QGraphicsView::mousePressEvent(event);
}

void WorldEditorView::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() == translateButton)
	{
		isDoubleClick = true;
	}

	QGraphicsView::mouseDoubleClickEvent(event);
}

void WorldEditorView::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & translateButton)
	{
		if (!isDoubleClick)
		{
			endPoint = event->pos();
			QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(beginPoint);
			beginPoint = endPoint;
			translate(mouseDelta);
		}
	}

	QGraphicsView::mouseMoveEvent(event);
}

void WorldEditorView::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == translateButton)
	{
		if (!isDoubleClick)
		{
			endPoint = event->pos();
			isMousePressed = false;
			setDragMode(QGraphicsView::RubberBandDrag);
		}
	}

	QGraphicsView::mouseReleaseEvent(event);
}

void WorldEditorView::paintEvent(QPaintEvent* event)
{
	if (doubleBufferMode)
	{
		//手动 双重缓冲
		int x, y, w, h;
		x = beginPoint.x();
		y = endPoint.y();
		w = endPoint.x() - x;
		h = endPoint.y() - y;

		QPainter painter(this->viewport());

		if (isMousePressed)
		{
			bufferPixmap = currentPixmap;
			QPainter painter_temp(&bufferPixmap);
			painter_temp.drawRect(x, y, w, h);
			painter.drawPixmap(0, 0, bufferPixmap);
		}
		else
		{
			QPainter painter_temp(&currentPixmap);
			painter_temp.drawRect(x, y, w, h);
			painter.drawPixmap(0, 0, currentPixmap);
		}
	}
	QGraphicsView::paintEvent(event);
}
