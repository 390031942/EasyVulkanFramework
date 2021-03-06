#include "BasicShapes.h"

BasicShapes::BasicShapes(int vertexCount) :
	m_VertexCount(vertexCount >= 5 ? vertexCount + 1 : vertexCount),
	m_IsValid(false)
{
	m_VertexData = new float[ParamCount * m_VertexCount];
	ZeroMemory(m_VertexData, ParamCount * m_VertexCount * sizeof(float));

	m_VertexSize = ParamCount * m_VertexCount * sizeof(float);
	m_IndexSize = (vertexCount - 2) * 3 * sizeof(float);

	if (vertexCount == 3)
	{
		m_IndexSize = 3;
		m_Indices.push_back(0);
		m_Indices.push_back(1);
		m_Indices.push_back(2);
	}

	else if (vertexCount == 4)
	{
		m_IndexSize = 6;
		m_Indices.push_back(0);
		m_Indices.push_back(1);
		m_Indices.push_back(2);

		m_Indices.push_back(0);
		m_Indices.push_back(2);
		m_Indices.push_back(3);
	}

	else
	{
		m_IndexSize = 3 * vertexCount;

		for (int i = 0; i < vertexCount - 1; i++)
		{
			m_Indices.push_back(vertexCount - 1);
			m_Indices.push_back(i % (vertexCount - 1));
			m_Indices.push_back((i + 1) % (vertexCount - 1));
		}
	}

}

void BasicShapes::SetVertex2f(float x, float y)
{
	if (m_VertexCount <= 4 ? (m_VertexIndex == m_VertexCount) : (m_VertexIndex == m_VertexCount - 1))
	{
		CreateVertexData();
		return;
	}
	

	m_VertexData[m_VertexIndex * ParamCount + 0] = x;
	m_VertexData[m_VertexIndex * ParamCount + 1] = y;
	m_VertexData[m_VertexIndex * ParamCount + 2] = 0;

	m_VertexIndex++;
}

void BasicShapes::SetVertex3f(float x, float y, float z)
{
	if (m_VertexCount <= 4 ? (m_VertexIndex == m_VertexCount) : (m_VertexIndex == m_VertexCount - 1))
	{
		CreateVertexData();
		return;
	}

	m_VertexData[m_VertexIndex * ParamCount + 0] = x;
	m_VertexData[m_VertexIndex * ParamCount + 1] = y;
	m_VertexData[m_VertexIndex * ParamCount + 2] = z;

	m_VertexIndex++;
}

void BasicShapes::SetColor3f(float r, float g, float b)
{
	if (m_VertexCount<=4?(m_VertexIndex == m_VertexCount): (m_VertexIndex == m_VertexCount-1))
	{
		CreateVertexData();
		return;
	}

	m_VertexData[m_VertexIndex * ParamCount + 3] = r;
	m_VertexData[m_VertexIndex * ParamCount + 4] = g;
	m_VertexData[m_VertexIndex * ParamCount + 5] = b;
	m_VertexData[m_VertexIndex * ParamCount + 6] = 1.0f;
}

void BasicShapes::SetColor4f(float r, float g, float b, float a)
{
	if (m_VertexCount <= 4 ? (m_VertexIndex == m_VertexCount) : (m_VertexIndex == m_VertexCount - 1))
	{
		CreateVertexData();
		return;
	}

	m_VertexData[m_VertexIndex * ParamCount + 3] = r;
	m_VertexData[m_VertexIndex * ParamCount + 4] = g;
	m_VertexData[m_VertexIndex * ParamCount + 5] = b;
	m_VertexData[m_VertexIndex * ParamCount + 6] = a;
}

