#include "EasyVulkanTextureManager.h"

vector<Texture*> EasyVulkanTextureManager::textures;
vector<VkSampler> EasyVulkanTextureManager::vkSamplers;
VkDevice EasyVulkanTextureManager::vkDevice;
VkPhysicalDevice EasyVulkanTextureManager::vkGPU;
VkPhysicalDeviceMemoryProperties EasyVulkanTextureManager::vkMemoryProperties;
VkCommandBuffer EasyVulkanTextureManager::vkCmdBuffer;
VkQueue EasyVulkanTextureManager::vkQueue;
VkPipeline EasyVulkanTextureManager::vkPipeline = VK_NULL_HANDLE;
VkPipelineLayout EasyVulkanTextureManager::vkPipelineLayout = VK_NULL_HANDLE;
QMap<QString, Texture*> EasyVulkanTextureManager::m_TextureMap = QMap<QString, Texture*>();
VkDescriptorSet* EasyVulkanTextureManager::vkpDescriptorSet = NULL;

Texture* EasyVulkanTextureManager::getTexturesByName(QString name)
{
	return m_TextureMap[name];
}

vector<Texture*> EasyVulkanTextureManager::getTexturesList()
{
	return m_TextureMap.values().toVector().toStdVector();
}

int EasyVulkanTextureManager::getTextureCount()
{
	return m_TextureMap.size();
}

VkDescriptorImageInfo* EasyVulkanTextureManager::getImageInfoByIndex(int index)
{
	if (m_TextureMap.isEmpty())
		return NULL;
	Texture* texture = m_TextureMap.values().at(index);
	TextureResource* resource = &(texture->m_Resource);
	return &resource->vkImageInfo;
}

VkSampler EasyVulkanTextureManager::GetStandardSampler()
{
	return vkSamplers[0];
}

void EasyVulkanTextureManager::Init(VkDevice& device, VkPhysicalDevice& gpu, VkPhysicalDeviceMemoryProperties& memoryroperties, VkCommandBuffer& cmdBuffer, VkQueue& queueGraphics)
{
	vkDevice = device;
	vkGPU = gpu;
	vkMemoryProperties = memoryroperties;
	vkCmdBuffer = cmdBuffer;
	vkQueue = queueGraphics;

	InitSample();
}

void EasyVulkanTextureManager::DestoryAll()
{
	for (int i = 0; i < SAMPLER_COUNT; i++)
	{
		vkDestroySampler(vkDevice, vkSamplers[i], NULL);
	}
	auto list = m_TextureMap.keys();
	for (int i = 0; i < textures.size(); i++)
	{
		TextureResource *resource = &(textures[i]->m_Resource);
		vkDestroyImageView(vkDevice, resource->vkImageView, NULL);
		vkDestroyImage(vkDevice, resource->vkImage, NULL);
		vkFreeMemory(vkDevice, resource->vkDeviceMemory, NULL);
		delete resource;
		resource = NULL;
	}

	for (int i = 0; i < textures.size(); i++) 
	{
		delete textures[i];
	}
}

int EasyVulkanTextureManager::GetIndexByName(QString textureName)
{
	int result = -1;
	for (int i = 0;  i < textures.size(); i++)
	{
		Texture* texture = textures[i];
		if (texture[i].GetTextureName()!= textureName)
		{
			result = i;
			break;
		}
	}
	VkAssert(result != -1,"GetIndexByName");
	return result;
}

