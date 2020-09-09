#pragma once
/*Q_CLASS*/
#include <QSize>
#include <QImage>
#include <qfileinfo.h>
#include <QUuid>

#include "EasyVulkanGlobal.h"


#include "EasyVulkanTextureManager.h"
#include "Transform.h"
#include "BasicShapes/Drawable.h"
#include "EasyVulkanGlobal.h"

#include "DrawTask.h"

struct TextureResource
{
	VkImage                vkImage;
	VkDeviceMemory         vkDeviceMemory;

	VkImageView            vkImageView;
	VkDescriptorImageInfo  vkImageInfo;
	//VkBuffer               vkVertexBuffer;
	//VkDeviceMemory         vkVertexMemory;
	VkDescriptorBufferInfo vkVertexBufferInfo;

	void operator = (TextureResource &textureResource)
	{
		this->vkDeviceMemory = textureResource.vkDeviceMemory;
		this->vkImage = textureResource.vkImage;
		this->vkImageInfo = textureResource.vkImageInfo;
		this->vkImageView = textureResource.vkImageView;
		//this->vkVertexBuffer = textureResource.vkVertexBuffer;
		//this->vkVertexMemory = textureResource.vkVertexMemory;
		//this->vkVertexBufferInfo = textureResource.vkVertexBufferInfo;
	}
};

class TextureBase:public Drawable
{
protected:
	const static int VertexCount = 4;
	TextureBase();

	int                   byteCount;
	unsigned char*        bytes;

	void setImageLayout(VkCommandBuffer cmd, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout, VkImageLayout new_image_layout)
	{
		VkImageMemoryBarrier image_memory_barrier = {};
		image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		image_memory_barrier.pNext = NULL;
		image_memory_barrier.srcAccessMask = 0;
		image_memory_barrier.dstAccessMask = 0;
		image_memory_barrier.oldLayout = old_image_layout;
		image_memory_barrier.newLayout = new_image_layout;
		image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		image_memory_barrier.image = image;
		image_memory_barrier.subresourceRange.aspectMask = aspectMask;
		image_memory_barrier.subresourceRange.baseMipLevel = 0;
		image_memory_barrier.subresourceRange.levelCount = 1;
		image_memory_barrier.subresourceRange.baseArrayLayer = 0;
		image_memory_barrier.subresourceRange.layerCount = 1;
		if (old_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			image_memory_barrier.srcAccessMask =
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}
		if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		}
		if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		if (old_image_layout == VK_IMAGE_LAYOUT_PREINITIALIZED) {
			image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		}
		if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			image_memory_barrier.srcAccessMask =
				VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}
		if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			image_memory_barrier.dstAccessMask =
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}
		if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			image_memory_barrier.dstAccessMask =
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
		VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		vkCmdPipelineBarrier(cmd, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
	}

	void                  ensure()
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
			buf_info.size = this->byteCount;
			buf_info.queueFamilyIndexCount = 0;
			buf_info.pQueueFamilyIndices = NULL;
			buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			buf_info.flags = 0;
			VkResult result = vkCreateBuffer(vkDevice, &buf_info, NULL, &tempBuf);
			VkAssert(result == VK_SUCCESS, "vkCreateBuffer");

			VkMemoryRequirements mem_reqs;
			vkGetBufferMemoryRequirements(vkDevice, tempBuf, &mem_reqs);
			VkAssert(texture->m_DataByteCount <= mem_reqs.size, "EnsureBufferMemoryRequirements");

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
			VkAssert(result == VK_SUCCESS, "vkAllocateMemory");

			uint8_t* pData;
			result = vkMapMemory(vkDevice, memTemp, 0, mem_reqs.size, 0, (void**)&pData);
			VkAssert(result == VK_SUCCESS, "vkMapMemory");

			memcpy(pData, this->bytes, this->byteCount);
			vkUnmapMemory(vkDevice, memTemp);
			result = vkBindBufferMemory(vkDevice, tempBuf, memTemp, 0);
			VkAssert(result == VK_SUCCESS, "vkBindBufferMemory");

			VkImageCreateInfo image_create_info = {};
			image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_create_info.pNext = NULL;
			image_create_info.imageType = VK_IMAGE_TYPE_2D;
			image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
			image_create_info.extent.width = this->size.width();
			image_create_info.extent.height = this->size.height();
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
			VkAssert(result == VK_SUCCESS, "vkCreateImage");
			resource.vkImage = textureImage;

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
			bufferCopyRegion.imageExtent.width = this->size.width();
			bufferCopyRegion.imageExtent.height = this->size.height();
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
			image_create_info.extent.width = this->size.width();
			image_create_info.extent.height = this->size.height();
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
			VkAssert(result == VK_SUCCESS, "vkCreateImage");
			resource.vkImage = textureImage;

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
			memcpy(pData, this->bytes, mem_reqs.size);
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
		resource.vkImageView = viewTexture;
		VkDescriptorImageInfo texImageInfo; //构建图像描述信息结构体实例

		texImageInfo.imageView = viewTexture;//采用的图像视图
		texImageInfo.sampler = EasyVulkanTextureManager::GetStandardSampler(); //采用的采样器
		texImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL; //图像布局
		resource.vkImageInfo = texImageInfo;//添加到纹理图像描述信息列表

		this->resource = resource;
	}
