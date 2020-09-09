#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "EasyVulkanMemoryUtility.h"

#include "EasyVulkanShaderCompiler.h"
#include "BasicShapes/Drawable.h"

#include "Texture.h"
using namespace std;

class EasyVulkanPipelineCreateInfo
{
private:
	const  static  int                MaxArraySize = 32;
	int                               vertexAttributeDescriptionCount;

	VkVertexInputBindingDescription   vkVertexInputBinding;
	VkVertexInputAttributeDescription vkVertexAttribs[MaxArraySize];

	VkShaderModule                    vkVertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule                    vkFragmentShaderModule = VK_NULL_HANDLE;

	int                               vkDescriptorSetMaxCount;
	int                               vkDescriptorSetLayoutBindingCount;
	VkDescriptorSetLayoutBinding      vkLayoutBinding[MaxArraySize];
	VkDescriptorPoolSize              vkDescriptPoolSize[MaxArraySize];

	int                               pushConstantRangeCount;
	VkPushConstantRange               vkPushConstantRanges[MaxArraySize];

	VkSampleCountFlagBits             vkMultiSampleLevelFlag;

	bool                              enableDepthTest;
	bool                              enableStencilTest;

	VkPolygonMode                     vkPolygonMode; 
	int                               vkLineWidth;

	int                               vsUniformBytesCount;
	int                               fsUniformBytesCount;

	bool                              validInfo = true;
	int                               calledRecordBit = 0;
	std::string                       errorMessage;

	VkDeviceSize                      vertexOffset = 0;
	VkDeviceSize                      pushConstantOffset = 0;

public:
	void                   addVertexInput(VkFormat vertexFormat);

	void                   setVertexShader(std::string path);

	void                   setFragmentShader(std::string path);

	void                   setDescriptorMaxCount(int count);

	void                   addDescriptorLayoutBinding(VkDescriptorType type, VkShaderStageFlags stage);

	void                   addPushConstantRange(int constantCount, VkShaderStageFlags stage);

	void                   setMSAALevel(int level);

	void                   setDepthTestEnable(bool enabled);

	void                   setStencilTestEnable(bool enabled);

	void                   setPolygonMode(VkPolygonMode mode, int lineWidth_if_use_lineMode = 1);

	void                   setUniformBytesCount(int stage_vertex_count, int stage_fragment_count);

	bool                   checkInfoValid();

	int                    getVertexAttributeCount();

	VkVertexInputAttributeDescription* getVertexAttributes();

	VkVertexInputBindingDescription* getVertexBinding();

	VkShaderModule         getVertexShaderModule();

	VkShaderModule         getFragmentShaderModule();
	
	int                    getDescriptorMaxCount();

	int                    getDescriptorSetLayoutBindingCount();

	VkDescriptorSetLayoutBinding* getDescriptorSetLayoutBinding();

	VkDescriptorPoolSize* getDescriptorPoolSizes();

	int                    getPushConstantRangeCount();

	VkPushConstantRange*   getPushConstantRanges();

	VkSampleCountFlagBits     getMultiSampleLevelFlag();

	bool                   isDepthTestEnabled();

	bool                   isStencilTestEnabled();

	VkPolygonMode          getPolygonMode();

	int                    getLineWidth();

	int                    getVertexStageUniformBytesCount();

	int                    getFragmentStageUniformBytesCount();
};

class EasyVulkanPipeline
{
protected:
	VkBuffer                          vkUniformBuffer;    //一致缓冲
	VkDeviceMemory                    vkUniformBufferMemory;    //一致缓冲
	VkDescriptorBufferInfo            vkUniformBufferInfo;

	vector<VkDescriptorSetLayout>     vkDescriptorLayouts;//描述集布局列表
	VkDescriptorPool                  vkDescriptorPool;//描述池
	VkWriteDescriptorSet*             vkWriteDescriptorSet;  //一致变量写入描述集

	EasyVulkanPipelineCreateInfo* createInfo;

	int                               currentDescriptorSetIndex;
	int                               maxDescriptorSetIndex;

	void CreateUniformBuffer();
	void DestoryUniformBuffer();
	void CreatePipelineLayout();
	void DestoryPipelineLayout();
	void InitDescriptorSet();
	void CreateShader();
	void DestroyShader();
	void CreatePipeline();
	void DestroyPipeline();

public:
	vector<VkDescriptorSet> vkDescriptorSet;//描述集列表
	VkPipelineLayout vkPipelineLayout;//管线布局
	VkPipeline vkPipeline;//管线

	bool create(EasyVulkanPipelineCreateInfo* createInfo);

	VkDescriptorSet*  allocateDescriptorSet();

	void resetDescriptorSetIndex();
};
class SpriteDrawPipeline:public EasyVulkanPipeline
{
	SpriteDrawPipeline()
	{
		createInfo = new EasyVulkanPipelineCreateInfo;

		createInfo->addVertexInput(VK_FORMAT_R8G8B8_UNORM);
		createInfo->addVertexInput(VK_FORMAT_R8G8B8A8_UNORM);

		createInfo->setVertexShader("shaders/build_in_sprite.vert");
		createInfo->setFragmentShader("shaders/build_in_sprite.frag");

		createInfo->setUniformBytesCount(0, 1);

		createInfo->setMSAALevel(2);

		createInfo->addPushConstantRange(16, VK_SHADER_STAGE_VERTEX_BIT);
		createInfo->addPushConstantRange(4, VK_SHADER_STAGE_FRAGMENT_BIT);

		createInfo->addDescriptorLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		createInfo->addDescriptorLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

		createInfo->setDescriptorMaxCount(8192);

		createInfo->setDepthTestEnable(true);
		createInfo->setStencilTestEnable(false);

		createInfo->setPolygonMode(VK_POLYGON_MODE_FILL);
	}

