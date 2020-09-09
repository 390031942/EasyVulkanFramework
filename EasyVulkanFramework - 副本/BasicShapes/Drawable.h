#pragma once

#include <vulkan/vulkan.h>
#include <qvector3d.h>
#include <QMatrix4x4>
#include <vector>
#include <Windows.h>

#include "../EasyVulkanMemoryUtility.h"
using namespace std;


#define ParamCount 7

class Drawable
{
public:
	Drawable();
protected:
	float* m_VertexData;
	int    m_DataByteCount;
	int    m_VertexCount;
	int    m_VertexIndex;

public:
	void  SetVertex2f(float x, float y);
	void  SetVertex3f(float x, float y, float z);

	void SetColor3f(float r, float g, float b);

	void SetColor4f(float r, float g, float b, float a);
private:
	QVector3D  m_Position;
	QVector3D  m_Rotation;
	QVector3D  m_Scale;
public:
	QMatrix4x4 GetTransformMartix();

protected:
	VkBuffer        m_Buffer;
	VkDeviceMemory  m_BufferMemory;

protected:
	static VkCommandBuffer m_CmdBuffer;
public:
	static void SetCmdBuffer(VkCommandBuffer CmdBuffer);

	static void SetDevice(VkDevice dev);

	static void SetPhysicalDevice(VkPhysicalDevice phyDev);
protected:
	bool            m_IsValid;
public:
	static VkDevice                m_vkDevice;
	static VkPhysicalDevice        m_vkPhysicalDevice;
public:
	virtual void  CreateVertexData() = 0;
	virtual void  Draw(VkPipelineLayout& pipelineLayout, VkPipeline& pipeline, VkDescriptorSet& desSetPointer) = 0;
};
