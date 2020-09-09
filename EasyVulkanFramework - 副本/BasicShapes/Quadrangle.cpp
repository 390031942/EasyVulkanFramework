#include "Quadrangle.h"

VkBufferCreateInfo*   Quadrangle::m_pBufferInfo = NULL;
VkDeviceSize          Quadrangle::m_BufferByteCount = NULL;
VkMemoryRequirements* Quadrangle::m_pMemoryReq = NULL;
VkMemoryAllocateInfo* Quadrangle::m_pMemoryAllocateInfo = NULL;

vector<uint16_t>      Quadrangle::m_Indices;
VkDeviceSize          Quadrangle::m_VertexSize;
VkDeviceSize          Quadrangle::m_IndexSize;