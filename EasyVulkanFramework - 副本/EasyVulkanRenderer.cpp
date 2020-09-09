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
	//ѭ�����ٽ������и���ͼ���Ӧ��֡����
	for (unsigned int i = 0; i < vkSwapchainImageCount; i++)
	{
		vkDestroyFramebuffer(vkDevice, vkFrameBuffers[i], NULL);
	}
	free(vkFrameBuffers);
	printf("����֡����ɹ���\n");
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
	printf("������Ȼ�����سɹ�!\n");
}

void EasyVulkanRenderer::DestroySwapChain()
{
	for (uint32_t i = 0; i < vkSwapchainImageCount; i++)
	{
		vkDestroyImageView(vkDevice, vkSwapchainImageViews[i], NULL);
		printf("[����SwapChain ImageView %d �ɹ�]\n", i);
	}
	vkDestroySwapchainKHR(vkDevice, vkSwapChain, NULL);
	printf("����SwapChain�ɹ�!\n");
}


void EasyVulkanRenderer::DestroyCommandBuffer()
{
	//����Ҫ�ͷŵ����������
	VkCommandBuffer cmdBufferArray[1] = { vkCmdBuffer };
	//�ͷ������
	vkFreeCommandBuffers
	(
		vkDevice, //�����߼��豸
		vkCmdPool,//���������
		1,      //Ҫ���ٵ����������
		cmdBufferArray//Ҫ���ٵ����������
	);
	//���������
	vkDestroyCommandPool(vkDevice, vkCmdPool, NULL);
}

void EasyVulkanRenderer::DestroyDevices()
{
	vkDestroyDevice(vkDevice, NULL);
	printf("�߼��豸������ϣ�\n");
}

void EasyVulkanRenderer::DestroyInstance()
{
	vkDestroyInstance(vkInstance, NULL);
	printf("Vulkanʵ���������!\n");
}
