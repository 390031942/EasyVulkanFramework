#pragma once
#include <SDL_ttf.h>
#include <QString>
#include "EasyVulkanFontManager.h"
#include "Matrix/EasyVulkanMatrixState.h"

#include "Transform.h"

class Label
{
public:
	QString   text;
	Transform transform;

	Label(QString text, QVector3D pos);

	void Draw(VkDescriptorSet* vkpDescriptorset);
	void SetDesriptSetStartIndex(int index)
	{
		m_descriptorsSetIndex = index;
	}

public:
	static VkCommandBuffer    vkCmdBuffer;
	static VkPipelineLayout   vkPipelineLayout;

private:
	int m_descriptorsSetIndex;

};

