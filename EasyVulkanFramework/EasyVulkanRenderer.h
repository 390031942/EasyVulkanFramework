#pragma once

#include "EasyVulkanGlobal.h"
#include "EasyVulkanMemoryUtility.h"
#include "EasyVulkanPipeline.h"
#include "EasyVulkanTextureManager.h"
#include "BasicShapes/Triangle.h"
#include "Label.h"

using namespace std;

#define FENCE_TIMEOUT 100000000

class EasyVulkanRenderer
{
private:
	vector<const char*> vkInstanceExtensionNames;
	uint32_t vkQueueFamilyCount;
	std::vector<VkQueueFamilyProperties> vkQueueFamilyprops;
	uint32_t vkQueueGraphicsFamilyIndex;
	vector<const char*> vkDeviceExtensionNames;
	VkCommandBufferBeginInfo vkCmdBufferInfo;
	VkCommandPool vkCmdPool;
	VkSubmitInfo vkSubmitInfo[1];
	HWND hWnd;
	HINSTANCE hInst;
	VkSurfaceKHR vkSurface;
	VkQueue vkQueueGraphics;
	uint32_t vkQueuePresentFamilyIndex;
	std::vector<VkFormat> vkformats;
	VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
	uint32_t vkPresentModeCount;
	std::vector<VkPresentModeKHR> vkPresentModes;
	VkExtent2D vkSwapchainExtent;
	VkSwapchainKHR vkSwapChain;
	uint32_t vkSwapchainImageCount;
	std::vector<VkImage> vkSwapchainImages;
	std::vector<VkImageView> vkSwapchainImageViews;
	VkFormat vkDepthFormat;
	VkFormatProperties vkDepthFormatProps;
	VkImage vkDepthImage;
	VkDeviceMemory vkMemoryDepth;
	VkImageView vkDepthImageView;
	VkSemaphore vkImageAcquiredSemaphore;
	uint32_t vkCurrentBuffer;
	VkClearValue vkClearValues[2];
	VkRenderPassBeginInfo vkRenderPassBeginInfo;
	VkFence vkTaskFinishFence;
	VkPresentInfoKHR vkPresent;
	VkFramebuffer* vkFrameBuffers;

	TextureDrawPipeline *Pipeline;
	BasicDrawPipeline* Basic_Pipeline;

