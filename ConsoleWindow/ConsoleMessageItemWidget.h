#pragma once
#include <QListWidget>
#include <QHBoxLayout>
#include <QLabel>

#include <QDateTime>

#define Default_Message_Icon_Size QSize(30,30)
#define Default_Message_Icon_Width 30
class ConsoleMessageItemWidget:public QWidget
{
public:
	enum MessageType { Info, Warning, Error };
private:
	QPixmap getIconByMessageType(MessageType type)
	{
		if (type == MessageType::Info)
		{
			return "icons/console/console_info.png";
		}
		else if (type == MessageType::Warning)
		{
			return "icons/console/console_warning.png";
		}
		else
		{
			return "icons/console/console_error.png";
		}
	}

	std::string getMessagePrefixString(MessageType type)
	{
		std::string result;

		if (type == MessageType::Info)
		{
			result = "[Info] ";
		}
		else if (type == MessageType::Warning)
		{
			result = "[Warning] ";
		}
		else
		{
			result = "[Error] ";
		}
		QDateTime current_date_time = QDateTime::currentDateTime();
		QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
		result += current_date.toStdString();
		
		return result;
	}

	std::string detail;
	std::string message;
	MessageType type;

	QLabel* detailLabel;
	QLabel* messasgeLabel;
public:
	MessageType getType()
	{
		return type;
	}
	bool serach(std::string content)
	{
		int detailPos = this->detail.find(content);
		int messagePos = this->message.find(content);

		bool detailMatched = false;
		bool messageMatched = false;

		int contentSize = content.size();
		if (detailPos != this->detail.npos)
		{
			detailMatched = true;
			this->detailLabel->setSelection(detailPos, contentSize);
		}
		if (messagePos != this->message.npos)
		{
			messageMatched = true;
			this->messasgeLabel->setSelection(messagePos, contentSize);
		}
		return detailMatched|| messageMatched;
	}

	ConsoleMessageItemWidget(QListWidget *parent,MessageType type, std::string messageSource, std::string methodName,std::string message)
	{
		this->type = type;

		setLayout(new QHBoxLayout);
		layout()->setContentsMargins(10, 0, 10, 0);
		layout()->setSpacing(1);
		QLabel* label = new QLabel;
		label->setFixedWidth(Default_Message_Icon_Width);
		label->setPixmap(getIconByMessageType(type).scaled(Default_Message_Icon_Size));

		layout()->addWidget(label);

		QWidget* widget_Right = new QWidget;
		widget_Right->setLayout(new QVBoxLayout);
		widget_Right->layout()->setSpacing(1);

		this->detail = (getMessagePrefixString(type) + " at File \"" + messageSource + "\" " + methodName + "()");
		detailLabel = new QLabel(this->detail.c_str());
		messasgeLabel = new QLabel(message.c_str());

		//this->detailLabel->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByKeyboard);
		//this->messasgeLabel->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByKeyboard);

		widget_Right->layout()->addWidget(detailLabel);

		this->message = message;
		widget_Right->layout()->addWidget(messasgeLabel);

		layout()->addWidget(widget_Right);
	}
};

