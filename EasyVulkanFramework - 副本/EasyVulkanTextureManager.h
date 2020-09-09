#pragma once
#include <QMap>
#include <vector>
#include <QString>

#include <vulkan/vulkan.h>

#include "EasyVulkanAssert.h"
#include "EasyVulkanMemoryUtility.h"
#include "Texture.h"

using namespace std;

#define SAMPLER_COUNT 4

class Texture;

class EasyVulkanTextureManager
{
private:
	static vector<Texture*> textures;
	static vector<VkSampler> vkSamplers;
	static VkDevice vkDevice;
	static VkPhysicalDevice vkGPU;
	static VkPhysicalDeviceMemoryProperties vkMemoryProperties;
	static VkCommandBuffer vkCmdBuffer;
	static VkQueue vkQueue;
	static VkPipeline vkPipeline;
	static VkPipelineLayout vkPipelineLayout;
	static QMap<QString, Texture*> m_TextureMap;
	static VkDescriptorSet* vkpDescriptorSet;

public:
	static void setImageLayout(VkCommandBuffer cmd, VkImage image,
		VkImageAspectFlags aspectMask,
		VkImageLayout old_image_layout,
		VkImageLayout new_image_layout);
	
	static void Init(VkDevice& device, VkPhysicalDevice& gpu, VkPhysicalDeviceMemoryProperties& memoryroperties, VkCommandBuffer& cmdBuffer, VkQueue& queueGraphics);

	static void DestoryAll();
	
	static int GetIndexByName(QString textureName);
	
	static void CreateVertexData(Texture* texture, bool useValidRect=false);
	static void EusureTexture(Texture* texture);

	static Texture* CreateTexture(QString path);
	static Texture* CreateTexture(uchar *data,int width, int height, QImage::Format format);

	static Texture* getTexturesByName(QString name);
	static vector<Texture*> getTexturesList();
	static int getTextureCount();

	static VkDescriptorImageInfo* getImageInfoByIndex(int index);

	static VkCommandBuffer getCmdBuffer();
	static VkPipeline getPipeline();
	static VkPipelineLayout getPipelineLayout();
	static VkDescriptorSet* getDescriptorSetPtr();

	static void setPipeline(VkPipeline pipeline);
	static void setPipelineLayout(VkPipelineLayout pipelineLayout);
	static void setDescriptorSetPtr(VkDescriptorSet* vkpDescriptorset);
private:
	static void InitSample();
};

