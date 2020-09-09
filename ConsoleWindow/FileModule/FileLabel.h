#pragma once

#include <QLabel>
#include <qlineedit.h>
#include "FileLabelEdit.h"

class FileLabel : public QLabel
{
	Q_OBJECT

public:
	FileLabel(QWidget *parent);
	~FileLabel();
	FileLabelEdit *m_edit;
	void setText(const QString string);
protected:
	void mousePressEvent(QMouseEvent *event);
    void ResetGeometry(QRect rect);
private:
	QString m_FullString;
};
