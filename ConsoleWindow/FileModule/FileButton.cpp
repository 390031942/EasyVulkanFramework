#include "FileButton.h"

FileButton::FileButton(QWidget *parent,const QString filename, const QString suffix)
	: QPushButton(parent)

{
	//Create File Button Icon,and set icon as well as it's size
	m_Icon = QPixmap(FILEICON_PATH(suffix));
    setIcon(m_Icon);
	setIconSize(QSize(Default_FileButtonIconSizeX, Default_FileButtonIconSizeY));

	//Two statements below are to set the transparent effect of the button's icon
	setFlat(true);
	setAttribute(Qt::WA_TranslucentBackground);

	//Create Menu of the button,so that you can click the Mouse1 to show the menu
	m_contextMenu = new QMenu;
	m_addAction = new QAction("TEST MENU1", this);
	m_delAction = new QAction("TEST MENU2", this);

	//Add actions of the menu's buttons
	m_contextMenu->addAction(m_addAction);
	m_contextMenu->addAction(m_delAction);

	//Connect the response event
	connect(m_addAction, &QAction::triggered, this, &FileButton::OnClick_Add);
	connect(m_delAction, &QAction::triggered, this, &FileButton::OnClick_Del);

	setCheckable(true);
	setStyleSheet("color:white;");

	//Set the property of button's label
	m_FileLabel = new FileLabel(this);
	//the rect of the button
	QRect rect = geometry();
	//Connect the response event
	connect(m_FileLabel->m_edit, &QLineEdit::editingFinished, this, &FileButton::OnEdit_Finished);
	m_FileLabel->setGeometry(rect.x()+Default_FileButtonLabelOffsetX,
		                     rect.y()+Default_FileButtonLabelOffsetY, 
		                     Default_FileButtonLabelSizeX, 
	                      	 Default_FileButtonLabelSizeY);
	m_FileLabel->setText(filename);

}

FileButton::~FileButton()
{

}

void FileButton::SetButtonID(int id)
{
	m_id = id;
}

QString FileButton::GetFileLabel()
{
	return m_FileLabel->text();
}

void FileButton::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		/*if (isChecked())
		{
			setStyleSheet("color:black;");
		}
		else
		{
			setStyleSheet("color:white;");
		}*/
	}
	else if (event->button()== Qt::RightButton)
	{
		m_contextMenu->exec(event->globalPos());
	}

	QPushButton::mousePressEvent(event);
}

void FileButton::mouseDoubleClickEvent(QMouseEvent * event)
{
	//QMessageBox::information(NULL, "Ë«»÷", "");
	if (event->button() == Qt::LeftButton)
	{
		emit DoubleClick(m_id);
	}
}


void FileButton::OnClick_Add()
{
	QMessageBox::information(this, "Tips", "123");
}

void FileButton::OnClick_Del()
{
	QMessageBox::information(this, "Tips", "del");
}

void FileButton::OnEdit_Finished()
{
	m_FileLabel->m_edit->hide();
	m_FileLabel->setText(m_FileLabel->m_edit->text());
	m_FileLabel->show();
}
