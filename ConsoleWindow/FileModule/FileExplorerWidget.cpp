#include "FileExplorerWidget.h"

FileExplorerWidget::FileExplorerWidget(QWidget *parent=NULL)
	: QWidget(parent)
{
	/*
	widget_2->setMinimumSize(1500, 1000);

	QHBoxLayout *pLayout = new QHBoxLayout;
	pLayout->addWidget(m_FileScrollArea);
	pLayout->setMargin(0);
	pLayout->setSpacing(0);
	setLayout(pLayout);*/
	m_FileDisplayWidget = new QWidget(this);


	QRect rect1 = m_FileDisplayWidget->geometry();
	QRect rect2 = geometry();
	m_FileScrollArea= new QScrollArea(this);
	m_FileScrollArea->setWidget(m_FileDisplayWidget);//给widget_2设置滚动条
	m_FileDisplayWidget->setMinimumSize(geometry().width(), 1000);//这里注意，要比主窗体的尺寸要大，不然太小的话会留下一片空白

	QHBoxLayout *pLayout = new QHBoxLayout;
	pLayout->addWidget(m_FileScrollArea);
	pLayout->setMargin(0);
	pLayout->setSpacing(0);
	setLayout(pLayout);

	
	m_ButtonGroup = new QButtonGroup(parent);
	m_ButtonGroup->setExclusive(true);
	setMinimumSize(MinWindowSize);
	m_PreviousSize = size();
	GetFileButtonProperty();
	GetFileList(m_currentPath);
	ShowFileList();
}

FileExplorerWidget::~FileExplorerWidget()
{
}

void FileExplorerWidget::resizeEvent(QResizeEvent *)
{
	int w = ((QWidget*)this->parent())->geometry().width();
	m_FileDisplayWidget->setGeometry(0, 0, w, 10000);
	int deltaWidth = m_PreviousSize.width() - size().width();
	{
		GetFileButtonProperty();
		for (int i = 0; i < m_fileList.size(); ++i) {
			FileButton *button = (FileButton*)m_ButtonGroup->button(i);
			button->setGeometry(File_GetPosition(i, m_ButtonCountPerRow, m_ButtonSize_X, m_Spacing_X, m_ButtonSize_Y, m_Spacing_Y), m_ButtonSize_X, m_ButtonSize_Y);
		}
	}
	m_PreviousSize = size();

}
void FileExplorerWidget::GetFileList(QString path)
{
	QVector<QFileInfo> list;
	m_Dir.setPath(path);
	m_Dir.setFilter(QDir::Files | QDir::AllDirs | QDir::NoSymLinks);
	m_Dir.setSorting(QDir::Size | QDir::SortFlag::DirsFirst);
	m_fileList = m_Dir.entryInfoList();


#ifdef SHOW_FILE_NAMES
	QString ret;
	ret += "Files:\n";
	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		if (fileInfo.isDir())
		{
			ret += ch("文件夹:");
			ret += fileInfo.fileName();
			ret += "\n";
			list.push_back(fileInfo);
		}
		else
		{
			ret += ch("文件:");
			ret += fileInfo.fileName();
			ret += ch("   文件大小:");
			ret += QString::number(fileInfo.size() / 1024);
			ret += ch("kb   后缀:");
			ret += fileInfo.suffix();
			ret += "\n";
		}

	}
	QMessageBox::information(NULL, "File names", ret);
#endif
}

void FileExplorerWidget::ShowFileList()
{
	for (int i = 0; i < m_fileList.size(); ++i) {
		QFileInfo fileInfo = m_fileList.at(i);
		FileButton *button = new FileButton(this->m_FileDisplayWidget, fileInfo.fileName(), fileInfo.suffix());
		connect(button, static_cast<void(FileButton::*)(int)>(&FileButton::DoubleClick),
			this, &FileExplorerWidget::OnDoubleClick);
		button->setGeometry(File_GetPosition(i, m_ButtonCountPerRow, m_ButtonSize_X, m_Spacing_X,m_ButtonSize_Y,m_Spacing_Y), m_ButtonSize_X, m_ButtonSize_Y);
		button->SetButtonID(i);
		button->show();
		if (fileInfo.isDir())
		{
			m_ButtonGroup->addButton(button, i);
		}
		else
		{
			m_ButtonGroup->addButton(button, i);
		}
	}
}

