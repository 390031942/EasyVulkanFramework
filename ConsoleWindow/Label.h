#pragma once
#include <qlabel.h>

class Label :public QLabel
{
private:

	bool autoAdjustSize = true;

	int getWidth()
	{
		QFontMetrics metrics = this->fontMetrics();
		return metrics.width(text());
	}

	void adjustSize()
	{
		if (!autoAdjustSize)
		{
			this->setFixedWidth(this->getWidth());
		}
	}
	class Property
	{
	private:
		Label* _this;
	public:
		Property(Label* _this = NULL)
		{
			this->_this = _this;
		}
		void setIsSelectable(bool enabled)
		{
			_this->setTextInteractionFlags(_this->textInteractionFlags().setFlag(Qt::TextInteractionFlags::enum_type::TextSelectableByMouse, enabled));
		}
		bool getIsSelectable()
		{
			return _this->textInteractionFlags()&Qt::TextInteractionFlag::TextSelectableByMouse;
		}

		void setIsAutoAdjustSize(bool enabled)
		{
			_this->autoAdjustSize = enabled;
		}
		bool getIsAutoAdjustSize()
		{
			return _this->autoAdjustSize;
		}
	};
public:
	Label(std::string label = "")
	{
		property = Property(this);
		setAttribute(Qt::WA_DeleteOnClose);
	}
	Property property;

};