public:
	TextureResource       resource;
	QSize                 size;
	QSize                 actualSize;
	std::string           textureName;

	void                  draw()
	{
		auto pipelineResource = vkPipelineMap[0];
		vkCmdBindPipeline(vkCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineResource->pipeline);
		vkCmdBindDescriptorSets(vkCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineResource->pipelineLayout, 0, 1,
			&pipelineResource->vkDescriptorSets[DescriptorIndex], 0, NULL);
		const VkDeviceSize offsetsVertex[1] = { 0 };
		vkCmdBindVertexBuffers
		(
			vkCmdBuffer,
			0,
			1,
			&(m_VertexBuffer),
			offsetsVertex
			);
		vkCmdDraw(vkCmdBuffer, 6, 1, 0, 0);
	}
	virtual void          createVertexData() {}
	friend class EasyVulkanTextureManager;
};

class Sprite :public TextureBase
{
private:
	void createVertexData()
	{
		int w = this->size.width();
		int h = this->size.height();

		float* vdataIn;

		float scale = (float)h / (float)w;


		vdataIn = new float[30]{ //顶点数据数组
			15,15 * scale,0, 1,0,   -15,15 * scale,0, 0,0,   -15,-15 * scale,0, 0,1,	//第1个三角形的数据
			15,15 * scale,0, 1,0,   -15,-15 * scale,0, 0,1,   15,-15 * scale,0, 1,1     //第2个三角形的数据
		};


		/*this->m_VertexRect[0] = -15;
		this->m_VertexRect[1] = 15;
		this->m_VertexRect[2] = -15 * scale;
		this->m_VertexRect[3] = 15 * scale
		
		else
		{
			float scale = (texCoorY2 - texCoorY1) / (texCoorX2 - texCoorX1);

			float basic = 10;
			vdataIn = new float[30]{ //顶点数据数组
				basic / scale,basic,0, texCoorX2,texCoorY1,   -basic / scale,basic,0, texCoorX1,texCoorY1,   -basic / scale,-basic,0, texCoorX1,texCoorY2,	//第1个三角形的数据
				basic / scale,basic,0, texCoorX2,texCoorY1,   -basic / scale,-basic,0, texCoorX1,texCoorY2,  basic / scale,-basic,0, texCoorX2,texCoorY2//第2个三角形的数据
			};

			texture->m_VertexRect[0] = -basic / scale;
			texture->m_VertexRect[1] = basic / scale;
			texture->m_VertexRect[2] = basic;
			texture->m_VertexRect[3] = basic;

		}*/
		VkBufferCreateInfo buf_info = {};
		buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buf_info.pNext = NULL;
		buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		buf_info.size = 30 * sizeof(float);
		buf_info.queueFamilyIndexCount = 0;
		buf_info.pQueueFamilyIndices = NULL;
		buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		buf_info.flags = 0;
		VkResult result = vkCreateBuffer(vkDevice, &buf_info, NULL, &(this->m_VertexBuffer));
		assert(result == VK_SUCCESS);
		VkMemoryRequirements mem_reqs;
		vkGetBufferMemoryRequirements(vkDevice, this->m_VertexBuffer, &mem_reqs);
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
		result = vkAllocateMemory(vkDevice, &alloc_info, NULL, &(this->m_VertexBufferMemory));
		assert(result == VK_SUCCESS);
		uint8_t* pData;
		result = vkMapMemory(vkDevice, this->m_VertexBufferMemory, 0, mem_reqs.size, 0, (void**)&pData);
		assert(result == VK_SUCCESS);
		memcpy(pData, vdataIn, 30 * sizeof(float));
		vkUnmapMemory(vkDevice, this->m_VertexBufferMemory);
		result = vkBindBufferMemory(vkDevice, this->m_VertexBuffer,this->m_VertexBufferMemory, 0);
		assert(result == VK_SUCCESS);

		this->resource.vkVertexBufferInfo.buffer = this->m_VertexBuffer;
		this->resource.vkVertexBufferInfo.offset = 0;
		this->resource.vkVertexBufferInfo.range = mem_reqs.size;
	}
	void create_Internal(unsigned char* bytes)
	{
		this->byteCount = 4 * size.width() * size.height() * sizeof(float);

		this->createVertexData();
		this->ensure();
	}

public:

