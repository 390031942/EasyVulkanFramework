#pragma once
#include <QLineEdit>

class LineEdit:public QLineEdit
{
	Q_PROPERTY(double minValue READ getMinValue WRITE setMinValue)
public:
	LineEdit()
	{
		setAttribute(Qt::WA_DeleteOnClose);
	}
};

