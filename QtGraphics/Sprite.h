#pragma once

#include <QObject>
#include <qgraphicsview.h>
#include <qgraphicsscene.h>
#include <qgraphicsitem.h>
#include <qcolor.h>
class Sprite : public QObject, public QGraphicsItem
{
	Q_OBJECT
public:
	explicit Sprite()
	{

	}
	void SetSprite(QString filename)
	{
		bool ret = m_Picture.load(filename);
		if (!ret)
			return;

		m_Picture_Copy.load(filename);
	}
	void SetSize(int w, int h) {
		m_Picture = m_Picture.scaled(QSize(w, h));
		m_Picture_Copy = m_Picture_Copy.scaled(QSize(w, h));
	}
	//Color Blend 算法 
	void SetColor(QColor color)
	{
		SetColor(color.red(), color.green(), color.blue(), color.alpha());
	}

	QVector<QColor> m_ColorTable;
	void SetColor(int r, int g, int b, int a = 255) {
		int bmpWidth = m_Picture.width();
		int bmpHeight = m_Picture.height();

		QColor color = QColor(r, g, b, a);
		for (int i = 0; i < bmpWidth; ++i)
		{
			for (int j = 0; j < bmpHeight; ++j)
			{
				QColor color_p = m_Picture_Copy.pixelColor(i, j);
				if (color_p == QColor(0, 0, 0, 0))
					continue;
				QColor color_blend;


				float r1, g1, b1, a1;
				float r2, g2, b2, a2;
				float rm, gm, bm, am;

				r1 = color_p.redF();
				g1 = color_p.greenF();
				b1 = color_p.blueF();
				a1 = color_p.alphaF();

				r2 = color.redF();
				g2 = color.greenF();
				b2 = color.blueF();
				a2 = color.alphaF();
				//颜色分量叠加
				rm = (r1 * a1 + r2 * a2) / (a1 + a2);
				gm = (g1 * a1 + g2 * a2) / (a1 + a2);
				bm = (b1 * a1 + b2 * a2) / (a1 + a2);
				am = a1 * a2;

				color_blend = QColor(rm * 255, gm * 255, bm * 255, am * 255);

				m_Picture.setPixelColor(i, j, color_blend);
			}
		}
	}

	QRectF   boundingRect() const
	{
		int offset = 2;
		QRectF ret = QRectF(-m_Picture.width() / 2 - offset,
			-m_Picture.height() / 2 - offset,
			m_Picture.width() + offset * 2,
			m_Picture.height() + offset * 2);

		return ret;
	}

public slots:

protected:
	void     paint(QPainter* painter,
		const QStyleOptionGraphicsItem* option,
		QWidget* widget);

private:
	QImage m_Picture;
	QImage m_Picture_Copy;
public:
	~Sprite();
};