	friend class EasyVulkanPipelineManager;
};

class TextDrawPipeline :public EasyVulkanPipeline
{
	TextDrawPipeline()
	{
		createInfo = new EasyVulkanPipelineCreateInfo;

		createInfo->addVertexInput(VK_FORMAT_R8G8_UNORM);
		createInfo->addVertexInput(VK_FORMAT_R8G8B8A8_UNORM);

		createInfo->setVertexShader("shaders/build_in_text.vert");
		createInfo->setFragmentShader("shaders/build_in_text.frag");

		createInfo->setUniformBytesCount(0, 4);

		createInfo->setMSAALevel(4);

		createInfo->addPushConstantRange(16, VK_SHADER_STAGE_VERTEX_BIT);

		createInfo->addDescriptorLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		createInfo->addDescriptorLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

		createInfo->setDescriptorMaxCount(2048);

		createInfo->setDepthTestEnable(true);
		createInfo->setStencilTestEnable(false);

		createInfo->setPolygonMode(VK_POLYGON_MODE_FILL);
	}

	friend class EasyVulkanPipelineManager;
};

class BasicDrawPipeline :public EasyVulkanPipeline
{
	BasicDrawPipeline()
	{
		createInfo = new EasyVulkanPipelineCreateInfo;

		createInfo->addVertexInput(VK_FORMAT_R8G8B8_UNORM);

		createInfo->setVertexShader("shaders/build_in_basic.vert");
		createInfo->setFragmentShader("shaders/build_in_basic.frag");

		createInfo->setUniformBytesCount(0, 4);

		createInfo->setMSAALevel(16);

		createInfo->addPushConstantRange(16, VK_SHADER_STAGE_VERTEX_BIT);

		createInfo->setDescriptorMaxCount(1024);

		createInfo->setDepthTestEnable(true);
		createInfo->setStencilTestEnable(false);

		createInfo->setPolygonMode(VK_POLYGON_MODE_FILL);
	}

	friend class EasyVulkanPipelineManager;
};

class EasyVulkanPipelineManager
{
public:

	static std::map<int, EasyVulkanPipeline*> rttiMap;

	/*Build_In*/
	static SpriteDrawPipeline* spriteDrawPipeline;
	static TextDrawPipeline* textDrawPipeline;
	static BasicDrawPipeline* basicDrawPipeline;

	static void init()
	{
		spriteDrawPipeline = new SpriteDrawPipeline;
		textDrawPipeline = new TextDrawPipeline;
		basicDrawPipeline = new BasicDrawPipeline;

		rttiMap[RTTI_DRAWABLE_SPRITE] = spriteDrawPipeline;
		rttiMap[RTTI_DRAWABLE_TEXT] = textDrawPipeline;
		rttiMap[RTTI_DRAWABLE_BASIC] = basicDrawPipeline;
	}
	static VkPipeline getPipelineByRtti(int rtti)
	{
		auto it = rttiMap.find(rtti);
		if (it != rttiMap.end())
		{
			return ((*it).second)->vkPipeline;
		}
		return NULL;
	}

	static void resetDescriptorAllocate()
	{
		spriteDrawPipeline->resetDescriptorSetIndex();
		textDrawPipeline->resetDescriptorSetIndex();
		basicDrawPipeline->resetDescriptorSetIndex();
	}
	static void allocateDescriptorIndex(Drawable * drawable)
	{
		if (!drawable)
			return;

		int rtti = drawable->getDrawableRTTI();

		if (rtti == RTTI_DRAWABLE_SPRITE)
		{
			Sprite* sprite = static_cast<Sprite*>(drawable);
			sprite->setVkDescriptorSet(spriteDrawPipeline->allocateDescriptorSet());
			sprite->setVkPipeLine(&(spriteDrawPipeline->vkPipeline));
		}
		else if (rtti == RTTI_DRAWABLE_TEXT)
		{
			 
		}
		else if (rtti == RTTI_DRAWABLE_BASIC)
		{

		}
	}

	static void flushDescriptorSet()
	{
		vector<Drawable*> drawables = DrawTaskManager::getDrawable();

		for (int i = 0; i < drawables.size(); i++)
		{
			Drawable* drawable = drawables.at(i);
			int rtti = drawable->getDrawableRTTI();

			if (rtti == RTTI_DRAWABLE_SPRITE)
			{
				spriteDrawPipeline->vkWriteDescriptorSet[0].dstSet = drawable->getVkDescriptorSet();
				spriteDrawPipeline->vkWriteDescriptorSet[1].dstSet = drawable->getVkDescriptorSet();
				spriteDrawPipeline->vkWriteDescriptorSet[1].pImageInfo = &(static_cast<Sprite*>(drawable)->resource.vkImageInfo);

				vkUpdateDescriptorSets(vkDevice, 2, spriteDrawPipeline->vkWriteDescriptorSet, 0, NULL);

			}
			else if (rtti == RTTI_DRAWABLE_TEXT)
			{

			}
			else if (rtti == RTTI_DRAWABLE_BASIC)
			{

			}
		}
	}
};