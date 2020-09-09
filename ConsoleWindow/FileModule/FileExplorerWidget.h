#pragma once

#include <QWidget>
#include <qlayout.h>
#include <qscrollarea.h>
#include <qdir.h>
#include <qbuttongroup.h>
#include "Easy2DFileDefs.h"
#include "FileButton.h"

class FileExplorerWidget : public QWidget
{
	Q_OBJECT

public:
	FileExplorerWidget(QWidget *parent);
	~FileExplorerWidget();

public:
protected:
	void resizeEvent(QResizeEvent*);
private:
	QList<QFileInfo> m_fileList;
	QButtonGroup *m_ButtonGroup;

	void GetFileList(QString path);
	void ShowFileList();
	void ReSetFileList();
	void GetFileButtonProperty();

	int m_Spacing_X;
	int m_Spacing_Y;

	int m_ButtonSize_X;
	int m_ButtonSize_Y;

	int m_ButtonCountPerRow;
	QSize m_PreviousSize;
	void OnDoubleClick(int);

	QString m_currentPath;
	void CleanCurrentFileList();

	QDir m_Dir;

private:
	QScrollArea* m_FileScrollArea;
	QGridLayout* m_FileLayout;
	QWidget    * m_FileDisplayWidget;
	void InitWidget();
};
