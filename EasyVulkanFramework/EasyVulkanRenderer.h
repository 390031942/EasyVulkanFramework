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
		//初始化所需扩展列表，下列两个扩展是Android下做渲染必须要的
		vkInstanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);      //此处移植Windows不需更改
		vkInstanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);//此处移植Windows需要获取VK_KHR_SURFACE_EXTENSION_NAME扩展
		vkInstanceExtensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		VkApplicationInfo app_info = {};//构建应用信息结构体实例
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;//结构体的类型
		app_info.pNext = NULL;//自定义数据的指针
		app_info.pApplicationName = VulkanConfig::AppInfo::AppName;//应用的名称
		app_info.applicationVersion = VulkanConfig::AppInfo::AppVersion;//应用的版本号
		app_info.pEngineName = VulkanConfig::AppInfo::EngineName;//应用的引擎名称
		app_info.engineVersion = VulkanConfig::AppInfo::EngineVersion;//应用的引擎版本号
		app_info.apiVersion = VulkanConfig::AppInfo::ApiVersion;//使用的Vulkan图形应用程序API版本

		std::vector<const char*> exceptedLayerNames;//期望启动的验证Layer名称列表
		exceptedLayerNames.push_back("VK_LAYER_LUNARG_core_validation");
		exceptedLayerNames.push_back("VK_LAYER_LUNARG_parameter_validation");
		exceptedLayerNames.push_back("VK_LAYER_LUNARG_standard_validation");
		VkDeviceLayerAndExtension layerAndExtension = EasyVulkanConsole::GetLayerProperties(exceptedLayerNames);	//获取支持情况


		for (auto s : layerAndExtension.extensionNames) {//将所需的扩展加入扩展名称列表
			vkInstanceExtensionNames.push_back((*s).c_str());
		}
		exceptedLayerNames.clear();	//清空验证Layer名称列表
		for (auto s : layerAndExtension.layerNames) {//将能支持的验证Layer名称加入Layer名称列表
			exceptedLayerNames.push_back((*s).c_str());
		}

		VkInstanceCreateInfo inst_info = {};//构建实例创建信息结构体实例
		inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;//结构体的类型
		inst_info.pNext = NULL;//自定义数据的指针
		inst_info.flags = 0;//供将来使用的标志
		inst_info.pApplicationInfo = &app_info;//绑定应用信息结构体
		inst_info.enabledExtensionCount = vkInstanceExtensionNames.size();//扩展的数量
		inst_info.ppEnabledExtensionNames = vkInstanceExtensionNames.data();//扩展名称列表数据
		inst_info.enabledLayerCount = exceptedLayerNames.size();//启动的层数量
		inst_info.ppEnabledLayerNames = exceptedLayerNames.data();//启动的层名称列表

		VkResult result;//存储运行结果的辅助变量
		result = vkCreateInstance(&inst_info, NULL, &vkInstance);

		VkAssert(result == VK_SUCCESS, "vkCreateInstance");
	}

	void EnumerateVkPhysicalDevices()
	{
		vkGpuCount = 0;//存储物理设备数量的变量
		VkResult result = vkEnumeratePhysicalDevices(vkInstance, &vkGpuCount, NULL);//获取物理设备数量
		VkAssert(result == VK_SUCCESS, "vkEnumeratePhysicalDevices");


		vkGpus.resize(vkGpuCount);//设置物理设备列表尺寸
		result = vkEnumeratePhysicalDevices(vkInstance, &vkGpuCount, vkGpus.data());//填充物理设备列表
		VkAssert(result == VK_SUCCESS, "vkEnumeratePhysicalDevices");

		vkGetPhysicalDeviceMemoryProperties(vkGpus[VulkanConfig::GPU::DefaultGpuIndex], &vkMemoryProperties);//获取第一物理设备的内存属性
	}

	void CreateVkDevice()
	{
		vkGetPhysicalDeviceQueueFamilyProperties(vkGpus[0], &vkQueueFamilyCount, NULL);//获取物理设备0中队列家族的数量

		vkQueueFamilyprops.resize(vkQueueFamilyCount);//随队列家族数量改变vector长度
		vkGetPhysicalDeviceQueueFamilyProperties(vkGpus[0], &vkQueueFamilyCount, vkQueueFamilyprops.data());//填充物理设备0队列家族属性列表


		VkDeviceQueueCreateInfo queueInfo = {};//构建设备队列创建信息结构体实例
		bool found = false;//辅助标志
		for (unsigned int i = 0; i < vkQueueFamilyCount; i++) {//遍历所有队列家族
			if (vkQueueFamilyprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {//若当前队列家族支持图形工作
				queueInfo.queueFamilyIndex = i;//绑定此队列家族索引
				vkQueueGraphicsFamilyIndex = i;//记录支持图形工作的队列家族索引
				found = true;
				break;
			}
		}

		float queue_priorities[1] = { 0.0 };//创建队列优先级数组
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;//给出结构体类型
		queueInfo.pNext = NULL;//自定义数据的指针
		queueInfo.queueCount = 1;//指定队列数量
		queueInfo.pQueuePriorities = queue_priorities;//给出每个队列的优先级
		queueInfo.queueFamilyIndex = vkQueueGraphicsFamilyIndex;//绑定队列家族索引
		vkDeviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);//设置所需扩展

		VkDeviceCreateInfo deviceInfo = {};//构建逻辑设备创建信息结构体实例
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;//给出结构体类型
		deviceInfo.pNext = NULL;//自定义数据的指针
		deviceInfo.queueCreateInfoCount = 1;//指定设备队列创建信息结构体数量
		deviceInfo.pQueueCreateInfos = &queueInfo;//给定设备队列创建信息结构体列表
		deviceInfo.enabledExtensionCount = vkDeviceExtensionNames.size();//所需扩展数量
		deviceInfo.ppEnabledExtensionNames = vkDeviceExtensionNames.data();//所需扩展列表
		deviceInfo.enabledLayerCount = 0;//需启动Layer的数量
		deviceInfo.ppEnabledLayerNames = NULL;//需启动Layer的名称列表
		deviceInfo.pEnabledFeatures = NULL;//启用的设备特性
		VkResult result = vkCreateDevice(vkGpus[0], &deviceInfo, NULL, &vkDevice);//创建逻辑设备

		VkAssert(result == VK_SUCCESS, "vkCreateDevice");
	}

	void CreateVkCommandBuffer()
	{
		VkCommandPoolCreateInfo cmd_pool_info = {};//构建命令池创建信息结构体实例
		cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; 	//给定结构体类型
		cmd_pool_info.pNext = NULL;//自定义数据的指针
		cmd_pool_info.queueFamilyIndex = vkQueueGraphicsFamilyIndex;//绑定所需队列家族索引
		cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	//执行控制标志
		VkResult result = vkCreateCommandPool(vkDevice, &cmd_pool_info, NULL, &vkCmdPool);//创建命令池
		assert(result == VK_SUCCESS);//检查命令池创建是否成功

		VkCommandBufferAllocateInfo cmdBAI = {};//构建命令缓冲分配信息结构体实例
		cmdBAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;//给定结构体类型
		cmdBAI.pNext = NULL;//自定义数据的指针
		cmdBAI.commandPool = vkCmdPool;//指定命令池
		cmdBAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//分配的命令缓冲级别
		cmdBAI.commandBufferCount = 1;//分配的命令缓冲数量
		result = vkAllocateCommandBuffers(vkDevice, &cmdBAI, &vkCmdBuffer);//分配命令缓冲

		assert(result == VK_SUCCESS);//检查分配是否成功
		vkCmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;//给定结构体类型
		vkCmdBufferInfo.pNext = NULL;//自定义数据的指针
		vkCmdBufferInfo.flags = 0;//描述使用标志
		vkCmdBufferInfo.pInheritanceInfo = NULL;//命令缓冲继承信息
		vkCmdBuffers.push_back(vkCmdBuffer);//要提交到队列执行的命令缓冲数组

		VkPipelineStageFlags* pipe_stage_flags = new VkPipelineStageFlags();//目标管线阶段
		*pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vkSubmitInfo[0].pNext = NULL;//自定义数据的指针
		vkSubmitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;//给定结构体类型
		vkSubmitInfo[0].pWaitDstStageMask = pipe_stage_flags;//给定目标管线阶段
		vkSubmitInfo[0].commandBufferCount = 1;//命令缓冲数量
		vkSubmitInfo[0].pCommandBuffers = vkCmdBuffers.data();//提交的命令缓冲数组
		vkSubmitInfo[0].signalSemaphoreCount = 0;//信号量数量
		vkSubmitInfo[0].pSignalSemaphores = NULL;//信号量数组
	}
	void CreateVkSwapChain()
	{

		//构建KHR表面创建信息结构体实例
		VkWin32SurfaceCreateInfoKHR createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;//给定结构体类型
		createInfo.pNext = nullptr;//自定义数据的指针
		createInfo.flags = 0;//供未来使用的标志
		createInfo.hwnd = hWnd;//给定窗体
		createInfo.hinstance = hInst;

		//创建KHR表面
		VkResult result = vkCreateWin32SurfaceKHR(vkInstance, &createInfo, nullptr, &vkSurface);
		assert(result == VK_SUCCESS);

		//遍历设备对应的队列家族列表
		VkBool32* pSupportsPresent = (VkBool32*)malloc(vkQueueFamilyCount * sizeof(VkBool32));
		for (uint32_t i = 0; i < vkQueueFamilyCount; i++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(vkGpus[VulkanConfig::GPU::DefaultGpuIndex], i, vkSurface, &pSupportsPresent[i]);
			//printf("队列家族索引=%d %s显示\n", i, (pSupportsPresent[i] == 1 ? "支持" : "不支持"));
		}

		vkQueueGraphicsFamilyIndex = UINT32_MAX;//支持图形工作的队列家族索引
		vkQueuePresentFamilyIndex = UINT32_MAX;//支持显示(呈现)工作的队列家族索引
		for (uint32_t i = 0; i < vkQueueFamilyCount; ++i)//遍历设备对应的队列家族列表
		{
			//如果当前遍历到的队列家族支持Graphis（图形）工作
			if ((vkQueueFamilyprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)//若此队列家族支持图形工作
			{
				//记录支持Graphis（图形）工作的队列家族索引
				if (vkQueueGraphicsFamilyIndex == UINT32_MAX) vkQueueGraphicsFamilyIndex = i;
				//如果当前遍历到的队列家族支持Present（显示工作）工作
				if (pSupportsPresent[i] == VK_TRUE)//如果当前队列家族支持显示工作
				{
					vkQueueGraphicsFamilyIndex = i;//记录此队列家族索引为支持图形工作的
					vkQueuePresentFamilyIndex = i;//记录此队列家族索引为支持显示工作的
					//printf("队列家族索引=%d 同时支持Graphis（图形）和Present（显示）工作\n", i);
					break;
				}
			}
		}

		if (vkQueuePresentFamilyIndex == UINT32_MAX)//若没有找到同时支持两项工作的队列家族
		{
			for (size_t i = 0; i < vkQueueFamilyCount; ++i)//遍历设备对应的队列家族列表
			{
				if (pSupportsPresent[i] == VK_TRUE)//判断是否支持显示工作
				{
					vkQueuePresentFamilyIndex = i;//记录此队列家族索引为支持显示工作的
					break;
				}
			}
		}
		free(pSupportsPresent);//释放存储是否支持呈现工作的布尔值列表

		//没有找到支持Graphis（图形）或Present（显示）工作的队列家族
		//没有找到支持图形或显示工作的队列家族
		VkAssert(vkQueueGraphicsFamilyIndex == UINT32_MAX || vkQueuePresentFamilyIndex == UINT32_MAX, "没有找到支持图形或显示工作的队列家族");

		uint32_t formatCount;//支持的格式数量
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkGpus[0], vkSurface, &formatCount, NULL);//获取支持的格式数量
		printf("支持的格式数量为 %d\n", formatCount);

		VkSurfaceFormatKHR* surfFormats = (VkSurfaceFormatKHR*)malloc(formatCount * sizeof(VkSurfaceFormatKHR));//分配对应数量的空间
		vkformats.resize(formatCount);//调整对应Vector尺寸
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(vkGpus[0], vkSurface, &formatCount, surfFormats);//获取支持的格式信息
		for (unsigned int i = 0; i < formatCount; i++) {//记录支持的格式信息
			vkformats[i] = surfFormats[i].format;
			printf("[%d]支持的格式为%d\n", i, vkformats[i]);
		}
		if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {//特殊情况处理
			vkformats[0] = VK_FORMAT_B8G8R8A8_UNORM;
		}
		free(surfFormats);

		//获取KHR表面的能力
		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkGpus[VulkanConfig::GPU::DefaultGpuIndex], vkSurface, &vkSurfaceCapabilities);
		assert(result == VK_SUCCESS);

		//获取支持的显示模式数量
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkGpus[VulkanConfig::GPU::DefaultGpuIndex], vkSurface, &vkPresentModeCount, NULL);
		assert(result == VK_SUCCESS);
		printf("显示模式数量为%d\n", vkPresentModeCount);

		//调整对应Vector尺寸
		vkPresentModes.resize(vkPresentModeCount);
		//获取支持的显示模式列表
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(vkGpus[VulkanConfig::GPU::DefaultGpuIndex], vkSurface, &vkPresentModeCount, vkPresentModes.data());
		for (unsigned int i = 0; i < vkPresentModeCount; i++)
		{
			printf("显示模式[%d]编号为%d\n", i, vkPresentModes[i]);
		}

		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;//确定交换链显示模式
		for (size_t i = 0; i < vkPresentModeCount; i++)//遍历显示模式列表
		{
			//如果也支持VK_PRESENT_MODE_MAILBOX_KHR模式，由于其效率高，便选用
			if (vkPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (vkPresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				//如果没能用上VK_PRESENT_MODE_MAILBOX_KHR模式，但有VK_PRESENT_MODE_IMMEDIATE_KHR模式
				//也比VK_PRESENT_MODE_FIFO_KHR模式强，故选用
				swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		//确定surface的宽度、高度
		//如果surface能力中的尺寸没有定义（宽度为0xFFFFFFFF表示没定义）
		if (vkSurfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
		{
			vkSwapchainExtent.width = screenWidth;//设置宽度为窗体宽度
			vkSwapchainExtent.height = screenHeight;//设置高度为窗体高度
			//宽度设置值限制到最大值与最小值之间
			if (vkSwapchainExtent.width < vkSurfaceCapabilities.minImageExtent.width)
			{
				vkSwapchainExtent.width = vkSurfaceCapabilities.minImageExtent.width;
			}
			else if (vkSwapchainExtent.width > vkSurfaceCapabilities.maxImageExtent.width)
			{
				vkSwapchainExtent.width = vkSurfaceCapabilities.maxImageExtent.width;
			}
			//高度设置值限制到最大值与最小值之间
			if (vkSwapchainExtent.height < vkSurfaceCapabilities.minImageExtent.height)
			{
				vkSwapchainExtent.height = vkSurfaceCapabilities.minImageExtent.height;
			}
			else if (vkSwapchainExtent.height > vkSurfaceCapabilities.maxImageExtent.height)
			{
				vkSwapchainExtent.height = vkSurfaceCapabilities.maxImageExtent.height;
			}
			printf("使用自己设置的 宽度 %d 高度 %d\n", vkSwapchainExtent.width, vkSwapchainExtent.height);
		}
		else
		{
			//若表面有确定尺寸
			vkSwapchainExtent = vkSurfaceCapabilities.currentExtent;
			printf("使用获取的surface能力中的 宽度 %d 高度 %d\n", vkSwapchainExtent.width, vkSwapchainExtent.height);
		}

		screenWidth = vkSwapchainExtent.width;//记录实际采用的宽度
		screenHeight = vkSwapchainExtent.height;//记录实际采用的高度

		//期望交换链中的最少图像数量
		uint32_t desiredMinNumberOfSwapChainImages = vkSurfaceCapabilities.minImageCount + 1;
		//将数量限制到范围内
		if ((vkSurfaceCapabilities.maxImageCount > 0) && (desiredMinNumberOfSwapChainImages > vkSurfaceCapabilities.maxImageCount))
		{
			desiredMinNumberOfSwapChainImages = vkSurfaceCapabilities.maxImageCount;
		}

		//KHR表面变换标志
		VkSurfaceTransformFlagBitsKHR preTransform;
		if (vkSurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)//若支持所需的变换
		{
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else//若不支持变换
		{
			preTransform = vkSurfaceCapabilities.currentTransform;
		}

		VkSwapchainCreateInfoKHR swapchain_ci = {};//构建交换链创建信息结构体实例
		swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;//结构体类型
		swapchain_ci.pNext = NULL;//自定义数据的指针
		swapchain_ci.surface = vkSurface;//指定KHR表面
		swapchain_ci.minImageCount = desiredMinNumberOfSwapChainImages;//最少图像数量
		swapchain_ci.imageFormat = vkformats[0];//图像格式
		swapchain_ci.imageExtent.width = vkSwapchainExtent.width;//交换链图像宽度
		swapchain_ci.imageExtent.height = vkSwapchainExtent.height;//交换链图像高度
		swapchain_ci.preTransform = preTransform;//指定变换标志
		swapchain_ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;//混合Alpha值
		swapchain_ci.imageArrayLayers = 1;//图像数组层数
		swapchain_ci.presentMode = swapchainPresentMode;//交换链的显示模式
		swapchain_ci.oldSwapchain = VK_NULL_HANDLE;//前导交换链
		swapchain_ci.clipped = true;//开启剪裁
		swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;//色彩空间
		swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//图像用途
		swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;	//图像共享模式
		swapchain_ci.queueFamilyIndexCount = 0;//队列家族数量
		swapchain_ci.pQueueFamilyIndices = NULL;//队列家族索引列表

		if (vkQueueGraphicsFamilyIndex != vkQueuePresentFamilyIndex)//若支持图形和显示工作的队列家族不相同
		{
			swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchain_ci.queueFamilyIndexCount = 2;//交换链所需的队列家族索引数量为2
			uint32_t queueFamilyIndices[2] = { vkQueueGraphicsFamilyIndex,vkQueuePresentFamilyIndex };
			swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;//交换链所需的队列家族索引列表
		}

		result = vkCreateSwapchainKHR(vkDevice, &swapchain_ci, NULL, &vkSwapChain);//创建交换链
		assert(result == VK_SUCCESS);//检查交换链是否创建成功

		 //获取交换链中的图像数量
		result = vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &vkSwapchainImageCount, NULL);
		assert(result == VK_SUCCESS);
		printf("[SwapChain中的Image数量为%d]\n", vkSwapchainImageCount);//检查是否获取成功
		vkSwapchainImages.resize(vkSwapchainImageCount);//调整图像列表尺寸
		//获取交换链中的图像列表
		result = vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &vkSwapchainImageCount, vkSwapchainImages.data());
		assert(result == VK_SUCCESS);
		vkSwapchainImageViews.resize(vkSwapchainImageCount);//调整图像视图列表尺寸
		for (uint32_t i = 0; i < vkSwapchainImageCount; i++)//为交换链中的各幅图像创建图像视图
		{
			VkImageViewCreateInfo color_image_view = {};//构建图像视图创建信息结构体实例
			color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;//设置结构体类型
			color_image_view.pNext = NULL;//自定义数据的指针
			color_image_view.flags = 0;//供将来使用的标志
			color_image_view.image = vkSwapchainImages[i];//对应交换链图像
			color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;//图像视图的类型
			color_image_view.format = vkformats[0];//图像视图格式
			color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;//设置R通道调和
			color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;//设置G通道调和
			color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;//设置B通道调和
			color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;//设置A通道调和
			color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;//图像视图使用方面
			color_image_view.subresourceRange.baseMipLevel = 0;//基础Mipmap级别
			color_image_view.subresourceRange.levelCount = 1;//Mipmap级别的数量
			color_image_view.subresourceRange.baseArrayLayer = 0;//基础数组层
			color_image_view.subresourceRange.layerCount = 1;//数组层的数量
			result = vkCreateImageView(vkDevice, &color_image_view, NULL, &vkSwapchainImageViews[i]);//创建图像视图
			assert(result == VK_SUCCESS);//检查是否创建成功
		}
	}

	//创建深度缓冲相关
	void CreateVkDepthBuffer()
	{
		vkDepthFormat = VK_FORMAT_D16_UNORM;//指定深度图像的格式
		VkImageCreateInfo image_info = {};//构建深度图像创建信息结构体实例
		vkGetPhysicalDeviceFormatProperties(vkGpus[0], vkDepthFormat, &vkDepthFormatProps);//获取物理设备支持的指定格式的属性
		//确定平铺方式
		if (vkDepthFormatProps.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)//是否支持线性瓦片组织方式
		{
			image_info.tiling = VK_IMAGE_TILING_LINEAR;//采用线性瓦片组织方式
			printf("tiling为VK_IMAGE_TILING_LINEAR!\n");
		}
		else if (vkDepthFormatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)//是否支持最优瓦片组织方式
		{
			image_info.tiling = VK_IMAGE_TILING_OPTIMAL;//采用最优瓦片组织方式
			printf("tiling为VK_IMAGE_TILING_OPTIMAL!\n");
		}
		else
		{
			printf("不支持VK_FORMAT_D16_UNORM!\n");//打印不支持指定格式的提示信息
		}
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;	//指定结构体类型
		image_info.pNext = NULL;//自定义数据的指针
		image_info.imageType = VK_IMAGE_TYPE_2D;//图像类型
		image_info.format = vkDepthFormat;//图像格式
		image_info.extent.width = screenWidth;//图像宽度
		image_info.extent.height = screenHeight;//图像高度
		image_info.extent.depth = 1;//图像深度
		image_info.mipLevels = 1;//图像mipmap级数
		image_info.arrayLayers = 1;//图像数组层数量
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;//采样模式
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//初始布局
		image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;//图像用途
		image_info.queueFamilyIndexCount = 0;//队列家族数量
		image_info.pQueueFamilyIndices = NULL;//队列家族索引列表
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//共享模式
		image_info.flags = 0;//标志

		VkMemoryAllocateInfo mem_alloc = {};//构建内存分配信息结构体实例
		mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;//结构体类型
		mem_alloc.pNext = NULL;//自定义数据的指针
		mem_alloc.allocationSize = 0;//分配的内存字节数
		mem_alloc.memoryTypeIndex = 0;//内存的类型索引

		VkImageViewCreateInfo view_info = {};//构建深度图像视图创建信息结构体实例
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;//设置结构体类型
		view_info.pNext = NULL;//自定义数据的指针
		view_info.image = VK_NULL_HANDLE;//对应的图像
		view_info.format = vkDepthFormat;//图像视图的格式
		view_info.components.r = VK_COMPONENT_SWIZZLE_R;//设置R通道调和
		view_info.components.g = VK_COMPONENT_SWIZZLE_G;//设置G通道调和
		view_info.components.b = VK_COMPONENT_SWIZZLE_B;//设置B通道调和
		view_info.components.a = VK_COMPONENT_SWIZZLE_A;//设置A通道调和
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;//图像视图使用方面
		view_info.subresourceRange.baseMipLevel = 0;//基础Mipmap级别
		view_info.subresourceRange.levelCount = 1;//Mipmap级别的数量
		view_info.subresourceRange.baseArrayLayer = 0;//基础数组层
		view_info.subresourceRange.layerCount = 1;//数组层的数量
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;//图像视图的类型
		view_info.flags = 0;//标志

		VkResult result = vkCreateImage(vkDevice, &image_info, NULL, &vkDepthImage);//创建深度图像
		assert(result == VK_SUCCESS);

		VkMemoryRequirements mem_reqs;//获取图像内存需求
		vkGetImageMemoryRequirements(vkDevice, vkDepthImage, &mem_reqs);
		mem_alloc.allocationSize = mem_reqs.size;//获取所需内存字节数
		VkFlags requirements_mask = 0;//需要的内存类型掩码
		bool flag = memoryTypeFromProperties(vkMemoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &mem_alloc.memoryTypeIndex);//获取所需内存类型索引
		assert(flag);//检查获取是否成功
		printf("确定内存类型成功 类型索引为%d\n", mem_alloc.memoryTypeIndex);
		result = vkAllocateMemory(vkDevice, &mem_alloc, NULL, &vkMemoryDepth);	//分配内存
		VkAssert(result == VK_SUCCESS, "vkAllocateMemory");
		result = vkBindImageMemory(vkDevice, vkDepthImage, vkMemoryDepth, 0);//绑定图像和内存
		VkAssert(result == VK_SUCCESS, "vkBindImageMemory");
		view_info.image = vkDepthImage;//指定图像视图对应图像
		result = vkCreateImageView(vkDevice, &view_info, NULL, &vkDepthImageView);//创建深度图像视图
		VkAssert(result == VK_SUCCESS, "vkCreateImageView");
	}

	//创建渲染通道
	void CreateVkRenderPass()
	{
		VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;		//构建信号量创建信息结构体实例
		imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;//结构体类型
		imageAcquiredSemaphoreCreateInfo.pNext = NULL;//自定义数据的指针
		imageAcquiredSemaphoreCreateInfo.flags = 0;//供将来使用的标志

		VkResult result = vkCreateSemaphore(vkDevice, &imageAcquiredSemaphoreCreateInfo, NULL, &vkImageAcquiredSemaphore);//创建信号量
		assert(result == VK_SUCCESS);//检测信号量是否创建成功

		VkAttachmentDescription attachments[2];//附件描述信息数组
		attachments[0].format = vkformats[0];//设置颜色附件的格式
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;//设置采样模式
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//加载时对附件的操作
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;//存储时对附件的操作
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//模板加载时对附件的操作
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//模板存储时对附件的操作
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//初始的布局
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//结束时的最终布局
		attachments[0].flags = 0;//设置位掩码
		attachments[1].format = vkDepthFormat;//设置深度附件的格式
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;//设置采样模式
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//加载时对附件的操作
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//存储时对附件的操作
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//模板加载时对附件的操作
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//模板存储时对附件的操作
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; 	//初始的布局
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//结束时的布局
		attachments[1].flags = 0;//设置位掩码

		VkAttachmentReference color_reference = {};//颜色附件引用
		color_reference.attachment = 0;//对应附件描述信息数组下标
		color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;//设置附件布局

		VkAttachmentReference depth_reference = {};//深度附件引用
		depth_reference.attachment = 1;//对应附件描述信息数组下标
		depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//设置附件布局

		VkSubpassDescription subpass = {};//构建渲染子通道描述结构体实例
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;//设置管线绑定点
		subpass.flags = 0;//设置掩码
		subpass.inputAttachmentCount = 0;//输入附件数量
		subpass.pInputAttachments = NULL;//输入附件列表
		subpass.colorAttachmentCount = 1;//颜色附件数量
		subpass.pColorAttachments = &color_reference;//颜色附件列表
		subpass.pResolveAttachments = NULL;//Resolve附件
		subpass.pDepthStencilAttachment = &depth_reference;//深度模板附件
		subpass.preserveAttachmentCount = 0;//preserve附件数量
		subpass.pPreserveAttachments = NULL;//preserve附件列表

		VkRenderPassCreateInfo rp_info = {};//构建渲染通道创建信息结构体实例
		rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;//结构体类型
		rp_info.pNext = NULL;//自定义数据的指针
		rp_info.attachmentCount = 2;//附件的数量
		rp_info.pAttachments = attachments;//附件列表
		rp_info.subpassCount = 1;//渲染子通道数量
		rp_info.pSubpasses = &subpass;//渲染子通道列表
		rp_info.dependencyCount = 0;//子通道依赖数量
		rp_info.pDependencies = NULL;//子通道依赖列表

		result = vkCreateRenderPass(vkDevice, &rp_info, NULL, &vkRenderPass);//创建渲染通道
		assert(result == VK_SUCCESS);

		vkClearValues[0].color.float32[0] = 0.3f;//帧缓冲清除用R分量值
		vkClearValues[0].color.float32[1] = 0.3f;//帧缓冲清除用G分量值
		vkClearValues[0].color.float32[2] = 0.3f;//帧缓冲清除用B分量值
		vkClearValues[0].color.float32[3] = 0.3f;//帧缓冲清除用A分量值
		vkClearValues[1].depthStencil.depth = 1.0f;//帧缓冲清除用深度值
		vkClearValues[1].depthStencil.stencil = 0;//帧缓冲清除用模板值

		vkRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;//渲染通道启动信息结构体类型
		vkRenderPassBeginInfo.pNext = NULL;//自定义数据的指针
		vkRenderPassBeginInfo.renderPass = vkRenderPass;//指定要启动的渲染通道
		vkRenderPassBeginInfo.renderArea.offset.x = 0;//渲染区域起始X坐标
		vkRenderPassBeginInfo.renderArea.offset.y = 0;//渲染区域起始Y坐标
		vkRenderPassBeginInfo.renderArea.extent.width = screenWidth;//渲染区域宽度
		vkRenderPassBeginInfo.renderArea.extent.height = screenHeight;//渲染区域高度
		vkRenderPassBeginInfo.clearValueCount = 2;//帧缓冲清除值数量
		vkRenderPassBeginInfo.pClearValues = vkClearValues;//帧缓冲清除值数组
	}

	//获取设备中支持图形工作的队列
	void InitQueue()
	{
		//获取指定家族中索引为0的队列
		vkGetDeviceQueue(vkDevice, vkQueueGraphicsFamilyIndex, 0, &vkQueueGraphics);
	}
	//创建帧缓冲
	void CreateFrameBuffer()
	{
		VkImageView attachments[2];//附件图像视图数组
		attachments[1] = vkDepthImageView;//给定深度图像视图

		VkFramebufferCreateInfo fb_info = {};//构建帧缓冲创建信息结构体实例
		fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;//结构体类型
		fb_info.pNext = NULL;//自定义数据的指针
		fb_info.renderPass = vkRenderPass;//指定渲染通道
		fb_info.attachmentCount = 2;//附件数量
		fb_info.pAttachments = attachments;//附件图像视图数组
		fb_info.width = screenWidth;//宽度
		fb_info.height = screenHeight;//高度
		fb_info.layers = 1;//层数

		uint32_t i;//循环控制变量
		vkFrameBuffers = (VkFramebuffer*)malloc(vkSwapchainImageCount * sizeof(VkFramebuffer));//为帧缓冲序列动态分配内存
		VkAssert(vkFrameBuffers != NULL, "AllocateFrameBuffers");//检查内存分配是否成功
		//遍历交换链中的各个图像
		for (i = 0; i < vkSwapchainImageCount; i++)
		{
			attachments[0] = vkSwapchainImageViews[i];//给定颜色附件对应图像视图
			VkResult result = vkCreateFramebuffer(vkDevice, &fb_info, NULL, &vkFrameBuffers[i]);//创建帧缓冲
			VkAssert(result == VK_SUCCESS, "vkCreateFramebuffer");//检查是否创建成功
			printf("[创建帧缓冲%d成功！]\n", i);
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
		VkFenceCreateInfo fenceInfo;//栅栏创建信息结构体实例
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;//结构体类型
		fenceInfo.pNext = NULL;//自定义数据的指针
		fenceInfo.flags = 0;//供将来使用的标志位
		vkCreateFence(vkDevice, &fenceInfo, NULL, &vkTaskFinishFence);//创建时栅栏
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
		vkPresent.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;	//结构体类型
		vkPresent.pNext = NULL;//自定义数据的指针
		vkPresent.swapchainCount = 1;//交换链的数量
		vkPresent.pSwapchains = &vkSwapChain;//交换链列表
		vkPresent.waitSemaphoreCount = 0;//等待的信号量数量
		vkPresent.pWaitSemaphores = NULL;//等待的信号量列表
		vkPresent.pResults = NULL;//呈现操作结果标志列表
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
	void FlushUniformBuffer()//将当前帧相关数据送入一致变量缓冲
	{
		/*******片元着色阶段********/
		uint8_t* pData;
		float* brightFactor = new float[1]{1.0f};
		VkResult result = vkMapMemory(vkDevice, Pipeline->vkUniformBufferMemory_FS, 0, Pipeline->vkBufferByteCount_FS, 0, (void**)&pData);//将设备内存映射为CPU可访问
		assert(result == VK_SUCCESS);
		memcpy(pData, brightFactor, sizeof(float));//将最终矩阵数据拷贝进显存
		vkUnmapMemory(vkDevice, Pipeline->vkUniformBufferMemory_FS);	//解除内存映射

	}
	void FlushTexToDesSet()//更新绘制用描述集的方法
	{
		for (int i = 0; i <2 /*EasyVulkanTextureManager::getTextureCount()*/; i++)
		{
			Pipeline->vkWriteDescriptorSet[0].dstSet = Pipeline->vkDescriptorSet[i];//更新描述集对应的写入属性
			Pipeline->vkWriteDescriptorSet[0].pBufferInfo = &Pipeline->vkUniformBufferInfo_FS;
			Pipeline->vkWriteDescriptorSet[1].dstSet = Pipeline->vkDescriptorSet[i];//更新描述集对应的写入属性
			Pipeline->vkWriteDescriptorSet[1].pImageInfo = EasyVulkanTextureManager::getImageInfoByIndex(i);

			vkUpdateDescriptorSets(vkDevice, 2, Pipeline->vkWriteDescriptorSet, 0, NULL);//更新描述集

			//EasyVulkanTextureManager::setImageLayout(vkCmdBuffer, EasyVulkanTextureManager::getTexturesList()[0]->GetVkImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_GENERAL);
		}
		
		m_Label->SetDesriptSetStartIndex(3);
		for (int i = 0; i < m_Label->text.size(); i++)
		{
			Pipeline->vkWriteDescriptorSet[0].dstSet = Pipeline->vkDescriptorSet[3+i];//更新描述集对应的写入属性
			Pipeline->vkWriteDescriptorSet[0].pBufferInfo = &Pipeline->vkUniformBufferInfo_FS;
			Pipeline->vkWriteDescriptorSet[1].dstSet = Pipeline->vkDescriptorSet[3+i];//更新描述集对应的写入属性
			Pipeline->vkWriteDescriptorSet[1].pImageInfo = &EasyVulkanFontManager::GetCharInfo(m_Label->text[i].toLatin1())->texture->GetVkImageInfo();

			vkUpdateDescriptorSets(vkDevice, 2, Pipeline->vkWriteDescriptorSet, 0, NULL);//更新描述集
		}
		Pipeline->vkWriteDescriptorSet[0].dstSet = Pipeline->vkDescriptorSet[2];//更新描述集对应的写入属性
		Pipeline->vkWriteDescriptorSet[0].pBufferInfo = &Pipeline->vkUniformBufferInfo_FS;
		Pipeline->vkWriteDescriptorSet[1].dstSet = Pipeline->vkDescriptorSet[2];//更新描述集对应的写入属性
		Pipeline->vkWriteDescriptorSet[1].pImageInfo = EasyVulkanTextureManager::getImageInfoByIndex(98);

		vkUpdateDescriptorSets(vkDevice, 2, Pipeline->vkWriteDescriptorSet, 0, NULL);//更新描述集
	}
	void Draw()
	{
		EasyVulkanFpsCalculator::Init();//初始化FPS计算
		float  angle = 0.0f;
		while (true)//每循环一次绘制一帧画面
		{
			angle = angle + 1.0f;
			SetWindowText(hWnd, QString("FPS = %1").arg(EasyVulkanFpsCalculator::currFPS).toStdWString().c_str());
			m_Label->text= QString("FPS = %1").arg(EasyVulkanFpsCalculator::currFPS);
			EasyVulkanFpsCalculator::Calculate();//计算FPS
			EasyVulkanFpsCalculator::BeForeFrame();//一帧开始

			//获取交换链中的当前帧索引
			VkResult result = vkAcquireNextImageKHR(vkDevice, vkSwapChain, UINT64_MAX, vkImageAcquiredSemaphore, VK_NULL_HANDLE, &vkCurrentBuffer);
			//为渲染通道设置当前帧缓冲
			vkRenderPassBeginInfo.framebuffer = vkFrameBuffers[vkCurrentBuffer];

			vkResetCommandBuffer(vkCmdBuffer, 0);//恢复命令缓冲到初始状态
			result = vkBeginCommandBuffer(vkCmdBuffer, &vkCmdBufferInfo);//启动命令缓冲

			FlushUniformBuffer();//将当前帧相关数据送入一致变量缓冲
			FlushTexToDesSet();//更新绘制用描述集

			vkCmdBeginRenderPass(vkCmdBuffer, &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);//启动渲染通道
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

			vkCmdEndRenderPass(vkCmdBuffer);//结束渲染通道

			result = vkEndCommandBuffer(vkCmdBuffer);//结束命令缓冲

			vkSubmitInfo[0].waitSemaphoreCount = 1;//等待的信号量数量
			vkSubmitInfo[0].pWaitSemaphores = &vkImageAcquiredSemaphore;//等待的信号量列表

			result = vkQueueSubmit(vkQueueGraphics, 1, vkSubmitInfo, vkTaskFinishFence);//提交命令缓冲
			do {	//等待渲染完毕
				result = vkWaitForFences(vkDevice, 1, &vkTaskFinishFence, VK_TRUE, FENCE_TIMEOUT);
			} while (result == VK_TIMEOUT);
			vkResetFences(vkDevice, 1, &vkTaskFinishFence);//重置栅栏

			vkPresent.pImageIndices = &vkCurrentBuffer;//指定此次呈现的交换链图像索引
			result = vkQueuePresentKHR(vkQueueGraphics, &vkPresent);//执行呈现
			EasyVulkanFpsCalculator::AfterFrame(VulkanConfig::Display::MAX_FPS);//限制FPS不超过指定的值
		}
	}

	void InitTransformMartix()
	{
		EasyVulkanMatrixState::setCamera(0, 0, 200, 0, 0, 0, 0, 1, 0);//初始化摄像机
		EasyVulkanMatrixState::setInitStack();//初始化基本变换矩阵
		float ratio = (float)screenWidth / (float)screenHeight;//求屏幕长宽比
		EasyVulkanMatrixState::setProjectFrustum(-ratio, ratio, -1, 1, 1.5f, 1000);//设置投影参数
	}

	void DestroyFence();//销毁栅栏
	void DestroyPipeline();//销毁管线
	void DestroyDrawableObject();//销毁绘制用物体
	void DestroyFrameBuffer();//销毁帧缓冲
	void DestroyRenderPass();//销毁渲染通道相关
	void DestroyDepthBuffer();//销毁深度缓冲相关
	void DestroySwapChain();//销毁交换链相关
	void DestroyCommandBuffer();//销毁命令缓冲
	void DestroyDevices();//销毁逻辑设备
	void DestroyInstance();//销毁Vulkan 实例
};

