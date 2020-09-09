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

	static VkQueue vkQueue;
	static QMap<QString, Texture*> m_TextureMap;
	static VkDescriptorSet* vkpDescriptorSet;


public:
	static VkSampler GetStandardSampler();

	static void Init(VkDevice& device, VkPhysicalDevice& gpu, VkPhysicalDeviceMemoryProperties& memoryroperties, VkCommandBuffer& cmdBuffer, VkQueue& queueGraphics);

	static void DestoryAll();
	
	static int GetIndexByName(QString textureName);
	
	static void EusureTexture(Texture* texture);

	static Texture* CreateTexture(QString path);
	static Texture* CreateTexture(uchar *data,int width, int height, QImage::Format format);

	static Texture* getTexturesByName(QString name);
	static vector<Texture*> getTexturesList();
	static int getTextureCount();

private:
	static void InitSample();
};

