#pragma once

#include <QPushButton>
#include <QLabel>
#include <qevent.h>
#include <qmenu.h>
#include <qmessagebox.h>
#include "FileLabel.h"
#include "Easy2DFileDefs.h"
#include <qtimer.h>
class QTimer;

class FileButton : public QPushButton
{
	Q_OBJECT

public:
	FileButton(QWidget *parent,const QString filename,const QString suffix);
	~FileButton();
	void SetButtonID(int id);
	QString GetFileLabel();
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent (QMouseEvent *event);
private:
	FileLabel *m_FileLabel;
	int m_id;
	QMenu *m_contextMenu;
	QAction *m_addAction;
	QAction *m_delAction;
	QPixmap  m_Icon;
	void OnClick_Add();
	void OnClick_Del();
	void OnEdit_Finished();
signals:
	void DoubleClick(int id);
};