	bool create(std::string fileName)
	{
		QImage image(fileName.c_str());

		if (image.isNull())
			return false;

		this->actualSize = image.size();

		int x = ceil(log2(actualSize.width()));
		int y = ceil(log2(actualSize.height()));

		int z = max(x, y);
		int u = pow(2, z);

		this->size = QSize(u, u);

		image = image.scaled(u,u);

		this->create_Internal(image.bits());

		return true;
	}
};

class SingleText :public TextureBase
{
private:
	float coordinate[4];
	float vertexRect[4];

	int BearingX;
	int BearingY;
	int Advance;

	void createVertexData()
	{
		int w = this->size.width();
		int h = this->size.height();

		float* vdataIn;

		float scale = (coordinate[2]- coordinate[3]) / (coordinate[1]- coordinate[0]);

		float basic = 10;

		vdataIn = new float[30]{ //顶点数据数组
			basic / scale,basic,0, coordinate[1],coordinate[2],   -basic / scale,basic,0, coordinate[0],coordinate[2],   -basic / scale,-basic,0, coordinate[0],coordinate[3],	//第1个三角形的数据
			basic / scale,basic,0, coordinate[1],coordinate[2],   -basic / scale,-basic,0, coordinate[0],coordinate[3],  basic / scale,-basic,0, coordinate[1],coordinate[3]    //第2个三角形的数据
		};

		vertexRect[0] = -basic / scale;
		vertexRect[1] = basic / scale;
		vertexRect[2] = basic;
		vertexRect[3] = basic;

		VkBufferCreateInfo buf_info = {};
		buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buf_info.pNext = NULL;
		buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		buf_info.size = 30 * sizeof(float);
		buf_info.queueFamilyIndexCount = 0;
		buf_info.pQueueFamilyIndices = NULL;
		buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		buf_info.flags = 0;
		VkResult result = vkCreateBuffer(vkDevice, &buf_info, NULL, &(this->m_VertexBuffer));
		assert(result == VK_SUCCESS);
		VkMemoryRequirements mem_reqs;
		vkGetBufferMemoryRequirements(vkDevice, this->m_VertexBuffer, &mem_reqs);
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
		result = vkAllocateMemory(vkDevice, &alloc_info, NULL, &(this->m_VertexBufferMemory));
		assert(result == VK_SUCCESS);
		uint8_t* pData;
		result = vkMapMemory(vkDevice, this->m_VertexBufferMemory, 0, mem_reqs.size, 0, (void**)&pData);
		assert(result == VK_SUCCESS);
		memcpy(pData, vdataIn, 30 * sizeof(float));
		vkUnmapMemory(vkDevice, this->m_VertexBufferMemory);
		result = vkBindBufferMemory(vkDevice, this->m_VertexBuffer, this->m_VertexBufferMemory, 0);
		assert(result == VK_SUCCESS);

		this->resource.vkVertexBufferInfo.buffer = this->m_VertexBuffer;
		this->resource.vkVertexBufferInfo.offset = 0;
		this->resource.vkVertexBufferInfo.range = mem_reqs.size;
	}
	void expand(int n, bool drawBorder = false);
public:
	void create(unsigned char* bytes, int width, int height, int BearingX, int BearingY, int advance,float coordinate[])
	{
		this->actualSize = QSize(width, height);

		int x = ceil(log2(actualSize.width()));
		int y = ceil(log2(actualSize.height()));

		int z = max(x, y);
		int u = pow(2, z);

		size = QSize(u, u);

		this->byteCount = 4 * u * u * sizeof(float);

		this->BearingX = BearingX;
		this->BearingY = BearingY;
		this->Advance = advance;

		memcpy(this->coordinate, coordinate, 4 * sizeof(float));

		this->expand(u);
		this->createVertexData();
		this->ensure();
	}
};

class SpriteObject:public DrawableObject
{
public:
	QColor color;
	bool   flipX;
	bool   flipY;
	
	std::string spriteName;
	
	SpriteObject(std::string fileName)
	{
		this->spriteName = fileName;
		DrawTaskQueue::addObject(this);
	}
	int  GetRTTI() override
	{
		return RTTI_DRAWABLE_SPRITE;
	}
};