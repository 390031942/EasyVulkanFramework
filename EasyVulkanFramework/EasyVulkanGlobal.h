#ifndef EASY_VULKAN_GLOBAL
#define EASY_VULKAN_GLOBAL

/*STL*/
#include <vector>
#include <map>
/*VULKAN*/
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_win32.h>
/*EASY_VULKAN*/
#include "EasyVulkanAssert.h"
#include "EasyVulkanConfig.h"
#include "EasyVulkanConsole.h"
#include "Matrix/EasyVulkanMatrixState.h"
#include "EasyVulkanFpsCalculator.h"
/*WINDOWS*/
#include <Windows.h>
/*Q_CLASS*/
#include <QVector3D>
#include <QQuaternion>
/*NAMESPACE*/
using namespace std;

/*GLOBAL VARIABLES*/
static VkDevice                 vkDevice    = VK_NULL_HANDLE;
static VkPhysicalDevice         vkGPU       = VK_NULL_HANDLE;
static uint32_t                 vkGpuCount  = 0;
static VkCommandBuffer          vkCmdBuffer = VK_NULL_HANDLE;
static vector<VkCommandBuffer>  vkCmdBuffers;
static VkInstance               vkInstance  = VK_NULL_HANDLE;
static vector<VkPhysicalDevice> vkGpus;
static VkRenderPass             vkRenderPass= VK_NULL_HANDLE;
static VkQueue                  vkQueue     = VK_NULL_HANDLE;
static VkPhysicalDeviceMemoryProperties vkMemoryProperties;

#define RTTI int
struct PipelineResource
{
	VkPipeline              pipeline;
	VkPipelineLayout        pipelineLayout;
	vector<VkDescriptorSet> vkDescriptorSets;//√Ë ˆºØ¡–±Ì
};
static map<RTTI, PipelineResource*> vkPipelineMap;

static uint32_t screenWidth  = 0;
static uint32_t screenHeight = 0;

#endif