void BasicShapes::CreateVertexData()
{
	if (m_VertexCount >= 6)
	{
		float avgX, avgY, avgZ, avgR, avgG, avgB, avgA;
		for (int i = 0; i < m_VertexCount - 1; i++)
		{
			avgX += (m_VertexData[m_VertexIndex * ParamCount + 0]) / m_VertexCount;
			avgY += (m_VertexData[m_VertexIndex * ParamCount + 1]) / m_VertexCount;
			avgZ += (m_VertexData[m_VertexIndex * ParamCount + 2]) / m_VertexCount;
			avgR += (m_VertexData[m_VertexIndex * ParamCount + 3]) / m_VertexCount;
			avgG += (m_VertexData[m_VertexIndex * ParamCount + 4]) / m_VertexCount;
			avgB += (m_VertexData[m_VertexIndex * ParamCount + 5]) / m_VertexCount;
			avgA += (m_VertexData[m_VertexIndex * ParamCount + 6]) / m_VertexCount;
		}

		m_VertexData[m_VertexCount - 1 * ParamCount + 0] = avgX;
		m_VertexData[m_VertexCount - 1 * ParamCount + 1] = avgY;
		m_VertexData[m_VertexCount - 1 * ParamCount + 2] = avgZ;
		m_VertexData[m_VertexCount - 1 * ParamCount + 3] = avgR;
		m_VertexData[m_VertexCount - 1 * ParamCount + 4] = avgG;
		m_VertexData[m_VertexCount - 1 * ParamCount + 5] = avgB;
		m_VertexData[m_VertexCount - 1 * ParamCount + 6] = avgA;
	}
	int m_BufferByteCount = m_VertexSize + m_IndexSize;

	VkBufferCreateInfo* m_pBufferInfo = new VkBufferCreateInfo;

	m_pBufferInfo->sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	m_pBufferInfo->pNext = NULL;
	m_pBufferInfo->usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	m_pBufferInfo->size = m_BufferByteCount;
	m_pBufferInfo->queueFamilyIndexCount = 0;
	m_pBufferInfo->pQueueFamilyIndices = NULL;
	m_pBufferInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_pBufferInfo->flags = 0;

	VkResult result = vkCreateBuffer(vkDevice, m_pBufferInfo, NULL, &m_VertexBuffer);
	assert(result == VK_SUCCESS);


	VkMemoryRequirements* m_pMemoryReq = new VkMemoryRequirements;
	vkGetBufferMemoryRequirements(vkDevice, m_VertexBuffer, m_pMemoryReq);

	VkMemoryAllocateInfo* m_pMemoryAllocateInfo = new VkMemoryAllocateInfo;
	m_pMemoryAllocateInfo->sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	m_pMemoryAllocateInfo->pNext = NULL;
	m_pMemoryAllocateInfo->memoryTypeIndex = 0;
	m_pMemoryAllocateInfo->allocationSize = m_pMemoryReq->size;


	//内存类型掩码
	VkFlags vertexBufferMemoryRequirementMask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	//判断内存类型是否能够对应
	Q_ASSERT(memoryTypeFromProperties(vkMemoryProperties,
		m_pMemoryReq->memoryTypeBits,
		vertexBufferMemoryRequirementMask,
		&m_pMemoryAllocateInfo->memoryTypeIndex)
		);
	result = vkAllocateMemory(vkDevice, m_pMemoryAllocateInfo, NULL, &m_VertexBufferMemory);
	assert(result == VK_SUCCESS);
	result = vkBindBufferMemory(vkDevice, m_VertexBuffer, m_VertexBufferMemory, 0);
	assert(result == VK_SUCCESS);

	quint8* pData;
	//将设备内存 映射为 CPU可访问的内存?
	Q_ASSERT(vkMapMemory(vkDevice, m_VertexBufferMemory, 0, m_pMemoryReq->size, 0, reinterpret_cast<void**>(&pData)) == VK_SUCCESS);

	//顶点数据复制到内存
	memcpy(pData, m_VertexData, m_VertexSize);
	memcpy(pData + m_VertexSize, m_Indices.data(), m_IndexSize);
	//解除映射
	vkUnmapMemory(vkDevice, m_VertexBufferMemory);

	m_IsValid = true;
}

void BasicShapes::Draw()
{
	if (!m_IsValid)
		return;
	VkDeviceSize vbOffset = 0;

	PipelineResource* pipelineResource = vkPipelineMap[GetRTTI()];
	vkCmdBindPipeline(vkCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineResource->pipeline);
	vkCmdBindVertexBuffers(vkCmdBuffer, 0, 1, &m_VertexBuffer, &vbOffset);
	vkCmdBindIndexBuffer(vkCmdBuffer, m_VertexBuffer, m_VertexSize, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexed(vkCmdBuffer, m_Indices.size(), 1, 0, 0, 0);
}
