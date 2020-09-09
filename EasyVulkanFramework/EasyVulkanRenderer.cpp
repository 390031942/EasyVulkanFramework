#include "EasyVulkanRenderer.h"

void EasyVulkanRenderer::DestroyFence()
{
	vkDestroyFence(vkDevice, vkTaskFinishFence, NULL);
}

void EasyVulkanRenderer::DestroyPipeline()
{

}

void EasyVulkanRenderer::DestroyDrawableObject()
{
	delete m_triangle;
}

void EasyVulkanRenderer::DestroyFrameBuffer()
{
	//循环销毁交换链中各个图像对应的帧缓冲
	for (unsigned int i = 0; i < vkSwapchainImageCount; i++)
	{
		vkDestroyFramebuffer(vkDevice, vkFrameBuffers[i], NULL);
	}
	free(vkFrameBuffers);
	printf("销毁帧缓冲成功！\n");
}

void EasyVulkanRenderer::DestroyRenderPass()
{
	vkDestroyRenderPass(vkDevice,vkRenderPass, NULL);
	vkDestroySemaphore(vkDevice, vkImageAcquiredSemaphore, NULL);
}

void EasyVulkanRenderer::DestroyDepthBuffer()
{
	vkDestroyImageView(vkDevice, vkDepthImageView, NULL);
	vkDestroyImage(vkDevice, vkDepthImage, NULL);
	vkFreeMemory(vkDevice, vkMemoryDepth, NULL);
	printf("销毁深度缓冲相关成功!\n");
}

void EasyVulkanRenderer::DestroySwapChain()
{
	for (uint32_t i = 0; i < vkSwapchainImageCount; i++)
	{
		vkDestroyImageView(vkDevice, vkSwapchainImageViews[i], NULL);
		printf("[销毁SwapChain ImageView %d 成功]\n", i);
	}
	vkDestroySwapchainKHR(vkDevice, vkSwapChain, NULL);
	printf("销毁SwapChain成功!\n");
}


void EasyVulkanRenderer::DestroyCommandBuffer()
{
	//创建要释放的命令缓冲数组
	VkCommandBuffer cmdBufferArray[1] = { vkCmdBuffer };
	//释放命令缓冲
	vkFreeCommandBuffers
	(
		vkDevice, //所属逻辑设备
		vkCmdPool,//所属命令池
		1,      //要销毁的命令缓冲数量
		cmdBufferArray//要销毁的命令缓冲数组
	);
	//销毁命令池
	vkDestroyCommandPool(vkDevice, vkCmdPool, NULL);
}

void EasyVulkanRenderer::DestroyDevices()
{
	vkDestroyDevice(vkDevice, NULL);
	printf("逻辑设备销毁完毕！\n");
}

void EasyVulkanRenderer::DestroyInstance()
{
	vkDestroyInstance(vkInstance, NULL);
	printf("Vulkan实例销毁完毕!\n");
}
