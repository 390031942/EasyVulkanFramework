#pragma once
#include <QString>
#include "EasyVulkanFontManager.h"
#include "Matrix/EasyVulkanMatrixState.h"

#include "Transform.h"
#include "BasicShapes/Drawable.h"

class Text:public DrawableObject
{
public:
	VkFont* font;
	std::wstring text;
	Text(std::wstring text);

	void draw();
	void getDesriptSetStartIndex(int index);
	int  getRTTI();
	void setText(std::string text);
	std::wstring setText();
private:
	int m_descriptorsSetIndex;
};

