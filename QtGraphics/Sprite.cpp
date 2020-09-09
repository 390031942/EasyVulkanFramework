#include "Sprite.h"


void Sprite::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->drawImage(boundingRect().topLeft(), m_Picture);
	//painter->drawPixmap(boundingRect().topLeft(), *m_Picture);
}

Sprite::~Sprite()
{

}