void EasyVulkanTextureManager::CreateVertexData(Texture* texture,bool useValidRect)
{
	int w = texture->m_Size.width();
	int h = texture->m_Size.height();


	float* validRect = texture->GetTexCoorRect();
	float texCoorX1 = validRect[0];
	float texCoorX2 = validRect[1];
	float texCoorY1 = validRect[2];
	float texCoorY2 = validRect[3];
	float* vdataIn;
	if (!useValidRect)
	{
		float scale = (float)h / (float)w;
		vdataIn = new float[30]{ //顶点数据数组
			15,15 * scale,0, 1,0,   -15,15 * scale,0, 0,0,   -15,-15 * scale,0, 0,1,	//第1个三角形的数据
			15,15 * scale,0, 1,0,   -15,-15 * scale,0, 0,1,  15,-15 * scale,0, 1,1//第2个三角形的数据
		};

		
		texture->m_VertexRect[0] = -15;
		texture->m_VertexRect[1] = 15;
		texture->m_VertexRect[2] = -15*scale;
		texture->m_VertexRect[3] = 15*scale;
	}
	else
	{
		float scale = (texCoorY2 - texCoorY1) / (texCoorX2 - texCoorX1);

		float basic = 10;
		vdataIn = new float[30]{ //顶点数据数组
			basic / scale,basic,0, texCoorX2,texCoorY1,   -basic / scale,basic,0, texCoorX1,texCoorY1,   -basic / scale,-basic,0, texCoorX1,texCoorY2,	//第1个三角形的数据
			basic / scale,basic,0, texCoorX2,texCoorY1,   -basic / scale,-basic,0, texCoorX1,texCoorY2,  basic / scale,-basic,0, texCoorX2,texCoorY2//第2个三角形的数据
		};

		texture->m_VertexRect[0] = -basic /scale;
		texture->m_VertexRect[1] = basic /scale;
		texture->m_VertexRect[2] = basic;
		texture->m_VertexRect[3] = basic;
		
	}	
	VkBufferCreateInfo buf_info = {};
	buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buf_info.pNext = NULL;
	buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	buf_info.size = 30 * sizeof(float);
	buf_info.queueFamilyIndexCount = 0;
	buf_info.pQueueFamilyIndices = NULL;
	buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buf_info.flags = 0;
	VkResult result = vkCreateBuffer(vkDevice, &buf_info, NULL, &(texture->m_Resource.vkVertexBuffer));
	assert(result == VK_SUCCESS);
	VkMemoryRequirements mem_reqs;
	vkGetBufferMemoryRequirements(vkDevice, (texture->m_Resource.vkVertexBuffer), &mem_reqs);
	assert(dataByteCount <= mem_reqs.size);
	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.pNext = NULL;
	alloc_info.memoryTypeIndex = 0;
	alloc_info.allocationSize = mem_reqs.size;
	VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	bool flag = memoryTypeFromProperties(vkMemoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
	assert(flag);
	printf("确定内存类型成功 类型索引为%d\n", alloc_info.memoryTypeIndex);
	result = vkAllocateMemory(vkDevice, &alloc_info, NULL, &(texture->m_Resource.vkVertexMemory));
	assert(result == VK_SUCCESS);
	uint8_t* pData;
	result = vkMapMemory(vkDevice, texture->m_Resource.vkVertexMemory, 0, mem_reqs.size, 0, (void**)&pData);
	assert(result == VK_SUCCESS);
	memcpy(pData, vdataIn, 30 * sizeof(float));
	vkUnmapMemory(vkDevice, texture->m_Resource.vkVertexMemory);
	result = vkBindBufferMemory(vkDevice, texture->m_Resource.vkVertexBuffer, texture->m_Resource.vkVertexMemory, 0);
	assert(result == VK_SUCCESS);
	texture->m_Resource.vkVertexBufferInfo.buffer = texture->m_Resource.vkVertexBuffer;
	texture->m_Resource.vkVertexBufferInfo.offset = 0;
	texture->m_Resource.vkVertexBufferInfo.range = mem_reqs.size;
}

void EasyVulkanTextureManager::EusureTexture(Texture *texture)
{
	VkFormatProperties formatProps;
	TextureResource resource;
	vkGetPhysicalDeviceFormatProperties(vkGPU, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
	bool needStaging = (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)) ? true : false;
	printf("TextureManager %s", (needStaging ? "不能使用线性瓦片纹理" : "能使用线性瓦片纹理"));
	if (needStaging)
	{
		VkBuffer tempBuf;
		VkBufferCreateInfo buf_info = {};
		buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buf_info.pNext = NULL;
		buf_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		buf_info.size = texture->m_DataByteCount;
		buf_info.queueFamilyIndexCount = 0;
		buf_info.pQueueFamilyIndices = NULL;
		buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		buf_info.flags = 0;
		VkResult result = vkCreateBuffer(vkDevice, &buf_info, NULL, &tempBuf);
		VkAssert(result == VK_SUCCESS,"vkCreateBuffer");

		VkMemoryRequirements mem_reqs;
		vkGetBufferMemoryRequirements(vkDevice, tempBuf, &mem_reqs);
		VkAssert(texture->m_DataByteCount <= mem_reqs.size,"EnsureBufferMemoryRequirements");

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.pNext = NULL;
		alloc_info.memoryTypeIndex = 0;
		alloc_info.allocationSize = mem_reqs.size;
		VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		bool flag = memoryTypeFromProperties(vkMemoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
		if (flag)
		{
			printf("确定内存类型成功 类型索引为%d", alloc_info.memoryTypeIndex);
		}
		else
		{
			printf("确定内存类型失败!");
		}
		VkDeviceMemory memTemp;
		result = vkAllocateMemory(vkDevice, &alloc_info, NULL, &memTemp);
		VkAssert(result == VK_SUCCESS,"vkAllocateMemory");

		uint8_t* pData;
		result = vkMapMemory(vkDevice, memTemp, 0, mem_reqs.size, 0, (void**)&pData);
		VkAssert(result == VK_SUCCESS,"vkMapMemory");

		memcpy(pData, texture->m_pTextureData, texture->m_DataByteCount);
		vkUnmapMemory(vkDevice, memTemp);
		result = vkBindBufferMemory(vkDevice, tempBuf, memTemp, 0);
		VkAssert(result == VK_SUCCESS,"vkBindBufferMemory");

		VkImageCreateInfo image_create_info = {};
		image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_create_info.pNext = NULL;
		image_create_info.imageType = VK_IMAGE_TYPE_2D;
		image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
		image_create_info.extent.width = texture->m_Size.width();
		image_create_info.extent.height = texture->m_Size.height();
		image_create_info.extent.depth = 1;
		image_create_info.mipLevels = 1;
		image_create_info.arrayLayers = 1;
		image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		image_create_info.queueFamilyIndexCount = 0;
		image_create_info.pQueueFamilyIndices = NULL;
		image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_create_info.flags = 0;
		VkImage textureImage;
		result = vkCreateImage(vkDevice, &image_create_info, NULL, &textureImage);
		VkAssert(result == VK_SUCCESS,"vkCreateImage");
		resource.vkImage= textureImage;

		VkMemoryAllocateInfo mem_alloc = {};
		mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mem_alloc.pNext = NULL;
		mem_alloc.allocationSize = 0;
		mem_alloc.memoryTypeIndex = 0;
		vkGetImageMemoryRequirements(vkDevice, textureImage, &mem_reqs);
		mem_alloc.allocationSize = mem_reqs.size;
		flag = memoryTypeFromProperties(vkMemoryProperties, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex);

		VkDeviceMemory textureMemory;
		result = vkAllocateMemory(vkDevice, &mem_alloc, NULL, &(textureMemory));
		resource.vkDeviceMemory = textureMemory;
		result = vkBindImageMemory(vkDevice, textureImage, textureMemory, 0);
		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyRegion.imageSubresource.mipLevel = 0;
		bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageExtent.width = texture->m_Size.width();
		bufferCopyRegion.imageExtent.height = texture->m_Size.height();
		bufferCopyRegion.imageExtent.depth = 1;
		bufferCopyRegion.bufferOffset = 0;

		VkCommandBufferBeginInfo cmd_buf_info = {};
		cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmd_buf_info.pNext = NULL;
		cmd_buf_info.flags = 0;
		cmd_buf_info.pInheritanceInfo = NULL;
		const VkCommandBuffer cmd_bufs[] = { vkCmdBuffer };
		VkSubmitInfo submit_info[1] = {};
		submit_info[0].pNext = NULL;
		submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info[0].waitSemaphoreCount = 0;
		submit_info[0].pWaitSemaphores = VK_NULL_HANDLE;
		submit_info[0].pWaitDstStageMask = VK_NULL_HANDLE;
		submit_info[0].commandBufferCount = 1;
		submit_info[0].pCommandBuffers = cmd_bufs;
		submit_info[0].signalSemaphoreCount = 0;
		submit_info[0].pSignalSemaphores = NULL;
		VkFenceCreateInfo fenceInfo;
		VkFence copyFence;
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.pNext = NULL;
		fenceInfo.flags = 0;

		vkCreateFence(vkDevice, &fenceInfo, NULL, &copyFence);
		vkResetCommandBuffer(vkCmdBuffer, 0);
		result = vkBeginCommandBuffer(vkCmdBuffer, &cmd_buf_info);
		setImageLayout(vkCmdBuffer, textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		vkCmdCopyBufferToImage(vkCmdBuffer, tempBuf, textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);
		setImageLayout(vkCmdBuffer, textureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		result = vkEndCommandBuffer(vkCmdBuffer);
		result = vkQueueSubmit(vkQueue, 1, submit_info, copyFence);
		do
		{
			result = vkWaitForFences(vkDevice, 1, &copyFence, VK_TRUE, 100000000);
		} while (result == VK_TIMEOUT);
		vkDestroyBuffer(vkDevice, tempBuf, NULL);
		vkFreeMemory(vkDevice, memTemp, NULL);
		vkDestroyFence(vkDevice, copyFence, NULL);
	}
	else
	{
		VkImageCreateInfo image_create_info = {};
		image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_create_info.pNext = NULL;
		image_create_info.imageType = VK_IMAGE_TYPE_2D;
		image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
		image_create_info.extent.width = texture->m_Size.width(); //必须都是2的次方？
		image_create_info.extent.height = texture->m_Size.height();
		image_create_info.extent.depth = 1;
		image_create_info.mipLevels = 1;
		image_create_info.arrayLayers = 1;
		image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
		image_create_info.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
		image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
		image_create_info.queueFamilyIndexCount = 0;
		image_create_info.pQueueFamilyIndices = NULL;
		image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_create_info.flags = 0;
		VkImage textureImage;
		VkResult result = vkCreateImage(vkDevice, &image_create_info, NULL, &textureImage);
		VkAssert(result == VK_SUCCESS,"vkCreateImage");
		resource.vkImage= textureImage;
	
		VkMemoryAllocateInfo mem_alloc = {};
		mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mem_alloc.pNext = NULL;
		mem_alloc.allocationSize = 0;
		mem_alloc.memoryTypeIndex = 0;
		VkMemoryRequirements mem_reqs;
		vkGetImageMemoryRequirements(vkDevice, textureImage, &mem_reqs);
		mem_alloc.allocationSize = mem_reqs.size;
		bool flag = memoryTypeFromProperties(vkMemoryProperties, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &mem_alloc.memoryTypeIndex);
		VkDeviceMemory textureMemory;
		result = vkAllocateMemory(vkDevice, &mem_alloc, NULL, &(textureMemory));
		VkAssert(result == VK_SUCCESS, "vkAllocateMemory");
		resource.vkDeviceMemory = textureMemory;

		result = vkBindImageMemory(vkDevice, textureImage, textureMemory, 0);
		VkAssert(result == VK_SUCCESS, "vkBindImageMemory");

		uint8_t* pData;
		vkMapMemory(vkDevice, textureMemory, 0, mem_reqs.size, 0, (void**)(&pData));
		memcpy(pData, texture->m_pTextureData, mem_reqs.size);
		vkUnmapMemory(vkDevice, textureMemory);
	}
	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.pNext = NULL;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
	view_info.components.r = VK_COMPONENT_SWIZZLE_R;
	view_info.components.g = VK_COMPONENT_SWIZZLE_G;
	view_info.components.b = VK_COMPONENT_SWIZZLE_B;
	view_info.components.a = VK_COMPONENT_SWIZZLE_A;
	view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;
	view_info.image = resource.vkImage; 
	VkImageView viewTexture;
	VkResult result = vkCreateImageView(vkDevice, &view_info, NULL, &viewTexture);
	resource.vkImageView =  viewTexture;
	VkDescriptorImageInfo texImageInfo; //构建图像描述信息结构体实例

	texImageInfo.imageView = viewTexture;//采用的图像视图
	texImageInfo.sampler = vkSamplers[0]; //采用的采样器
	texImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL; //图像布局
	resource.vkImageInfo= texImageInfo;//添加到纹理图像描述信息列表

	texture->m_Resource = resource;
	m_TextureMap[texture->m_Name] = texture;
}

Texture* EasyVulkanTextureManager::CreateTexture(QString path)
{
	QFile file(path);
	if (file.exists())
	{
		Texture* texture = new Texture();
		texture->Create(path);

		EasyVulkanTextureManager::EusureTexture(texture);
		EasyVulkanTextureManager::CreateVertexData(texture);
		return texture;
	}

	else {
		return NULL;
	}
}

Texture* EasyVulkanTextureManager::CreateTexture(uchar* data,int width, int height, QImage::Format format)
{
	if (data == nullptr)
	{
		return NULL; 
	}
	else {
		Texture* texture = new Texture();
		texture->Create(data, width, height, format);
		EasyVulkanTextureManager::EusureTexture(texture);
		EasyVulkanTextureManager::CreateVertexData(texture,true);
		return texture;
	}
}

void EasyVulkanTextureManager::InitSample()
{
	VkSamplerCreateInfo samplerCreateInfo = {}; //构建采样器创建信息结构体实例
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO; //结构体的类型
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR; //放大时的纹理采样方式
	samplerCreateInfo.minFilter = VK_FILTER_NEAREST; //缩小时的纹理采样方式
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST; //mipmap 模式
	for (int i = 0; i < SAMPLER_COUNT; i++) { //循环设置各种拉伸方式
		if (i == 0) { //设置为重复拉伸方式
			samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		}
		else if (i == 1) { //设置为截取拉伸方式
			samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		}
		else if (i == 2) { //设置为镜像重复拉伸方式
			samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		}
		else if (i == 3) { //设置为边框拉伸方式
			samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		}
		samplerCreateInfo.mipLodBias = 0.0; //mipmap 时的 Lod 调整值
		samplerCreateInfo.anisotropyEnable = VK_FALSE; //是否启用各向异性过滤
		samplerCreateInfo.maxAnisotropy = 1; //各向异性最大过滤值
		samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER; //纹素数据比较操作
		samplerCreateInfo.minLod = 0.0; //最小 LOD 值
		samplerCreateInfo.maxLod = 0.0; //最大 LOD 值
		samplerCreateInfo.compareEnable = VK_FALSE; //是否开启比较功能
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;//预置边框颜色
		VkSampler samplerTexture; //声明采样器对象
		VkResult result = vkCreateSampler(vkDevice, //创建采样器
			&samplerCreateInfo, NULL, &samplerTexture);
		vkSamplers.push_back(samplerTexture); //将采样器加入列表
	}
}
