#include "FileLabel.h"

FileLabel::FileLabel(QWidget *parent)
	: QLabel(parent)
{
	setAlignment(Qt::AlignCenter);
	m_edit = new FileLabelEdit(parent);
	m_edit->hide();

	setStyleSheet("color:black");
}

FileLabel::~FileLabel()
{
	
}

void FileLabel::mousePressEvent(QMouseEvent * event)
{
	this->hide();
	m_edit->setGeometry(geometry().x()+4, geometry().y(), width(), height());
	m_edit->show();
	m_edit->setFocus();
	m_edit->setText(m_FullString);
}

void FileLabel::ResetGeometry(QRect rect)
{
	setGeometry(rect.x(), rect.y(),width(),height());
}

void FileLabel::setText(const QString string)
{
	m_FullString = string;
	QFont font("Tahoma");
	font.setPixelSize(13);
	this->setFont(font);
	QFontMetrics fontMetrics(this->font());
	int fontSize = fontMetrics.width(string);//��ȡ֮ǰ���õ��ַ��������ش�С
	QString str = string;
	if (fontSize > this->width())
	{
		str = fontMetrics.elidedText(string, Qt::ElideRight, this->width());//����һ������ʡ�Ժŵ��ַ���
	}
	QLabel::setText(str);

}

