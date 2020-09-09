#pragma once

#include <QLineEdit>
#include <qevent.h>
#include <qmessagebox.h>
class FileLabelEdit : public QLineEdit
{
	Q_OBJECT

public:
	FileLabelEdit(QWidget *parent);
	~FileLabelEdit();
protected:
	//void mousePressEvent(QMouseEvent *event);
};
