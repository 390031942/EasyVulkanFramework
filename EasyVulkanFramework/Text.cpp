#include "Text.h"

Text::Text(std::wstring text)
{
	this->text = text;
}


void Text::draw()
{
	float x = transform.position.x();
	int index = 0;

	SingleText* preText;
	SingleText* curText;

	PipelineResource* resource = vkPipelineMap[GetRTTI()];
	
	for (auto iter = text.cbegin(); iter != text.cend(); iter++)
	{
		wchar_t ch = *iter;
		glyphMetrics gm;
		font->getCharInfo(ch,curText, &gm,NULL,NULL);
		if (iter != (text.cend() - 1))
		{
			pCharInfo_Next = EasyVulkanFontManager::GetCharInfo(ch);
		}
		else
		{
			pCharInfo_Next = NULL;
		}

		VkAssert(pCharInfo_Current != NULL, "EasyVulkanFontManager::GetCharInfo()");

		EasyVulkanMatrixState::pushMatrix();
		EasyVulkanMatrixState::translate(x, transform.position.y(), transform.position.z());

		float* mvp = EasyVulkanMatrixState::getFinalMatrix();

		vkCmdPushConstants(vkCmdBuffer, resource->pipelineLayout,VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, mvp);

		Texture* texture = pCharInfo_Current->texture;
		texture->DescriptorIndex = index + m_descriptorsSetIndex;

		texture->Draw();

		EasyVulkanMatrixState::popMatrix();

		if (iter != (text.cend() - 1))
		{
			char ch_next = (*(iter + 1)).toLatin1();
			Texture* nextTexture = EasyVulkanFontManager::GetCharInfo(ch_next)->texture;
			x = x + (texture->GetVertexRect()[1] - texture->GetVertexRect()[0]) / 2 + (nextTexture->GetVertexRect()[1] - nextTexture->GetVertexRect()[0]) / 2;
		}
		index++;
	}
}

void Text::getDesriptSetStartIndex(int index)
{
	m_descriptorsSetIndex = index;
}

int Text::getRTTI()
{
	return RTTI_DRAWABLE_TEXT;
}

void Text::setText(std::string text)
{
}

std::string Text::setText()
{
	return std::string();
}