void FileExplorerWidget::ReSetFileList()
{
}

void FileExplorerWidget::GetFileButtonProperty()
{
	m_ButtonSize_X = Default_ButtonSize_X;
	m_ButtonSize_Y = Default_ButtonSize_Y;

	m_Spacing_X = Default_m_Spacing_X;
	m_Spacing_Y = Default_m_Spacing_Y;

	m_ButtonCountPerRow = File_GetButtonCountPerRow(width(), m_ButtonSize_X, m_Spacing_X);

	m_Spacing_X = File_GetCorrectedSpacingX(m_ButtonCountPerRow, width(), m_ButtonSize_X);
}

void FileExplorerWidget::OnDoubleClick(int id)
{
	FileButton *fButton = (FileButton*)m_ButtonGroup->button(id);
	m_Dir.cd(fButton->GetFileLabel());
	m_Dir.setFilter(QDir::Files | QDir::AllDirs | QDir::NoSymLinks);
	m_Dir.setSorting(QDir::Size | QDir::SortFlag::DirsFirst);
	CleanCurrentFileList();
	m_fileList = m_Dir.entryInfoList();
	//if (m_fileList.size()&& m_fileList[0].fileName() == ".")
	//  m_fileList.removeAt(0);
	ShowFileList();
#ifdef SHOW_FILE_NAMES
	QString ret;
	ret += "Files:\n";
	for (int i = 0; i < m_fileList.size(); ++i) {
		QFileInfo fileInfo = m_fileList.at(i);
		if (fileInfo.isDir())
		{
			ret += ch("文件夹:");
			ret += fileInfo.fileName();
			ret += "\n";
		}
		else
		{
			ret += ch("文件:");
			ret += fileInfo.fileName();
			ret += ch("   文件大小:");
			ret += QString::number(fileInfo.size() / 1024);
			ret += ch("kb   后缀:");
			ret += fileInfo.suffix();
			ret += "\n";
		}
	}

	//QMessageBox::information(NULL, "File names", ret);
#endif
}

void FileExplorerWidget::CleanCurrentFileList()
{
	for (int i = 0; i < m_fileList.size(); ++i) {
		FileButton *fButton = (FileButton *)m_ButtonGroup->button(i);
		m_ButtonGroup->removeButton(fButton);
		delete fButton;
	}
	m_fileList.clear();
}

void FileExplorerWidget::InitWidget()
{
	this->m_FileScrollArea = new QScrollArea(this);
	this->m_FileScrollArea->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	this->m_FileScrollArea->setWidgetResizable(true);
	this->m_FileScrollArea->setWidget(this);

	this->m_FileLayout = new QGridLayout(this);
	this->m_FileLayout->setSizeConstraint(QGridLayout::SetMinAndMaxSize);

	//QWidget* widget = new QWidget(this);
	//widget->geometry = geometry();

	//QVBoxLayout *mainLayout = new QVBoxLayout(this);
	//mainLayout->addWidget(this->m_pChatListScrollArea);
	//this->setLayout(mainLayout);

	setLayout(this->m_FileLayout);
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(this->m_FileScrollArea);
	this->setLayout(mainLayout);

	QPushButton* pushButton;
	pushButton = new QPushButton("Test", this);
	pushButton->setObjectName("TestButton");
	pushButton->setMinimumSize(pushButton->size());
	//pushButton->setMaximumSize(pushButton->size());
	this->m_FileLayout->addWidget(pushButton);
	connect(pushButton, &QPushButton::clicked, [=](void) {QMessageBox::information(NULL, "Clicked Button", "PushButton"); });
}