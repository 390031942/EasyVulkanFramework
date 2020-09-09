#pragma once
#include <QSize>
#include <QImage>
#include <qfileinfo.h>
#include "vulkan/vulkan.h"
#include <QUuid>
#define VertexCount 4

#include "EasyVulkanTextureManager.h"
#include "Transform.h"

struct TextureResource
{
	VkImage                vkImage;
	VkDeviceMemory         vkDeviceMemory;
	VkImageView            vkImageView;
	VkDescriptorImageInfo  vkImageInfo;
	VkBuffer               vkVertexBuffer;
	VkDeviceMemory         vkVertexMemory;
	VkDescriptorBufferInfo vkVertexBufferInfo;

	void operator = (TextureResource &textureResource)
	{
		this->vkDeviceMemory = textureResource.vkDeviceMemory;
		this->vkImage = textureResource.vkImage;
		this->vkImageInfo = textureResource.vkImageInfo;
		this->vkImageView = textureResource.vkImageView;
		this->vkVertexBuffer = textureResource.vkVertexBuffer;
		this->vkVertexMemory = textureResource.vkVertexMemory;
		this->vkVertexBufferInfo = textureResource.vkVertexBufferInfo;
	}
};

class Texture
{
private:
	Texture();

	QSize                 m_Size;
	int                   m_DataByteCount;
	unsigned char*        m_pTextureData;
	QString               m_Name;
	TextureResource       m_Resource;

	float                 m_VertexRect[4];
	float                 m_TexCoorRect[4];

	void                  Create(QString fileName);
	void                  Create(uchar *data,int width, int height, QImage::Format format);

	QImage                AdjustSize(QImage image, int width, int height, int n, bool drawBorder=false);

	void                  SetTexCoorRect(float left, float right, float top, float bottom);
public:
	Transform             transform;

	void                  SetPosition(float x, float y, float z = 0.0f);
	VkImage               GetVkImage();
	VkDescriptorImageInfo GetVkImageInfo();
	void                  Draw();
	QString               GetTextureName();
	QSize                 GetSize();

	float*                GetVertexRect();
	float*                GetTexCoorRect();
	friend class EasyVulkanTextureManager;
};

