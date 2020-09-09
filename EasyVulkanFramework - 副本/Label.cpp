#include "Label.h"

VkCommandBuffer       Label::vkCmdBuffer = VK_NULL_HANDLE;
VkPipelineLayout      Label::vkPipelineLayout = VK_NULL_HANDLE;

Label::Label(QString text, QVector3D pos)
{
	this->text = text;
	transform.position = pos;
}


void Label::Draw(VkDescriptorSet* vkpDescriptorset)
{
	float x = transform.position.x();
	int index = 0;
	Character* pCharInfo_Current;
	Character* pCharInfo_Next;

	for (auto iter = text.cbegin(); iter != text.cend(); iter++)
	{
		char ch = (*iter).toLatin1();
		pCharInfo_Current = EasyVulkanFontManager::GetCharInfo(ch);
		if (iter != (text.cend() - 1))
		{
			pCharInfo_Next = EasyVulkanFontManager::GetCharInfo(ch);
		}
		else
		{
			pCharInfo_Next = NULL;
		}

		VkAssert(pCharInfo_Current != NULL, "EasyVulkanFontManager::GetCharInfo()");
		if (true)
		{
			float xPos = x + pCharInfo_Current->bearing.x();
			float yPos = transform.position.y();

			float w = pCharInfo_Current->texture->GetSize().width();
			float h = pCharInfo_Current->texture->GetSize().height();

			EasyVulkanMatrixState::pushMatrix();
			EasyVulkanMatrixState::translate(x, transform.position.y(), transform.position.z());

			EasyVulkanTextureManager::setDescriptorSetPtr(&vkpDescriptorset[index + m_descriptorsSetIndex]);

			float* mvp = EasyVulkanMatrixState::getFinalMatrix();

			vkCmdPushConstants(vkCmdBuffer, vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, mvp);

			Texture* texture = pCharInfo_Current->texture;
			texture->Draw();


			EasyVulkanMatrixState::popMatrix();

			if (iter != (text.cend()-1))
			{
				char ch_next = (*(iter+1)).toLatin1();
				Texture *nextTexture = EasyVulkanFontManager::GetCharInfo(ch_next)->texture;
				x = x + (texture->GetVertexRect()[1] - texture->GetVertexRect()[0]) / 2 + (nextTexture->GetVertexRect()[1] - nextTexture->GetVertexRect()[0])/2;

			}
		}

		index++;
	}
}
