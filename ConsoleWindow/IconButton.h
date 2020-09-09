#pragma once
#include <QToolButton>
#include <string>

class IconButton:public QToolButton
{
public:
	IconButton(std::string iconPath)
	{
		setStyleSheet("QToolButton{border:Opx}");
		setIconSize(QSize(width(), height()));
	    QPixmap pixmap(iconPath.c_str());
	    setIcon(QIcon(pixmap));

		setAttribute(Qt::WA_DeleteOnClose);
	}

	void setButtonSize(int width, int height)
	{
		setGeometry(pos().x(), pos().y(), width, height);
	}
	
	void setPosition(int x, int y)
	{
		setGeometry(x,y, width(), height());
	}


	template<typename Func>
	void addClickEvent(const typename QtPrivate::FunctionPointer<Func>::Object* receiver, Func slot)
	{
		connect(this, &IconButton::clicked, receiver, slot);
	}
	template<typename Func>
	void addClickEvent(Func slot)
	{
		connect(this, &IconButton::clicked, this, slot);
	}
};

