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
	VkBuffer vkUniformBuffer_FS;//һ�±�������

	vector<VkDescriptorSetLayout> vkDescriptorLayouts;//�����������б�
	VkPipelineShaderStageCreateInfo vkShaderStages[2];//��ɫ���׶�����
	VkVertexInputBindingDescription vkVertexBinding;//���ߵĶ����������ݰ�����
	VkVertexInputAttributeDescription vkVertexAttribs[2];//���ߵĶ���������������
	VkPipelineCache vkPipelineCache;//���߻���
	VkDevice vkDevice;//�߼��豸ָ��
	VkRenderPass vkRenderPass;
	VkDescriptorPool vkDescriptorPool;//������
	VkPhysicalDeviceMemoryProperties vkMemoryProperties;

	void CreateUniformBuffer();//����һ�±�������
	void DestoryUniformBuffer();//����һ�±�������
	void CreatePipelineLayout();//�������߲���
	void DestoryPipelineLayout();	//���ٹ��߲���
	void InitDescriptorSet();	//��ʼ��������
	void CreateShader();//������ɫ��
	void DestroyShader();//������ɫ��
	void InitVertexAttributeInfo();//��ʼ����������������Ϣ
	void CreatePipeline(float,float);//��������
	void DestroyPipeline();//���ٹ���

public:
	int vkBufferByteCount_FS;//һ�»������ֽ���
	VkDeviceMemory vkUniformBufferMemory_FS;//һ�±��������ڴ�

	VkWriteDescriptorSet vkWriteDescriptorSet[2];//һ�±���д��������
	vector<VkDescriptorSet> vkDescriptorSet;//�������б�
	VkPipelineLayout vkPipelineLayout;//���߲���
	VkPipeline vkPipeline;//����
	VkDescriptorBufferInfo vkUniformBufferInfo_VS;//������ɫ�׶�һ�±�������������Ϣ
	VkDescriptorBufferInfo vkUniformBufferInfo_FS;//ƬԪ��ɫ�׶�һ�±�������������Ϣ

	EasyVulkanPipeline(QSize ScreenSize,VkDevice device, VkRenderPass renderPass, VkPhysicalDeviceMemoryProperties memoryroperties);//���캯��
	~EasyVulkanPipeline();//��������
};