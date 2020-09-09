#include "Triangle.h"

VkBufferCreateInfo*   Triangle::m_pBufferInfo = NULL;
uint32_t              Triangle::m_BufferByteCount = NULL;
VkMemoryRequirements* Triangle::m_pMemoryReq = NULL;
VkMemoryAllocateInfo* Triangle::m_pMemoryAllocateInfo = NULL;