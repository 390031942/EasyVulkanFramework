#include "Draggable.h"

AbstractDraggable::AbstractDraggable()
{
	setFlags(flags()| 
		QGraphicsItem::ItemIsSelectable |
		QGraphicsItem::ItemSendsGeometryChanges |
		QGraphicsItem::ItemIsFocusable |
		QGraphicsItem::ItemIsMovable);

	setAcceptHoverEvents(true);
}

QVariant AbstractDraggable::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == ItemPositionHasChanged && scene())
	{
		emit dragged();
	}
	return QGraphicsItem::itemChange(change, value);
}

AbstractDraggable::~AbstractDraggable()
{

}
