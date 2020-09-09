#include "BasicShapes/Drawable.h"

VkCommandBuffer         Drawable::m_CmdBuffer = VK_NULL_HANDLE;
VkDevice                Drawable::m_vkDevice = VK_NULL_HANDLE;
VkPhysicalDevice        Drawable::m_vkPhysicalDevice = VK_NULL_HANDLE;

Drawable::Drawable() {
	m_Position = QVector3D(0, 0, 0);
	m_Rotation = QVector3D(0, 0, 0);
	m_Scale = QVector3D(1, 1, 1);

	m_IsValid = false;
	m_VertexIndex = 0;
}

void Drawable::SetVertex2f(float x, float y)
{
	if (m_VertexIndex == m_VertexCount)
		return;

	m_VertexData[m_VertexIndex * ParamCount + 0] = x;
	m_VertexData[m_VertexIndex * ParamCount + 1] = y;
	m_VertexData[m_VertexIndex * ParamCount + 2] = 0;

	m_VertexIndex++;
}

void Drawable::SetVertex3f(float x, float y, float z)
{
	if (m_VertexIndex == m_VertexCount)
		return;

	m_VertexData[m_VertexIndex * ParamCount + 0] = x;
	m_VertexData[m_VertexIndex * ParamCount + 1] = y;
	m_VertexData[m_VertexIndex * ParamCount + 2] = z;

	m_VertexIndex++;
}

void Drawable::SetColor3f(float r, float g, float b)
{
	if (m_VertexIndex == m_VertexCount)
		return;

	m_VertexData[m_VertexIndex * ParamCount + 3] = r;
	m_VertexData[m_VertexIndex * ParamCount + 4] = g;
	m_VertexData[m_VertexIndex * ParamCount + 5] = b;
	m_VertexData[m_VertexIndex * ParamCount + 6] = 1.0f;
}

void Drawable::SetColor4f(float r, float g, float b, float a)
{
	if (m_VertexIndex == m_VertexCount)
		return;

	m_VertexData[m_VertexIndex * ParamCount + 3] = r;
	m_VertexData[m_VertexIndex * ParamCount + 4] = g;
	m_VertexData[m_VertexIndex * ParamCount + 5] = b;
	m_VertexData[m_VertexIndex * ParamCount + 6] = a;
}

QMatrix4x4 Drawable::GetTransformMartix()
{
	QMatrix4x4 result;
	result.setToIdentity();
	result.translate(m_Position);
	result.rotate(m_Rotation.x(), 1, 0, 0);
	result.rotate(m_Rotation.y(), 0, 1, 0);
	result.rotate(m_Rotation.z(), 0, 0, 1);
	result.scale(m_Scale);
	return result;
}

void Drawable::SetCmdBuffer(VkCommandBuffer CmdBuffer)
{
	m_CmdBuffer = CmdBuffer;
}

void Drawable::SetDevice(VkDevice dev)
{
	m_vkDevice = dev;
}

void Drawable::SetPhysicalDevice(VkPhysicalDevice phyDev)
{
	m_vkPhysicalDevice = phyDev;
}
