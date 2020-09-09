#include "Drawable.h"

class Quadrangle :public Drawable
{
public:
	Quadrangle()
	{
		m_VertexCount = 4;
		m_VertexData = new float[ParamCount * m_VertexCount];
		ZeroMemory(m_VertexData, ParamCount * m_VertexCount * sizeof(float));
	}
private:
	static VkBufferCreateInfo*   m_pBufferInfo;
	static VkMemoryRequirements* m_pMemoryReq;
	static VkMemoryAllocateInfo* m_pMemoryAllocateInfo;
	static VkDeviceSize          m_BufferByteCount;
	static vector<uint16_t>      m_Indices;
	static VkDeviceSize          m_VertexSize;
	static VkDeviceSize          m_IndexSize;
public:
	void  CreateVertexData()
	{
		if (m_VertexIndex != m_VertexCount)
			return;

		if (!m_pBufferInfo)
		{
			m_Indices = { 0,1,2,2,3,0 };

			m_VertexSize = ParamCount * m_VertexCount * sizeof(float);
			m_IndexSize  = sizeof(uint16_t) * m_Indices.size();
			m_BufferByteCount = m_VertexSize + m_IndexSize;

			m_pBufferInfo = new VkBufferCreateInfo;
			m_pBufferInfo->sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			m_pBufferInfo->pNext = NULL;
			m_pBufferInfo->usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			m_pBufferInfo->size = m_BufferByteCount;
			m_pBufferInfo->queueFamilyIndexCount = 0;
			m_pBufferInfo->pQueueFamilyIndices = NULL;
			m_pBufferInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			m_pBufferInfo->flags = 0;

		}

		VkResult result = vkCreateBuffer(m_vkDevice, m_pBufferInfo, NULL, &m_Buffer);
		assert(result == VK_SUCCESS);

		if (!m_pMemoryReq)
		{
			m_pMemoryReq = new VkMemoryRequirements;
			vkGetBufferMemoryRequirements(m_vkDevice, m_Buffer, m_pMemoryReq);

			m_pMemoryAllocateInfo = new VkMemoryAllocateInfo;
			m_pMemoryAllocateInfo->sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			m_pMemoryAllocateInfo->pNext = NULL;
			m_pMemoryAllocateInfo->memoryTypeIndex = 0;
			m_pMemoryAllocateInfo->allocationSize = m_pMemoryReq->size;
		}

		//内存类型掩码
		VkFlags vertexBufferMemoryRequirementMask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		VkPhysicalDeviceMemoryProperties  memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice, &memoryProperties);//获取第一物理设备的内存属性

		 //判断内存类型是否能够对应
		Q_ASSERT(memoryTypeFromProperties(memoryProperties,
			m_pMemoryReq->memoryTypeBits,
			vertexBufferMemoryRequirementMask,
			&m_pMemoryAllocateInfo->memoryTypeIndex)
		);
		result =vkAllocateMemory(m_vkDevice, m_pMemoryAllocateInfo, NULL, &m_BufferMemory);
		assert(result == VK_SUCCESS);
		result = vkBindBufferMemory(m_vkDevice, m_Buffer, m_BufferMemory, 0);
		assert(result == VK_SUCCESS);

		quint8* pData;
		//将设备内存 映射为 CPU可访问的内存?
		Q_ASSERT(vkMapMemory(m_vkDevice, m_BufferMemory, 0, m_pMemoryReq->size, 0, reinterpret_cast<void**>(&pData)) == VK_SUCCESS);

		//顶点数据复制到内存
		memcpy(pData, m_VertexData, m_VertexSize);
		memcpy(pData+ m_VertexSize, m_Indices.data(),m_IndexSize);
		//解除映射
		vkUnmapMemory(m_vkDevice, m_BufferMemory);

		m_IsValid = true;
	}
	void  Render()
	{
		if (!m_IsValid)
			return;
		VkDeviceSize vbOffset = 0;
		vkCmdBindVertexBuffers(m_CmdBuffer, 0, 1, &m_Buffer, &vbOffset);
		vkCmdBindIndexBuffer(m_CmdBuffer, m_Buffer, m_VertexSize, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(m_CmdBuffer, m_Indices.size(), 1, 0, 0, 0);
	}
};
