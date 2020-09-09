#pragma once

#include <vulkan/vulkan.h>
#include <qvector3d.h>
#include <QMatrix4x4>
#include <vector>
#include <Windows.h>

#include "../EasyVulkanMemoryUtility.h"
#include "../transform.h"
#include "../VkObject.h"
#include "../EasyVulkanGlobal.h"

using namespace std;


#define ParamCount 7

#define RTTI_DRAWABLE_TEXTURE 0x01
#define RTTI_DRAWABLE_BASIC   0x02
#define RTTI_DRAWABLE_TEXT    0x03
#define RTTI_DRAWABLE_SPRITE  0x05

class Drawable
{
public:
	Drawable() {}

protected:
	VkBuffer        m_VertexBuffer;
	VkDeviceMemory  m_VertexBufferMemory;

	VkPipeline vkPipeline;
	VkDescriptorSet vkDescriptorSet;

	int drawableRTTI;

public:
	void setVkPipeLine(VkPipeline vkPipeline)
	{
		this->vkPipeline = vkPipeline;
	}

	void setVkDescriptorSet(VkDescriptorSet vkDescriptorSet)
	{
		this->vkDescriptorSet = vkDescriptorSet;
	}

	VkPipeline getVkPipeLine()
	{
		return this->vkPipeline;
	}

	VkDescriptorSet getVkDescriptorSet()
	{
		return this->vkDescriptorSet;
	}
	int getDrawableRTTI()
	{
		return drawableRTTI;
	}
public:
	virtual void  createVertexData() = 0;
	virtual void  draw() = 0;
};

class DrawableObject :public VkObject
{
private:
	bool valid = false; 

	friend class DrawTaskManager;
};