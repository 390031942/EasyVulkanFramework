#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "EasyVulkanAssert.h"
#include "EasyVulkanMemoryUtility.h"
#include "EasyVulkanAssert.h"
#include "EasyVulkanShaderCompiler.h"
#include "EasyVulkanAssert.h"
using namespace std;

#define NUM_DESCRIPTOR_SETS 1


class EasyVulkanPipeline
{
private:
	VkBuffer vkUniformBuffer_FS;//一致变量缓冲

	vector<VkDescriptorSetLayout> vkDescriptorLayouts;//描述集布局列表
	VkPipelineShaderStageCreateInfo vkShaderStages[2];//着色器阶段数组
	VkVertexInputBindingDescription vkVertexBinding;//管线的顶点输入数据绑定描述
	VkVertexInputAttributeDescription vkVertexAttribs[2];//管线的顶点输入属性描述
	VkPipelineCache vkPipelineCache;//管线缓冲
	VkDevice vkDevice;//逻辑设备指针
	VkRenderPass vkRenderPass;
	VkDescriptorPool vkDescriptorPool;//描述池
	VkPhysicalDeviceMemoryProperties vkMemoryProperties;

	void CreateUniformBuffer();//创建一致变量缓冲
	void DestoryUniformBuffer();//销毁一致变量缓冲
	void CreatePipelineLayout();//创建管线布局
	void DestoryPipelineLayout();	//销毁管线布局
	void InitDescriptorSet();	//初始化描述集
	void CreateShader();//创建着色器
	void DestroyShader();//销毁着色器
	void InitVertexAttributeInfo();//初始化顶点输入属性信息
	void CreatePipeline(float,float);//创建管线
	void DestroyPipeline();//销毁管线

public:
	int vkBufferByteCount_FS;//一致缓冲总字节数
	VkDeviceMemory vkUniformBufferMemory_FS;//一致变量缓冲内存

	VkWriteDescriptorSet vkWriteDescriptorSet[2];//一致变量写入描述集
	vector<VkDescriptorSet> vkDescriptorSet;//描述集列表
	VkPipelineLayout vkPipelineLayout;//管线布局
	VkPipeline vkPipeline;//管线
	VkDescriptorBufferInfo vkUniformBufferInfo_VS;//顶点着色阶段一致变量缓冲描述信息
	VkDescriptorBufferInfo vkUniformBufferInfo_FS;//片元着色阶段一致变量缓冲描述信息

	EasyVulkanPipeline(QSize ScreenSize,VkDevice device, VkRenderPass renderPass, VkPhysicalDeviceMemoryProperties memoryroperties);//构造函数
	~EasyVulkanPipeline();//析构函数
};