	Triangle* m_triangle;
	Texture*  m_Texture;
	Texture*  m_Texture_New;
	Texture*  m_Texture_vulkan;
	Label*    m_Label;

public:
	void SetWindowHandle(HWND hWnd)
	{
		this->hWnd = hWnd;
		hInst = (HINSTANCE)GetModuleHandle(NULL);
	}
	void CreateVkInstance()
	{
		//��ʼ��������չ�б�����������չ��Android������Ⱦ����Ҫ��
		vkInstanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);      //�˴���ֲWindows�������
		vkInstanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);//�˴���ֲWindows��Ҫ��ȡVK_KHR_SURFACE_EXTENSION_NAME��չ
		vkInstanceExtensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		VkApplicationInfo app_info = {};//����Ӧ����Ϣ�ṹ��ʵ��
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;//�ṹ�������
		app_info.pNext = NULL;//�Զ������ݵ�ָ��
		app_info.pApplicationName = VulkanConfig::AppInfo::AppName;//Ӧ�õ�����
		app_info.applicationVersion = VulkanConfig::AppInfo::AppVersion;//Ӧ�õİ汾��
		app_info.pEngineName = VulkanConfig::AppInfo::EngineName;//Ӧ�õ���������
		app_info.engineVersion = VulkanConfig::AppInfo::EngineVersion;//Ӧ�õ�����汾��
		app_info.apiVersion = VulkanConfig::AppInfo::ApiVersion;//ʹ�õ�Vulkanͼ��Ӧ�ó���API�汾

		std::vector<const char*> exceptedLayerNames;//������������֤Layer�����б�
		exceptedLayerNames.push_back("VK_LAYER_LUNARG_core_validation");
		exceptedLayerNames.push_back("VK_LAYER_LUNARG_parameter_validation");
		exceptedLayerNames.push_back("VK_LAYER_LUNARG_standard_validation");
		VkDeviceLayerAndExtension layerAndExtension = EasyVulkanConsole::GetLayerProperties(exceptedLayerNames);	//��ȡ֧�����


		for (auto s : layerAndExtension.extensionNames) {//���������չ������չ�����б�
			vkInstanceExtensionNames.push_back((*s).c_str());
		}
		exceptedLayerNames.clear();	//�����֤Layer�����б�
		for (auto s : layerAndExtension.layerNames) {//����֧�ֵ���֤Layer���Ƽ���Layer�����б�
			exceptedLayerNames.push_back((*s).c_str());
		}

		VkInstanceCreateInfo inst_info = {};//����ʵ��������Ϣ�ṹ��ʵ��
		inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;//�ṹ�������
		inst_info.pNext = NULL;//�Զ������ݵ�ָ��
		inst_info.flags = 0;//������ʹ�õı�־
		inst_info.pApplicationInfo = &app_info;//��Ӧ����Ϣ�ṹ��
		inst_info.enabledExtensionCount = vkInstanceExtensionNames.size();//��չ������
		inst_info.ppEnabledExtensionNames = vkInstanceExtensionNames.data();//��չ�����б�����
		inst_info.enabledLayerCount = exceptedLayerNames.size();//�����Ĳ�����
		inst_info.ppEnabledLayerNames = exceptedLayerNames.data();//�����Ĳ������б�

		VkResult result;//�洢���н���ĸ�������
		result = vkCreateInstance(&inst_info, NULL, &vkInstance);

		VkAssert(result == VK_SUCCESS, "vkCreateInstance");
	}

	void EnumerateVkPhysicalDevices()
	{
		vkGpuCount = 0;//�洢�����豸�����ı���
		VkResult result = vkEnumeratePhysicalDevices(vkInstance, &vkGpuCount, NULL);//��ȡ�����豸����
		VkAssert(result == VK_SUCCESS, "vkEnumeratePhysicalDevices");


		vkGpus.resize(vkGpuCount);//���������豸�б�ߴ�
		result = vkEnumeratePhysicalDevices(vkInstance, &vkGpuCount, vkGpus.data());//��������豸�б�
		VkAssert(result == VK_SUCCESS, "vkEnumeratePhysicalDevices");

		vkGetPhysicalDeviceMemoryProperties(vkGpus[VulkanConfig::GPU::DefaultGpuIndex], &vkMemoryProperties);//��ȡ��һ�����豸���ڴ�����
	}

	void CreateVkDevice()
	{
		vkGetPhysicalDeviceQueueFamilyProperties(vkGpus[0], &vkQueueFamilyCount, NULL);//��ȡ�����豸0�ж��м��������

		vkQueueFamilyprops.resize(vkQueueFamilyCount);//����м��������ı�vector����
		vkGetPhysicalDeviceQueueFamilyProperties(vkGpus[0], &vkQueueFamilyCount, vkQueueFamilyprops.data());//��������豸0���м��������б�


		VkDeviceQueueCreateInfo queueInfo = {};//�����豸���д�����Ϣ�ṹ��ʵ��
		bool found = false;//������־
		for (unsigned int i = 0; i < vkQueueFamilyCount; i++) {//�������ж��м���
			if (vkQueueFamilyprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {//����ǰ���м���֧��ͼ�ι���
				queueInfo.queueFamilyIndex = i;//�󶨴˶��м�������
				vkQueueGraphicsFamilyIndex = i;//��¼֧��ͼ�ι����Ķ��м�������
				found = true;
				break;
			}
		}

		float queue_priorities[1] = { 0.0 };//�����������ȼ�����
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;//�����ṹ������
		queueInfo.pNext = NULL;//�Զ������ݵ�ָ��
		queueInfo.queueCount = 1;//ָ����������
		queueInfo.pQueuePriorities = queue_priorities;//����ÿ�����е����ȼ�
		queueInfo.queueFamilyIndex = vkQueueGraphicsFamilyIndex;//�󶨶��м�������
		vkDeviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);//����������չ

		VkDeviceCreateInfo deviceInfo = {};//�����߼��豸������Ϣ�ṹ��ʵ��
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;//�����ṹ������
		deviceInfo.pNext = NULL;//�Զ������ݵ�ָ��
		deviceInfo.queueCreateInfoCount = 1;//ָ���豸���д�����Ϣ�ṹ������
		deviceInfo.pQueueCreateInfos = &queueInfo;//�����豸���д�����Ϣ�ṹ���б�
		deviceInfo.enabledExtensionCount = vkDeviceExtensionNames.size();//������չ����
		deviceInfo.ppEnabledExtensionNames = vkDeviceExtensionNames.data();//������չ�б�
		deviceInfo.enabledLayerCount = 0;//������Layer������
		deviceInfo.ppEnabledLayerNames = NULL;//������Layer�������б�
		deviceInfo.pEnabledFeatures = NULL;//���õ��豸����
		VkResult result = vkCreateDevice(vkGpus[0], &deviceInfo, NULL, &vkDevice);//�����߼��豸

		VkAssert(result == VK_SUCCESS, "vkCreateDevice");
	}

	void CreateVkCommandBuffer()
	{
		VkCommandPoolCreateInfo cmd_pool_info = {};//��������ش�����Ϣ�ṹ��ʵ��
		cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; 	//�����ṹ������
		cmd_pool_info.pNext = NULL;//�Զ������ݵ�ָ��
		cmd_pool_info.queueFamilyIndex = vkQueueGraphicsFamilyIndex;//��������м�������
		cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	//ִ�п��Ʊ�־
		VkResult result = vkCreateCommandPool(vkDevice, &cmd_pool_info, NULL, &vkCmdPool);//���������
		assert(result == VK_SUCCESS);//�������ش����Ƿ�ɹ�

		VkCommandBufferAllocateInfo cmdBAI = {};//��������������Ϣ�ṹ��ʵ��
		cmdBAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;//�����ṹ������
		cmdBAI.pNext = NULL;//�Զ������ݵ�ָ��
		cmdBAI.commandPool = vkCmdPool;//ָ�������
		cmdBAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//���������弶��
		cmdBAI.commandBufferCount = 1;//��������������
		result = vkAllocateCommandBuffers(vkDevice, &cmdBAI, &vkCmdBuffer);//���������

		assert(result == VK_SUCCESS);//�������Ƿ�ɹ�
		vkCmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;//�����ṹ������
		vkCmdBufferInfo.pNext = NULL;//�Զ������ݵ�ָ��
		vkCmdBufferInfo.flags = 0;//����ʹ�ñ�־
		vkCmdBufferInfo.pInheritanceInfo = NULL;//�����̳���Ϣ
		vkCmdBuffers.push_back(vkCmdBuffer);//Ҫ�ύ������ִ�е����������

		VkPipelineStageFlags* pipe_stage_flags = new VkPipelineStageFlags();//Ŀ����߽׶�
		*pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vkSubmitInfo[0].pNext = NULL;//�Զ������ݵ�ָ��
		vkSubmitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;//�����ṹ������
		vkSubmitInfo[0].pWaitDstStageMask = pipe_stage_flags;//����Ŀ����߽׶�
		vkSubmitInfo[0].commandBufferCount = 1;//���������
		vkSubmitInfo[0].pCommandBuffers = vkCmdBuffers.data();//�ύ�����������
		vkSubmitInfo[0].signalSemaphoreCount = 0;//�ź�������
		vkSubmitInfo[0].pSignalSemaphores = NULL;//�ź�������
	}
	void CreateVkSwapChain()
	{

		//����KHR���洴����Ϣ�ṹ��ʵ��
		VkWin32SurfaceCreateInfoKHR createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;//�����ṹ������
		createInfo.pNext = nullptr;//�Զ������ݵ�ָ��
		createInfo.flags = 0;//��δ��ʹ�õı�־
		createInfo.hwnd = hWnd;//��������
		createInfo.hinstance = hInst;

		//����KHR����
		VkResult result = vkCreateWin32SurfaceKHR(vkInstance, &createInfo, nullptr, &vkSurface);
		assert(result == VK_SUCCESS);

		//�����豸��Ӧ�Ķ��м����б�
		VkBool32* pSupportsPresent = (VkBool32*)malloc(vkQueueFamilyCount * sizeof(VkBool32));
		for (uint32_t i = 0; i < vkQueueFamilyCount; i++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(vkGpus[VulkanConfig::GPU::DefaultGpuIndex], i, vkSurface, &pSupportsPresent[i]);
			//printf("���м�������=%d %s��ʾ\n", i, (pSupportsPresent[i] == 1 ? "֧��" : "��֧��"));
		}

		vkQueueGraphicsFamilyIndex = UINT32_MAX;//֧��ͼ�ι����Ķ��м�������
		vkQueuePresentFamilyIndex = UINT32_MAX;//֧����ʾ(����)�����Ķ��м�������
		for (uint32_t i = 0; i < vkQueueFamilyCount; ++i)//�����豸��Ӧ�Ķ��м����б�
		{
			//�����ǰ�������Ķ��м���֧��Graphis��ͼ�Σ�����
			if ((vkQueueFamilyprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)//���˶��м���֧��ͼ�ι���
			{
				//��¼֧��Graphis��ͼ�Σ������Ķ��м�������
				if (vkQueueGraphicsFamilyIndex == UINT32_MAX) vkQueueGraphicsFamilyIndex = i;
				//�����ǰ�������Ķ��м���֧��Present����ʾ����������
				if (pSupportsPresent[i] == VK_TRUE)//�����ǰ���м���֧����ʾ����
				{
					vkQueueGraphicsFamilyIndex = i;//��¼�˶��м�������Ϊ֧��ͼ�ι�����
					vkQueuePresentFamilyIndex = i;//��¼�˶��м�������Ϊ֧����ʾ������
					//printf("���м�������=%d ͬʱ֧��Graphis��ͼ�Σ���Present����ʾ������\n", i);
					break;
				}
			}
		}

		if (vkQueuePresentFamilyIndex == UINT32_MAX)//��û���ҵ�ͬʱ֧��������Ķ��м���
		{
			for (size_t i = 0; i < vkQueueFamilyCount; ++i)//�����豸��Ӧ�Ķ��м����б�
			{
				if (pSupportsPresent[i] == VK_TRUE)//�ж��Ƿ�֧����ʾ����
				{
					vkQueuePresentFamilyIndex = i;//��¼�˶��м�������Ϊ֧����ʾ������
					break;
				}
			}
		}
		free(pSupportsPresent);//�ͷŴ洢�Ƿ�֧�ֳ��ֹ����Ĳ���ֵ�б�

		//û���ҵ�֧��Graphis��ͼ�Σ���Present����ʾ�������Ķ��м���
		//û���ҵ�֧��ͼ�λ���ʾ�����Ķ��м���
		VkAssert(vkQueueGraphicsFamilyIndex == UINT32_MAX || vkQueuePresentFamilyIndex == UINT32_MAX, "û���ҵ�֧��ͼ�λ���ʾ�����Ķ��м���");

		uint32_t formatCount;//֧�ֵĸ�ʽ����
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkGpus[0], vkSurface, &formatCount, NULL);//��ȡ֧�ֵĸ�ʽ����
		printf("֧�ֵĸ�ʽ����Ϊ %d\n", formatCount);

		VkSurfaceFormatKHR* surfFormats = (VkSurfaceFormatKHR*)malloc(formatCount * sizeof(VkSurfaceFormatKHR));//�����Ӧ�����Ŀռ�
		vkformats.resize(formatCount);//������ӦVector�ߴ�
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkGpus[0], vkSurface, &formatCount, surfFormats);//��ȡ֧�ֵĸ�ʽ��Ϣ
		for (unsigned int i = 0; i < formatCount; i++) {//��¼֧�ֵĸ�ʽ��Ϣ
			vkformats[i] = surfFormats[i].format;
			printf("[%d]֧�ֵĸ�ʽΪ%d\n", i, vkformats[i]);
		}
		if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {//�����������
			vkformats[0] = VK_FORMAT_B8G8R8A8_UNORM;
		}
		free(surfFormats);

		//��ȡKHR���������
		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkGpus[VulkanConfig::GPU::DefaultGpuIndex], vkSurface, &vkSurfaceCapabilities);
		assert(result == VK_SUCCESS);

		//��ȡ֧�ֵ���ʾģʽ����
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkGpus[VulkanConfig::GPU::DefaultGpuIndex], vkSurface, &vkPresentModeCount, NULL);
		assert(result == VK_SUCCESS);
		printf("��ʾģʽ����Ϊ%d\n", vkPresentModeCount);

		//������ӦVector�ߴ�
		vkPresentModes.resize(vkPresentModeCount);
		//��ȡ֧�ֵ���ʾģʽ�б�
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkGpus[VulkanConfig::GPU::DefaultGpuIndex], vkSurface, &vkPresentModeCount, vkPresentModes.data());
		for (unsigned int i = 0; i < vkPresentModeCount; i++)
		{
			printf("��ʾģʽ[%d]���Ϊ%d\n", i, vkPresentModes[i]);
		}

		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;//ȷ����������ʾģʽ
		for (size_t i = 0; i < vkPresentModeCount; i++)//������ʾģʽ�б�
		{
			//���Ҳ֧��VK_PRESENT_MODE_MAILBOX_KHRģʽ��������Ч�ʸߣ���ѡ��
			if (vkPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (vkPresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				//���û������VK_PRESENT_MODE_MAILBOX_KHRģʽ������VK_PRESENT_MODE_IMMEDIATE_KHRģʽ
				//Ҳ��VK_PRESENT_MODE_FIFO_KHRģʽǿ����ѡ��
				swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		//ȷ��surface�Ŀ�ȡ��߶�
		//���surface�����еĳߴ�û�ж��壨���Ϊ0xFFFFFFFF��ʾû���壩
		if (vkSurfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
		{
			vkSwapchainExtent.width = screenWidth;//���ÿ��Ϊ������
			vkSwapchainExtent.height = screenHeight;//���ø߶�Ϊ����߶�
			//�������ֵ���Ƶ����ֵ����Сֵ֮��
			if (vkSwapchainExtent.width < vkSurfaceCapabilities.minImageExtent.width)
			{
				vkSwapchainExtent.width = vkSurfaceCapabilities.minImageExtent.width;
			}
			else if (vkSwapchainExtent.width > vkSurfaceCapabilities.maxImageExtent.width)
			{
				vkSwapchainExtent.width = vkSurfaceCapabilities.maxImageExtent.width;
			}
			//�߶�����ֵ���Ƶ����ֵ����Сֵ֮��
			if (vkSwapchainExtent.height < vkSurfaceCapabilities.minImageExtent.height)
			{
				vkSwapchainExtent.height = vkSurfaceCapabilities.minImageExtent.height;
			}
			else if (vkSwapchainExtent.height > vkSurfaceCapabilities.maxImageExtent.height)
			{
				vkSwapchainExtent.height = vkSurfaceCapabilities.maxImageExtent.height;
			}
			printf("ʹ���Լ����õ� ��� %d �߶� %d\n", vkSwapchainExtent.width, vkSwapchainExtent.height);
		}
		else
		{
			//��������ȷ���ߴ�
			vkSwapchainExtent = vkSurfaceCapabilities.currentExtent;
			printf("ʹ�û�ȡ��surface�����е� ��� %d �߶� %d\n", vkSwapchainExtent.width, vkSwapchainExtent.height);
		}

		screenWidth = vkSwapchainExtent.width;//��¼ʵ�ʲ��õĿ��
		screenHeight = vkSwapchainExtent.height;//��¼ʵ�ʲ��õĸ߶�

		//�����������е�����ͼ������
		uint32_t desiredMinNumberOfSwapChainImages = vkSurfaceCapabilities.minImageCount + 1;
		//���������Ƶ���Χ��
		if ((vkSurfaceCapabilities.maxImageCount > 0) && (desiredMinNumberOfSwapChainImages > vkSurfaceCapabilities.maxImageCount))
		{
			desiredMinNumberOfSwapChainImages = vkSurfaceCapabilities.maxImageCount;
		}

		//KHR����任��־
		VkSurfaceTransformFlagBitsKHR preTransform;
		if (vkSurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)//��֧������ı任
		{
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else//����֧�ֱ任
		{
			preTransform = vkSurfaceCapabilities.currentTransform;
		}

		VkSwapchainCreateInfoKHR swapchain_ci = {};//����������������Ϣ�ṹ��ʵ��
		swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;//�ṹ������
		swapchain_ci.pNext = NULL;//�Զ������ݵ�ָ��
		swapchain_ci.surface = vkSurface;//ָ��KHR����
		swapchain_ci.minImageCount = desiredMinNumberOfSwapChainImages;//����ͼ������
		swapchain_ci.imageFormat = vkformats[0];//ͼ���ʽ
		swapchain_ci.imageExtent.width = vkSwapchainExtent.width;//������ͼ����
		swapchain_ci.imageExtent.height = vkSwapchainExtent.height;//������ͼ��߶�
		swapchain_ci.preTransform = preTransform;//ָ���任��־
		swapchain_ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;//���Alphaֵ
		swapchain_ci.imageArrayLayers = 1;//ͼ���������
		swapchain_ci.presentMode = swapchainPresentMode;//����������ʾģʽ
		swapchain_ci.oldSwapchain = VK_NULL_HANDLE;//ǰ��������
		swapchain_ci.clipped = true;//��������
		swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;//ɫ�ʿռ�
		swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//ͼ����;
		swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;	//ͼ����ģʽ
		swapchain_ci.queueFamilyIndexCount = 0;//���м�������
		swapchain_ci.pQueueFamilyIndices = NULL;//���м��������б�

		if (vkQueueGraphicsFamilyIndex != vkQueuePresentFamilyIndex)//��֧��ͼ�κ���ʾ�����Ķ��м��岻��ͬ
		{
			swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchain_ci.queueFamilyIndexCount = 2;//����������Ķ��м�����������Ϊ2
			uint32_t queueFamilyIndices[2] = { vkQueueGraphicsFamilyIndex,vkQueuePresentFamilyIndex };
			swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;//����������Ķ��м��������б�
		}

		result = vkCreateSwapchainKHR(vkDevice, &swapchain_ci, NULL, &vkSwapChain);//����������
		assert(result == VK_SUCCESS);//��齻�����Ƿ񴴽��ɹ�

		 //��ȡ�������е�ͼ������
		result = vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &vkSwapchainImageCount, NULL);
		assert(result == VK_SUCCESS);
		printf("[SwapChain�е�Image����Ϊ%d]\n", vkSwapchainImageCount);//����Ƿ��ȡ�ɹ�
		vkSwapchainImages.resize(vkSwapchainImageCount);//����ͼ���б�ߴ�
		//��ȡ�������е�ͼ���б�
		result = vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &vkSwapchainImageCount, vkSwapchainImages.data());
		assert(result == VK_SUCCESS);
		vkSwapchainImageViews.resize(vkSwapchainImageCount);//����ͼ����ͼ�б�ߴ�
		for (uint32_t i = 0; i < vkSwapchainImageCount; i++)//Ϊ�������еĸ���ͼ�񴴽�ͼ����ͼ
		{
			VkImageViewCreateInfo color_image_view = {};//����ͼ����ͼ������Ϣ�ṹ��ʵ��
			color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;//���ýṹ������
			color_image_view.pNext = NULL;//�Զ������ݵ�ָ��
			color_image_view.flags = 0;//������ʹ�õı�־
			color_image_view.image = vkSwapchainImages[i];//��Ӧ������ͼ��
			color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;//ͼ����ͼ������
			color_image_view.format = vkformats[0];//ͼ����ͼ��ʽ
			color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;//����Rͨ������
			color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;//����Gͨ������
			color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;//����Bͨ������
			color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;//����Aͨ������
			color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;//ͼ����ͼʹ�÷���
			color_image_view.subresourceRange.baseMipLevel = 0;//����Mipmap����
			color_image_view.subresourceRange.levelCount = 1;//Mipmap���������
			color_image_view.subresourceRange.baseArrayLayer = 0;//���������
			color_image_view.subresourceRange.layerCount = 1;//����������
			result = vkCreateImageView(vkDevice, &color_image_view, NULL, &vkSwapchainImageViews[i]);//����ͼ����ͼ
			assert(result == VK_SUCCESS);//����Ƿ񴴽��ɹ�
		}
	}

	//������Ȼ������
	void CreateVkDepthBuffer()
	{
		vkDepthFormat = VK_FORMAT_D16_UNORM;//ָ�����ͼ��ĸ�ʽ
		VkImageCreateInfo image_info = {};//�������ͼ�񴴽���Ϣ�ṹ��ʵ��
		vkGetPhysicalDeviceFormatProperties(vkGpus[0], vkDepthFormat, &vkDepthFormatProps);//��ȡ�����豸֧�ֵ�ָ����ʽ������
		//ȷ��ƽ�̷�ʽ
		if (vkDepthFormatProps.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)//�Ƿ�֧��������Ƭ��֯��ʽ
		{
			image_info.tiling = VK_IMAGE_TILING_LINEAR;//����������Ƭ��֯��ʽ
			printf("tilingΪVK_IMAGE_TILING_LINEAR!\n");
		}
		else if (vkDepthFormatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)//�Ƿ�֧��������Ƭ��֯��ʽ
		{
			image_info.tiling = VK_IMAGE_TILING_OPTIMAL;//����������Ƭ��֯��ʽ
			printf("tilingΪVK_IMAGE_TILING_OPTIMAL!\n");
		}
		else
		{
			printf("��֧��VK_FORMAT_D16_UNORM!\n");//��ӡ��֧��ָ����ʽ����ʾ��Ϣ
		}
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;	//ָ���ṹ������
		image_info.pNext = NULL;//�Զ������ݵ�ָ��
		image_info.imageType = VK_IMAGE_TYPE_2D;//ͼ������
		image_info.format = vkDepthFormat;//ͼ���ʽ
		image_info.extent.width = screenWidth;//ͼ����
		image_info.extent.height = screenHeight;//ͼ��߶�
		image_info.extent.depth = 1;//ͼ�����
		image_info.mipLevels = 1;//ͼ��mipmap����
		image_info.arrayLayers = 1;//ͼ�����������
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;//����ģʽ
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//��ʼ����
		image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;//ͼ����;
		image_info.queueFamilyIndexCount = 0;//���м�������
		image_info.pQueueFamilyIndices = NULL;//���м��������б�
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//����ģʽ
		image_info.flags = 0;//��־

		VkMemoryAllocateInfo mem_alloc = {};//�����ڴ������Ϣ�ṹ��ʵ��
		mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;//�ṹ������
		mem_alloc.pNext = NULL;//�Զ������ݵ�ָ��
		mem_alloc.allocationSize = 0;//������ڴ��ֽ���
		mem_alloc.memoryTypeIndex = 0;//�ڴ����������

		VkImageViewCreateInfo view_info = {};//�������ͼ����ͼ������Ϣ�ṹ��ʵ��
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;//���ýṹ������
		view_info.pNext = NULL;//�Զ������ݵ�ָ��
		view_info.image = VK_NULL_HANDLE;//��Ӧ��ͼ��
		view_info.format = vkDepthFormat;//ͼ����ͼ�ĸ�ʽ
		view_info.components.r = VK_COMPONENT_SWIZZLE_R;//����Rͨ������
		view_info.components.g = VK_COMPONENT_SWIZZLE_G;//����Gͨ������
		view_info.components.b = VK_COMPONENT_SWIZZLE_B;//����Bͨ������
		view_info.components.a = VK_COMPONENT_SWIZZLE_A;//����Aͨ������
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;//ͼ����ͼʹ�÷���
		view_info.subresourceRange.baseMipLevel = 0;//����Mipmap����
		view_info.subresourceRange.levelCount = 1;//Mipmap���������
		view_info.subresourceRange.baseArrayLayer = 0;//���������
		view_info.subresourceRange.layerCount = 1;//����������
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;//ͼ����ͼ������
		view_info.flags = 0;//��־

		VkResult result = vkCreateImage(vkDevice, &image_info, NULL, &vkDepthImage);//�������ͼ��
		assert(result == VK_SUCCESS);

		VkMemoryRequirements mem_reqs;//��ȡͼ���ڴ�����
		vkGetImageMemoryRequirements(vkDevice, vkDepthImage, &mem_reqs);
		mem_alloc.allocationSize = mem_reqs.size;//��ȡ�����ڴ��ֽ���
		VkFlags requirements_mask = 0;//��Ҫ���ڴ���������
		bool flag = memoryTypeFromProperties(vkMemoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &mem_alloc.memoryTypeIndex);//��ȡ�����ڴ���������
		assert(flag);//����ȡ�Ƿ�ɹ�
		printf("ȷ���ڴ����ͳɹ� ��������Ϊ%d\n", mem_alloc.memoryTypeIndex);
		result = vkAllocateMemory(vkDevice, &mem_alloc, NULL, &vkMemoryDepth);	//�����ڴ�
		VkAssert(result == VK_SUCCESS, "vkAllocateMemory");
		result = vkBindImageMemory(vkDevice, vkDepthImage, vkMemoryDepth, 0);//��ͼ����ڴ�
		VkAssert(result == VK_SUCCESS, "vkBindImageMemory");
		view_info.image = vkDepthImage;//ָ��ͼ����ͼ��Ӧͼ��
		result = vkCreateImageView(vkDevice, &view_info, NULL, &vkDepthImageView);//�������ͼ����ͼ
		VkAssert(result == VK_SUCCESS, "vkCreateImageView");
	}

	//������Ⱦͨ��
	void CreateVkRenderPass()
	{
		VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;		//�����ź���������Ϣ�ṹ��ʵ��
		imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;//�ṹ������
		imageAcquiredSemaphoreCreateInfo.pNext = NULL;//�Զ������ݵ�ָ��
		imageAcquiredSemaphoreCreateInfo.flags = 0;//������ʹ�õı�־

		VkResult result = vkCreateSemaphore(vkDevice, &imageAcquiredSemaphoreCreateInfo, NULL, &vkImageAcquiredSemaphore);//�����ź���
		assert(result == VK_SUCCESS);//����ź����Ƿ񴴽��ɹ�

		VkAttachmentDescription attachments[2];//����������Ϣ����
		attachments[0].format = vkformats[0];//������ɫ�����ĸ�ʽ
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;//���ò���ģʽ
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//����ʱ�Ը����Ĳ���
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;//�洢ʱ�Ը����Ĳ���
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//ģ�����ʱ�Ը����Ĳ���
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//ģ��洢ʱ�Ը����Ĳ���
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//��ʼ�Ĳ���
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//����ʱ�����ղ���
		attachments[0].flags = 0;//����λ����
		attachments[1].format = vkDepthFormat;//������ȸ����ĸ�ʽ
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;//���ò���ģʽ
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//����ʱ�Ը����Ĳ���
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//�洢ʱ�Ը����Ĳ���
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//ģ�����ʱ�Ը����Ĳ���
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//ģ��洢ʱ�Ը����Ĳ���
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; 	//��ʼ�Ĳ���
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//����ʱ�Ĳ���
		attachments[1].flags = 0;//����λ����

		VkAttachmentReference color_reference = {};//��ɫ��������
		color_reference.attachment = 0;//��Ӧ����������Ϣ�����±�
		color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;//���ø�������

		VkAttachmentReference depth_reference = {};//��ȸ�������
		depth_reference.attachment = 1;//��Ӧ����������Ϣ�����±�
		depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//���ø�������

		VkSubpassDescription subpass = {};//������Ⱦ��ͨ�������ṹ��ʵ��
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;//���ù��߰󶨵�
		subpass.flags = 0;//��������
		subpass.inputAttachmentCount = 0;//���븽������
		subpass.pInputAttachments = NULL;//���븽���б�
		subpass.colorAttachmentCount = 1;//��ɫ��������
		subpass.pColorAttachments = &color_reference;//��ɫ�����б�
		subpass.pResolveAttachments = NULL;//Resolve����
		subpass.pDepthStencilAttachment = &depth_reference;//���ģ�帽��
		subpass.preserveAttachmentCount = 0;//preserve��������
		subpass.pPreserveAttachments = NULL;//preserve�����б�

		VkRenderPassCreateInfo rp_info = {};//������Ⱦͨ��������Ϣ�ṹ��ʵ��
		rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;//�ṹ������
		rp_info.pNext = NULL;//�Զ������ݵ�ָ��
		rp_info.attachmentCount = 2;//����������
		rp_info.pAttachments = attachments;//�����б�
		rp_info.subpassCount = 1;//��Ⱦ��ͨ������
		rp_info.pSubpasses = &subpass;//��Ⱦ��ͨ���б�
		rp_info.dependencyCount = 0;//��ͨ����������
		rp_info.pDependencies = NULL;//��ͨ�������б�

		result = vkCreateRenderPass(vkDevice, &rp_info, NULL, &vkRenderPass);//������Ⱦͨ��
		assert(result == VK_SUCCESS);

		vkClearValues[0].color.float32[0] = 0.3f;//֡���������R����ֵ
		vkClearValues[0].color.float32[1] = 0.3f;//֡���������G����ֵ
		vkClearValues[0].color.float32[2] = 0.3f;//֡���������B����ֵ
		vkClearValues[0].color.float32[3] = 0.3f;//֡���������A����ֵ
		vkClearValues[1].depthStencil.depth = 1.0f;//֡������������ֵ
		vkClearValues[1].depthStencil.stencil = 0;//֡���������ģ��ֵ

		vkRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;//��Ⱦͨ��������Ϣ�ṹ������
		vkRenderPassBeginInfo.pNext = NULL;//�Զ������ݵ�ָ��
		vkRenderPassBeginInfo.renderPass = vkRenderPass;//ָ��Ҫ��������Ⱦͨ��
		vkRenderPassBeginInfo.renderArea.offset.x = 0;//��Ⱦ������ʼX����
		vkRenderPassBeginInfo.renderArea.offset.y = 0;//��Ⱦ������ʼY����
		vkRenderPassBeginInfo.renderArea.extent.width = screenWidth;//��Ⱦ������
		vkRenderPassBeginInfo.renderArea.extent.height = screenHeight;//��Ⱦ����߶�
		vkRenderPassBeginInfo.clearValueCount = 2;//֡�������ֵ����
		vkRenderPassBeginInfo.pClearValues = vkClearValues;//֡�������ֵ����
	}

	//��ȡ�豸��֧��ͼ�ι����Ķ���
	void InitQueue()
	{
		//��ȡָ������������Ϊ0�Ķ���
		vkGetDeviceQueue(vkDevice, vkQueueGraphicsFamilyIndex, 0, &vkQueueGraphics);
	}
	//����֡����
	void CreateFrameBuffer()
	{
		VkImageView attachments[2];//����ͼ����ͼ����
		attachments[1] = vkDepthImageView;//�������ͼ����ͼ

		VkFramebufferCreateInfo fb_info = {};//����֡���崴����Ϣ�ṹ��ʵ��
		fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;//�ṹ������
		fb_info.pNext = NULL;//�Զ������ݵ�ָ��
		fb_info.renderPass = vkRenderPass;//ָ����Ⱦͨ��
		fb_info.attachmentCount = 2;//��������
		fb_info.pAttachments = attachments;//����ͼ����ͼ����
		fb_info.width = screenWidth;//���
		fb_info.height = screenHeight;//�߶�
		fb_info.layers = 1;//����

		uint32_t i;//ѭ�����Ʊ���
		vkFrameBuffers = (VkFramebuffer*)malloc(vkSwapchainImageCount * sizeof(VkFramebuffer));//Ϊ֡�������ж�̬�����ڴ�
		VkAssert(vkFrameBuffers != NULL, "AllocateFrameBuffers");//����ڴ�����Ƿ�ɹ�
		//�����������еĸ���ͼ��
		for (i = 0; i < vkSwapchainImageCount; i++)
		{
			attachments[0] = vkSwapchainImageViews[i];//������ɫ������Ӧͼ����ͼ
			VkResult result = vkCreateFramebuffer(vkDevice, &fb_info, NULL, &vkFrameBuffers[i]);//����֡����
			VkAssert(result == VK_SUCCESS, "vkCreateFramebuffer");//����Ƿ񴴽��ɹ�
			printf("[����֡����%d�ɹ���]\n", i);
		}
	}
	void CreatePipeline()
	{
		Pipeline = new TextureDrawPipeline();
		Basic_Pipeline = new BasicDrawPipeline();

		PipelineResource * pipelineResource = new PipelineResource();
		pipelineResource->pipeline = Pipeline->vkPipeline;
		pipelineResource->pipelineLayout = Pipeline->vkPipelineLayout;
		pipelineResource->vkDescriptorSets = Pipeline->vkDescriptorSet;

		vkPipelineMap[RTTI_DRAWABLE_TEXTURE] = pipelineResource;

		pipelineResource = new PipelineResource();
		pipelineResource->pipeline = Pipeline->vkPipeline;
		pipelineResource->pipelineLayout = Pipeline->vkPipelineLayout;
		pipelineResource->vkDescriptorSets = Pipeline->vkDescriptorSet;

		vkPipelineMap[RTTI_DRAWABLE_BASIC] = pipelineResource;

	}
	void CreateFence()
	{
		VkFenceCreateInfo fenceInfo;//դ��������Ϣ�ṹ��ʵ��
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;//�ṹ������
		fenceInfo.pNext = NULL;//�Զ������ݵ�ָ��
		fenceInfo.flags = 0;//������ʹ�õı�־λ
		vkCreateFence(vkDevice, &fenceInfo, NULL, &vkTaskFinishFence);//����ʱդ��
	}
	void InitDrawableResources()
	{
		EasyVulkanTextureManager::Init(vkDevice, vkGpus[0], vkMemoryProperties, vkCmdBuffer, vkQueueGraphics);

		m_Texture = EasyVulkanTextureManager::CreateTexture("123.jpg");
		m_Texture_New = EasyVulkanTextureManager::CreateTexture("123.png");
		m_Texture_vulkan = EasyVulkanTextureManager::CreateTexture("vulkan.png");

		EasyVulkanFontManager::Init("test.ttf", 20, false, false);

		m_Label = new Label("What about tonight?I have prepared for going back to Gold I", QVector3D(-230, 100, 0));
		
	}
	void InitPresentInfo()
	{
		vkPresent.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;	//�ṹ������
		vkPresent.pNext = NULL;//�Զ������ݵ�ָ��
		vkPresent.swapchainCount = 1;//������������
		vkPresent.pSwapchains = &vkSwapChain;//�������б�
		vkPresent.waitSemaphoreCount = 0;//�ȴ����ź�������
		vkPresent.pWaitSemaphores = NULL;//�ȴ����ź����б�
		vkPresent.pResults = NULL;//���ֲ��������־�б�
	}

	void CreateDebugConsole()
	{
		EasyVulkanConsole::vkInstance = vkInstance;
		EasyVulkanConsole::Init();
		EasyVulkanConsole::CreateDebugReportCallbackSelf();
	}
	void DestroyDebugConsole()
	{
		EasyVulkanConsole::vkInstance = vkInstance;
		EasyVulkanConsole::DestroyDebugReportCallbackSelf();
	}
	void FlushUniformBuffer()//����ǰ֡�����������һ�±�������
	{
		/*******ƬԪ��ɫ�׶�********/
		uint8_t* pData;
		float* brightFactor = new float[1]{1.0f};
		VkResult result = vkMapMemory(vkDevice, Pipeline->vkUniformBufferMemory_FS, 0, Pipeline->vkBufferByteCount_FS, 0, (void**)&pData);//���豸�ڴ�ӳ��ΪCPU�ɷ���
		assert(result == VK_SUCCESS);
		memcpy(pData, brightFactor, sizeof(float));//�����վ������ݿ������Դ�
		vkUnmapMemory(vkDevice, Pipeline->vkUniformBufferMemory_FS);	//����ڴ�ӳ��

	}
	void FlushTexToDesSet()//���»������������ķ���
	{
		for (int i = 0; i <2 /*EasyVulkanTextureManager::getTextureCount()*/; i++)
		{
			Pipeline->vkWriteDescriptorSet[0].dstSet = Pipeline->vkDescriptorSet[i];//������������Ӧ��д������
			Pipeline->vkWriteDescriptorSet[0].pBufferInfo = &Pipeline->vkUniformBufferInfo_FS;
			Pipeline->vkWriteDescriptorSet[1].dstSet = Pipeline->vkDescriptorSet[i];//������������Ӧ��д������
			Pipeline->vkWriteDescriptorSet[1].pImageInfo = EasyVulkanTextureManager::getImageInfoByIndex(i);

			vkUpdateDescriptorSets(vkDevice, 2, Pipeline->vkWriteDescriptorSet, 0, NULL);//����������

			//EasyVulkanTextureManager::setImageLayout(vkCmdBuffer, EasyVulkanTextureManager::getTexturesList()[0]->GetVkImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_GENERAL);
		}
		
		m_Label->SetDesriptSetStartIndex(3);
		for (int i = 0; i < m_Label->text.size(); i++)
		{
			Pipeline->vkWriteDescriptorSet[0].dstSet = Pipeline->vkDescriptorSet[3+i];//������������Ӧ��д������
			Pipeline->vkWriteDescriptorSet[0].pBufferInfo = &Pipeline->vkUniformBufferInfo_FS;
			Pipeline->vkWriteDescriptorSet[1].dstSet = Pipeline->vkDescriptorSet[3+i];//������������Ӧ��д������
			Pipeline->vkWriteDescriptorSet[1].pImageInfo = &EasyVulkanFontManager::GetCharInfo(m_Label->text[i].toLatin1())->texture->GetVkImageInfo();

			vkUpdateDescriptorSets(vkDevice, 2, Pipeline->vkWriteDescriptorSet, 0, NULL);//����������
		}
		Pipeline->vkWriteDescriptorSet[0].dstSet = Pipeline->vkDescriptorSet[2];//������������Ӧ��д������
		Pipeline->vkWriteDescriptorSet[0].pBufferInfo = &Pipeline->vkUniformBufferInfo_FS;
		Pipeline->vkWriteDescriptorSet[1].dstSet = Pipeline->vkDescriptorSet[2];//������������Ӧ��д������
		Pipeline->vkWriteDescriptorSet[1].pImageInfo = EasyVulkanTextureManager::getImageInfoByIndex(98);

		vkUpdateDescriptorSets(vkDevice, 2, Pipeline->vkWriteDescriptorSet, 0, NULL);//����������
	}
	void Draw()
	{
		EasyVulkanFpsCalculator::Init();//��ʼ��FPS����
		float  angle = 0.0f;
		while (true)//ÿѭ��һ�λ���һ֡����
		{
			angle = angle + 1.0f;
			SetWindowText(hWnd, QString("FPS = %1").arg(EasyVulkanFpsCalculator::currFPS).toStdWString().c_str());
			m_Label->text= QString("FPS = %1").arg(EasyVulkanFpsCalculator::currFPS);
			EasyVulkanFpsCalculator::Calculate();//����FPS
			EasyVulkanFpsCalculator::BeForeFrame();//һ֡��ʼ

			//��ȡ�������еĵ�ǰ֡����
			VkResult result = vkAcquireNextImageKHR(vkDevice, vkSwapChain, UINT64_MAX, vkImageAcquiredSemaphore, VK_NULL_HANDLE, &vkCurrentBuffer);
			//Ϊ��Ⱦͨ�����õ�ǰ֡����
			vkRenderPassBeginInfo.framebuffer = vkFrameBuffers[vkCurrentBuffer];

			vkResetCommandBuffer(vkCmdBuffer, 0);//�ָ�����嵽��ʼ״̬
			result = vkBeginCommandBuffer(vkCmdBuffer, &vkCmdBufferInfo);//���������

			FlushUniformBuffer();//����ǰ֡�����������һ�±�������
			FlushTexToDesSet();//���»�����������

			vkCmdBeginRenderPass(vkCmdBuffer, &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);//������Ⱦͨ��
			//m_triangle->Draw(Pipeline->vkPipelineLayout, Pipeline->vkPipeline, Pipeline->vkDescriptorSet[0]);

			EasyVulkanTextureManager::setPipeline(Pipeline->vkPipeline);
			EasyVulkanTextureManager::setPipelineLayout(Pipeline->vkPipelineLayout);
		
			EasyVulkanTextureManager::setDescriptorSetPtr(&Pipeline->vkDescriptorSet[0]);
			EasyVulkanMatrixState::pushMatrix();
			EasyVulkanMatrixState::translate(-100,0,100);

			float* mvp = EasyVulkanMatrixState::getFinalMatrix();
			vkCmdPushConstants(vkCmdBuffer, Pipeline->vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, mvp);

			//EasyVulkanTextureManager::setImageLayout(vkCmdBuffer, m_Texture->GetVkImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
			//m_Texture->Draw();
			EasyVulkanMatrixState::popMatrix();

			EasyVulkanMatrixState::pushMatrix();
			EasyVulkanMatrixState::translate(100, 0, 100);
			EasyVulkanTextureManager::setDescriptorSetPtr(&Pipeline->vkDescriptorSet[1]);
			mvp = EasyVulkanMatrixState::getFinalMatrix();
			vkCmdPushConstants(vkCmdBuffer, Pipeline->vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, mvp);
			//EasyVulkanTextureManager::setImageLayout(vkCmdBuffer, m_Texture_New->GetVkImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
			//m_Texture_New->Draw();
			EasyVulkanMatrixState::popMatrix();


			EasyVulkanMatrixState::pushMatrix();
			EasyVulkanMatrixState::translate(0, -55, -5);
			EasyVulkanMatrixState::rotate(angle, 0, 1, 0);
			EasyVulkanMatrixState::scale(7,7,1);
			EasyVulkanTextureManager::setDescriptorSetPtr(&Pipeline->vkDescriptorSet[2]);
			mvp = EasyVulkanMatrixState::getFinalMatrix();
			vkCmdPushConstants(vkCmdBuffer, Pipeline->vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, mvp);
			//EasyVulkanTextureManager::setImageLayout(vkCmdBuffer, m_Texture_New->GetVkImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
			m_Texture_vulkan->Draw();
			EasyVulkanMatrixState::popMatrix();

			m_Label->Draw(Pipeline->vkDescriptorSet.data());

			vkCmdEndRenderPass(vkCmdBuffer);//������Ⱦͨ��

			result = vkEndCommandBuffer(vkCmdBuffer);//���������

			vkSubmitInfo[0].waitSemaphoreCount = 1;//�ȴ����ź�������
			vkSubmitInfo[0].pWaitSemaphores = &vkImageAcquiredSemaphore;//�ȴ����ź����б�

			result = vkQueueSubmit(vkQueueGraphics, 1, vkSubmitInfo, vkTaskFinishFence);//�ύ�����
			do {	//�ȴ���Ⱦ���
				result = vkWaitForFences(vkDevice, 1, &vkTaskFinishFence, VK_TRUE, FENCE_TIMEOUT);
			} while (result == VK_TIMEOUT);
			vkResetFences(vkDevice, 1, &vkTaskFinishFence);//����դ��

			vkPresent.pImageIndices = &vkCurrentBuffer;//ָ���˴γ��ֵĽ�����ͼ������
			result = vkQueuePresentKHR(vkQueueGraphics, &vkPresent);//ִ�г���
			EasyVulkanFpsCalculator::AfterFrame(VulkanConfig::Display::MAX_FPS);//����FPS������ָ����ֵ
		}
	}

	void InitTransformMartix()
	{
		EasyVulkanMatrixState::setCamera(0, 0, 200, 0, 0, 0, 0, 1, 0);//��ʼ�������
		EasyVulkanMatrixState::setInitStack();//��ʼ�������任����
		float ratio = (float)screenWidth / (float)screenHeight;//����Ļ�����
		EasyVulkanMatrixState::setProjectFrustum(-ratio, ratio, -1, 1, 1.5f, 1000);//����ͶӰ����
	}

	void DestroyFence();//����դ��
	void DestroyPipeline();//���ٹ���
	void DestroyDrawableObject();//���ٻ���������
	void DestroyFrameBuffer();//����֡����
	void DestroyRenderPass();//������Ⱦͨ�����
	void DestroyDepthBuffer();//������Ȼ������
	void DestroySwapChain();//���ٽ��������
	void DestroyCommandBuffer();//���������
	void DestroyDevices();//�����߼��豸
	void DestroyInstance();//����Vulkan ʵ��
};

