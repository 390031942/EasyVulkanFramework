#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "EasyVulkanMemoryUtility.h"

#include "EasyVulkanShaderCompiler.h"
using namespace std;

#define NUM_DESCRIPTOR_SETS 1

/*class TextureDrawPipeline
{
private:
	VkBuffer vkUniformBuffer_FS;//一致变量缓冲

	vector<VkDescriptorSetLayout> vkDescriptorLayouts;//描述集布局列表
	VkPipelineShaderStageCreateInfo vkShaderStages[2];//着色器阶段数组
	VkVertexInputBindingDescription vkVertexBinding;//管线的顶点输入数据绑定描述
	VkVertexInputAttributeDescription vkVertexAttribs[2];//管线的顶点输入属性描述
	VkPipelineCache vkPipelineCache;//管线缓冲
	VkDevice vkDevice;//逻辑设备指针
	VkDescriptorPool vkDescriptorPool;//描述池

	void CreateUniformBuffer();//创建一致变量缓冲
	void DestoryUniformBuffer();//销毁一致变量缓冲
	void CreatePipelineLayout();//创建管线布局
	void DestoryPipelineLayout();	//销毁管线布局
	void InitDescriptorSet();	//初始化描述集
	void CreateShader();//创建着色器
	void DestroyShader();//销毁着色器
	void InitVertexAttributeInfo();//初始化顶点输入属性信息
	void CreatePipeline();//创建管线
	void DestroyPipeline();//销毁管线

public:
	int vkBufferByteCount_FS;//一致缓冲总字节数
	VkDeviceMemory vkUniformBufferMemory_FS;//一致变量缓冲内存

	VkWriteDescriptorSet vkWriteDescriptorSet[2];//一致变量写入描述集
	vector<VkDescriptorSet> vkDescriptorSet;//描述集列表
	VkPipelineLayout vkPipelineLayout;//管线布局h
	VkPipeline vkPipeline;//管线
	VkDescriptorBufferInfo vkUniformBufferInfo_VS;//顶点着色阶段一致变量缓冲描述信息
	VkDescriptorBufferInfo vkUniformBufferInfo_FS;//片元着色阶段一致变量缓冲描述信息

	TextureDrawPipeline();//构造函数
	~TextureDrawPipeline();//析构函数
};

class BasicDrawPipeline
{
private:
	vector<VkDescriptorSetLayout> vkDescriptorLayouts;//描述集布局列表
	VkPipelineShaderStageCreateInfo vkShaderStages[2];//着色器阶段数组
	VkVertexInputBindingDescription vkVertexBinding;//管线的顶点输入数据绑定描述
	VkVertexInputAttributeDescription vkVertexAttribs[2];//管线的顶点输入属性描述
	VkPipelineCache vkPipelineCache;//管线缓冲
	VkDescriptorPool vkDescriptorPool;//描述池

	void CreatePipelineLayout();//创建管线布局
	void DestoryPipelineLayout();	//销毁管线布局
	void InitDescriptorSet();	//初始化描述集
	void CreateShader();//创建着色器
	void DestroyShader();//销毁着色器
	void InitVertexAttributeInfo();//初始化顶点输入属性信息
	void CreatePipeline();//创建管线
	void DestroyPipeline();//销毁管线

public:

	VkWriteDescriptorSet vkWriteDescriptorSet[1];//一致变量写入描述集
	vector<VkDescriptorSet> vkDescriptorSet;//描述集列表
	VkPipelineLayout vkPipelineLayout;//管线布局
	VkPipeline vkPipeline;//管线

	BasicDrawPipeline();//构造函数
	~BasicDrawPipeline();//析构函数
}*/ 


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
	void                   addVertexInput(VkFormat vertexFormat)
	{
		if (vertexAttributeDescriptionCount == MaxArraySize)
		{
			validInfo = false;
			errorMessage = "vertexAttributeDescriptionCount Can not be more than MaxArraySize 32";
			return;
		}


		vkVertexAttribs[vertexAttributeDescriptionCount].binding = 0;
		vkVertexAttribs[vertexAttributeDescriptionCount].format = vertexFormat;
		vkVertexAttribs[vertexAttributeDescriptionCount].location = vertexAttributeDescriptionCount;
		vkVertexAttribs[vertexAttributeDescriptionCount].offset = vertexOffset;

		vertexAttributeDescriptionCount++;

		calledRecordBit = calledRecordBit | 0x01;

		switch (vkVertexAttribs[vertexAttributeDescriptionCount - 1].format)
		{
		case VK_FORMAT_R8_UNORM: vertexOffset += 1 * sizeof(float); break;
		case VK_FORMAT_R8G8_UNORM:vertexOffset += 2 * sizeof(float); break;
		case VK_FORMAT_R8G8B8_UNORM:vertexOffset += 3 * sizeof(float); break;
		case VK_FORMAT_R8G8B8A8_UNORM:vertexOffset += 4 * sizeof(float); break;
		default:
			//类别无效
			validInfo = false;
			errorMessage = "VK_FORMAT Error";
			return;
		}

		vkVertexInputBinding.binding = 0;//对应绑定点
		vkVertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;	//数据输入频率为每顶点
		vkVertexInputBinding.stride = vertexOffset;//每组数据的跨度字节数
	}

	void                   setVertexShader(std::string path)
	{
		QFile file(path.c_str());

		if (!file.exists())
		{
			validInfo = false;
			errorMessage = "SetVertexShade Failed : can not find file" + path;
			return;
		}

		vkVertexShaderModule  = EasyVulkanShaderCompiler::CompilerVertexShader(path.data());

		calledRecordBit = calledRecordBit | 0x02;
	}

	void                   setFragmentShader(std::string path)
	{
		QFile file(path.c_str());

		if (!file.exists())
		{
			validInfo = false;
			errorMessage = "SetVertexShade Failed : can not find file" + path;
			return;
		}

		vkFragmentShaderModule = EasyVulkanShaderCompiler::CompilerFragmentShader(path.data());

		calledRecordBit = calledRecordBit | 0x04;
	}

	void                   setDescriptorMaxCount(int count)
	{
		vkDescriptorSetMaxCount = count;
		calledRecordBit = calledRecordBit | 0x08;
	}

	void                   addDescriptorLayoutBinding(VkDescriptorType type, VkShaderStageFlags stage)
	{
		if (vkDescriptorSetLayoutBindingCount == MaxArraySize)
		{
			validInfo = false;
			errorMessage = "vkDescriptorSetLayoutBindingCount Can not be more than MaxArraySize 32";
			return;
		}
		vkLayoutBinding[vkDescriptorSetLayoutBindingCount].binding = vkDescriptorSetLayoutBindingCount;
		vkLayoutBinding[vkDescriptorSetLayoutBindingCount].descriptorCount = vkDescriptorSetMaxCount;
		vkLayoutBinding[vkDescriptorSetLayoutBindingCount].descriptorType = type;
		vkLayoutBinding[vkDescriptorSetLayoutBindingCount].pImmutableSamplers = NULL;
		vkLayoutBinding[vkDescriptorSetLayoutBindingCount].stageFlags = stage;


		vkDescriptPoolSize[vkDescriptorSetLayoutBindingCount].descriptorCount = vkDescriptorSetMaxCount;
		vkDescriptPoolSize[vkDescriptorSetLayoutBindingCount].type = type;

		vkDescriptorSetLayoutBindingCount++;

		calledRecordBit = calledRecordBit | 0x10;

	}

	void                   addPushConstantRange(int constantCount, VkShaderStageFlags stage)
	{
		if (pushConstantRangeCount == MaxArraySize)
		{
			validInfo = false;
			errorMessage = "pushConstantRangeCount Can not be more than MaxArraySize 32";
			return;
		}

		vkPushConstantRanges[pushConstantRangeCount].stageFlags = stage;
		vkPushConstantRanges[pushConstantRangeCount].size = constantCount * sizeof(float);
		vkPushConstantRanges[pushConstantRangeCount].offset = pushConstantOffset;

		pushConstantOffset += constantCount * sizeof(float);

		calledRecordBit = calledRecordBit | 0x20;
	}

	void                   setMSAALevel(int level)
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(vkGPU, &physicalDeviceProperties);

		VkSampleCountFlags counts = min(physicalDeviceProperties.limits.framebufferColorSampleCounts,
			physicalDeviceProperties.limits.framebufferDepthSampleCounts);

		VkSampleCountFlagBits result;
		if (counts & VK_SAMPLE_COUNT_64_BIT & level == 64) {
			result = VK_SAMPLE_COUNT_64_BIT;
		}

		if (counts & VK_SAMPLE_COUNT_32_BIT & level == 32) {
			result = VK_SAMPLE_COUNT_32_BIT;
		}

		if (counts & VK_SAMPLE_COUNT_16_BIT & level == 16) {
			result = VK_SAMPLE_COUNT_16_BIT;
		}

		if (counts & VK_SAMPLE_COUNT_8_BIT & level == 8) {
			result = VK_SAMPLE_COUNT_8_BIT;
		}

		if (counts & VK_SAMPLE_COUNT_4_BIT & level == 4) {
			result = VK_SAMPLE_COUNT_4_BIT;
		}

		if (counts & VK_SAMPLE_COUNT_2_BIT & level == 2) {
			result = VK_SAMPLE_COUNT_2_BIT;
		}
		else {
			result = VK_SAMPLE_COUNT_1_BIT;
		}

		vkMultiSampleLevelFlag = result;

		calledRecordBit = calledRecordBit | 0x40;
	}

	void                   setDepthTestEnable(bool enabled)
	{
		this->enableDepthTest = true;

		calledRecordBit = calledRecordBit | 0x80;
	}

	void                   setStencilTestEnable(bool enabled)
	{
		this->enableStencilTest = true;

		calledRecordBit = calledRecordBit | 0x100;
	}

	void                   setPolygonMode(VkPolygonMode mode, int lineWidth_if_use_lineMode = 1)
	{
		this->vkPolygonMode = mode;
		if (mode == VK_POLYGON_MODE_LINE)
		{
			vkLineWidth = max(1, lineWidth_if_use_lineMode);
		}

		calledRecordBit = calledRecordBit | 0x200;
	}

	void                   setUniformBytesCount(int stage_vertex_count, int stage_fragment_count)
	{
		this->vsUniformBytesCount = stage_vertex_count;
		this->fsUniformBytesCount = stage_fragment_count;
	}

	bool                   checkInfoValid()
	{
		return validInfo && (calledRecordBit == 0x3ff);
	}

	int                    getVertexAttributeCount()
	{
		return vertexAttributeDescriptionCount;
	}

	VkVertexInputAttributeDescription* getVertexAttributes()
	{
		return vkVertexAttribs;
	}

	VkVertexInputBindingDescription* getVertexBinding()
	{
		return &vkVertexInputBinding;
	}

	VkShaderModule         getVertexShaderModule()
	{
		return vkVertexShaderModule;
	}

	VkShaderModule         getFragmentShaderModule()
	{
		return vkFragmentShaderModule;
	}
	
	int                    getDescriptorMaxCount()
	{
		return vkDescriptorSetMaxCount;
	}

	int                    getDescriptorSetLayoutBindingCount()
	{
		return vkDescriptorSetLayoutBindingCount;
	}

	VkDescriptorSetLayoutBinding* getDescriptorSetLayoutBinding()
	{
		return vkLayoutBinding;
	}

	VkDescriptorPoolSize*   getDescriptorPoolSizes()
	{
		return vkDescriptPoolSize;
	}

	int                    getPushConstantRangeCount()
	{
		return pushConstantRangeCount;
	}

	VkPushConstantRange*    getPushConstantRanges()
	{
		return vkPushConstantRanges;
	}

	VkSampleCountFlagBits     getMultiSampleLevelFlag()
	{
		return vkMultiSampleLevelFlag;
	}

	bool                   isDepthTestEnabled()
	{
		return enableDepthTest;
	}

	bool                   isStencilTestEnabled()
	{
		return enableStencilTest;
	}

	VkPolygonMode          getPolygonMode()
	{
		return vkPolygonMode;
	}

	int                    getLineWidth()
	{
		return  vkLineWidth;
	}

	int                    getVertexStageUniformBytesCount()
	{
		return this->vsUniformBytesCount;
	}

	int                    getFragmentStageUniformBytesCount()
	{
		return this->fsUniformBytesCount;
	}

};

class EasyVulkanPipeline
{
private:
	VkBuffer                          vkUniformBuffer;    //一致缓冲
	VkDeviceMemory                    vkUniformBufferMemory;    //一致缓冲
	VkDescriptorBufferInfo            vkUniformBufferInfo;
	vector<VkDescriptorSetLayout>     vkDescriptorLayouts;//描述集布局列表
	VkPipelineShaderStageCreateInfo   vkShaderStages[2];  //着色器阶段数组
	VkVertexInputBindingDescription   vkVertexBinding;    //管线的顶点输入数据绑定描述
	VkVertexInputAttributeDescription vkVertexAttribs[2];//管线的顶点输入属性描述
	VkPipelineCache                   vkPipelineCache;//管线缓冲
	VkDescriptorPool                  vkDescriptorPool;//描述池

	EasyVulkanPipelineCreateInfo*     createInfo;

	VkWriteDescriptorSet*             vkWriteDescriptorSet;//一致变量写入描述集

	void CreateUniformBuffer()//创建一致变量缓冲
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(vkGPU, &physicalDeviceProperties);

		const VkPhysicalDeviceLimits* pdevLimits = &(physicalDeviceProperties.limits);
		const VkDeviceSize uniAlign = pdevLimits->minUniformBufferOffsetAlignment;

		int bindingCount = createInfo->getDescriptorSetLayoutBindingCount();

		int vsUniformBufferByteCount = createInfo->getVertexStageUniformBytesCount();
		int fsUniformBufferByteCount = createInfo->getFragmentStageUniformBytesCount();

		const VkDeviceSize vsUniformBufferAllocSize = aligned(vsUniformBufferByteCount, uniAlign);
		const VkDeviceSize fsUniformBufferAllocSize = aligned(fsUniformBufferByteCount, uniAlign);

		const VkDeviceSize allocateSize = vsUniformBufferAllocSize + fsUniformBufferAllocSize;//一致变量缓冲的总字节数

		VkBufferCreateInfo buf_info = {};//构建一致变量缓冲创建信息结构体实例
		buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;	//结构体的类型
		buf_info.pNext = NULL;//自定义数据的指针
		buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;//缓冲的用途
		buf_info.size = allocateSize;//缓冲总字节数
		buf_info.queueFamilyIndexCount = 0;	//队列家族数量
		buf_info.pQueueFamilyIndices = NULL;//队列家族索引列表
		buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//共享模式
		buf_info.flags = 0;//标志

		VkResult result = vkCreateBuffer(vkDevice, &buf_info, NULL, &vkUniformBuffer);//创建一致变量缓冲
		VkAssert(result == VK_SUCCESS, "vkCreateBuffer");//检查创建是否成功

		VkMemoryRequirements mem_reqs;
		vkGetBufferMemoryRequirements(vkDevice, vkUniformBuffer, &mem_reqs);//获取此缓冲的内存需求

		VkMemoryAllocateInfo alloc_info = {};//构建内存分配信息结构体实例
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;//结构体类型
		alloc_info.pNext = NULL;//自定义数据的指针
		alloc_info.memoryTypeIndex = 0;//内存类型索引
		alloc_info.allocationSize = mem_reqs.size;//缓冲内存分配字节数

		VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;//需要的内存类型掩码
		bool flag = memoryTypeFromProperties(vkMemoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);	//获取所需内存类型索引
		if (flag) { printf("确定内存类型成功 类型索引为%d", alloc_info.memoryTypeIndex); }
		else { printf("确定内存类型失败!"); }

		result = vkAllocateMemory(vkDevice, &alloc_info, NULL, &vkUniformBufferMemory);//分配内存
		VkAssert(result == VK_SUCCESS, "vkAllocateMemory");//检查内存分配是否成功
		result = vkBindBufferMemory(vkDevice, vkUniformBuffer, vkUniformBufferMemory, 0);//将内存和对应缓冲绑定
		VkAssert(result == VK_SUCCESS, "vkBindBufferMemory");//检查绑定操作是否成功

		vkUniformBufferInfo.buffer = vkUniformBuffer;//指定一致变量缓冲
		vkUniformBufferInfo.offset = 0;//起始偏移量
		vkUniformBufferInfo.range = allocateSize;//一致变量缓冲总字节数
	}
	void DestoryUniformBuffer()//销毁一致变量缓冲
	{
		if (vkUniformBuffer) {
			vkDestroyBuffer(vkDevice, vkUniformBuffer,NULL);
			vkUniformBuffer = VK_NULL_HANDLE;
		}

		if (vkUniformBufferMemory) {
			vkFreeMemory(vkDevice, vkUniformBufferMemory, NULL);
			vkUniformBufferMemory = VK_NULL_HANDLE;
		}
	}
	void CreatePipelineLayout()//创建管线布局
	{

		VkDescriptorSetLayoutCreateInfo descriptor_layout = {};	//构建描述集布局创建信息结构体实例
		descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;//结构体类型
		descriptor_layout.pNext = NULL;//自定义数据的指针
		descriptor_layout.bindingCount = createInfo->getDescriptorSetLayoutBindingCount();;//描述集布局绑定的数量
		descriptor_layout.pBindings = createInfo->getDescriptorSetLayoutBinding();;//描述集布局绑定数组

		//调整空间
		vkDescriptorLayouts.resize(createInfo->getDescriptorMaxCount());//调整描述集布局列表尺寸
		VkResult result = vkCreateDescriptorSetLayout(vkDevice, &descriptor_layout, NULL, vkDescriptorLayouts.data());//创建描述集布局
		VkAssert(result == VK_SUCCESS, "vkCreateDescriptorSetLayout");//检查描述集布局创建是否成功

		VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};//构建管线布局创建信息结构体实例
		pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;	//结构体类型
		pPipelineLayoutCreateInfo.pNext = NULL;//自定义数据的指针
		pPipelineLayoutCreateInfo.pushConstantRangeCount = createInfo->getPushConstantRangeCount();//推送常量范围的数量
		pPipelineLayoutCreateInfo.pPushConstantRanges = createInfo->getPushConstantRanges();//推送常量范围的列表
		pPipelineLayoutCreateInfo.setLayoutCount = createInfo->getDescriptorMaxCount();//描述集布局的数量
		pPipelineLayoutCreateInfo.pSetLayouts = vkDescriptorLayouts.data();//描述集布局列表

		result = vkCreatePipelineLayout(vkDevice, &pPipelineLayoutCreateInfo, NULL, &vkPipelineLayout);//创建管线布局
		VkAssert(result == VK_SUCCESS, "vkCreatePipelineLayout");//检查创建是否成功
	}
	void DestoryPipelineLayout()	//销毁管线布局
	{
		for (int i = 0; i < createInfo->getDescriptorSetLayoutBindingCount(); i++)//遍历描述集列表
		{
			vkDestroyDescriptorSetLayout(vkDevice, vkDescriptorLayouts[i], NULL);//销毁对应描述集布局
		}
		vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, NULL);//销毁管线布局
	}
	void InitDescriptorSet()	//初始化描述集
	{
		VkDescriptorPoolCreateInfo descriptor_pool = {};//构建描述集池创建信息结构体实例
		descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;//结构体类型
		descriptor_pool.pNext = NULL;//自定义数据的指针
		descriptor_pool.maxSets = createInfo->getDescriptorMaxCount();//描述集最大数量
		descriptor_pool.poolSizeCount = createInfo->getDescriptorSetLayoutBindingCount();//描述集池尺寸实例数量
		descriptor_pool.pPoolSizes = createInfo->getDescriptorPoolSizes();//描述集池尺寸实例数组

		VkResult result = vkCreateDescriptorPool(vkDevice, &descriptor_pool, NULL, &vkDescriptorPool);//创建描述集池
		VkAssert(result == VK_SUCCESS, "vkCreateDescriptorPool");//检查描述集池创建是否成功

		std::vector<VkDescriptorSetLayout> layouts;//描述集布局列表
		for (int i = 0; i < createInfo->getDescriptorMaxCount(); i++)
		{
			layouts.push_back(vkDescriptorLayouts[0]);//向列表中添加指定描述集布局
		}

		VkDescriptorSetAllocateInfo alloc_info[1];//构建描述集分配信息结构体实例数组
		alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;//结构体类型
		alloc_info[0].pNext = NULL;//自定义数据的指针
		alloc_info[0].descriptorPool = vkDescriptorPool;//指定描述集池
		alloc_info[0].descriptorSetCount = createInfo->getDescriptorMaxCount();//描述集数量
		alloc_info[0].pSetLayouts = layouts.data();//描述集布局列表

		vkDescriptorSet.resize(createInfo->getDescriptorMaxCount());//调整描述集列表尺寸
		result = vkAllocateDescriptorSets(vkDevice, alloc_info, vkDescriptorSet.data());//分配描述集
		VkAssert(result == VK_SUCCESS, "vkAllocateDescriptorSets");//检查描述集分配是否成功

		vkWriteDescriptorSet = new VkWriteDescriptorSet[createInfo->getDescriptorMaxCount()];

		for (int i = 0; i < createInfo->getDescriptorMaxCount(); i++)
		{
			vkWriteDescriptorSet[i] = {}; //完善一致变量写入描述集实例数组
			vkWriteDescriptorSet[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;//结构体类型
			vkWriteDescriptorSet[i].pNext = NULL;	//自定义数据的指针
			vkWriteDescriptorSet[i].descriptorCount = 1;//描述数量
			vkWriteDescriptorSet[i].descriptorType = createInfo->getDescriptorSetLayoutBinding()[i].descriptorType;//描述类型
			vkWriteDescriptorSet[i].dstArrayElement = 0;//目标数组起始元素
			vkWriteDescriptorSet[i].dstBinding = i;//目标绑定编号

			if (vkWriteDescriptorSet[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			{
				vkWriteDescriptorSet[i].pBufferInfo = &vkUniformBufferInfo;
			}
		}
	}
	void CreateShader()//创建着色器
	{
		vkShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vkShaderStages[0].pNext = NULL;//自定义数据的指针
		vkShaderStages[0].pSpecializationInfo = NULL;//特殊信息
		vkShaderStages[0].flags = 0;//供将来使用的标志
		vkShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;	//着色器阶段为顶点
		vkShaderStages[0].pName = "main";//入口函数为main
		vkShaderStages[0].module = createInfo->getVertexShaderModule();

		vkShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;//结构体类型
		vkShaderStages[1].pNext = NULL;//自定义数据的指针
		vkShaderStages[1].pSpecializationInfo = NULL;//特殊信息
		vkShaderStages[1].flags = 0;//供将来使用的标志
		vkShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;//着色器阶段为片元
		vkShaderStages[1].pName = "main";//入口函数为main
		vkShaderStages[1].module = createInfo->getFragmentShaderModule();
	}
	void DestroyShader()
	{
		vkDestroyShaderModule(vkDevice, vkShaderStages[0].module, NULL);//销毁顶点着色器模块
		vkDestroyShaderModule(vkDevice, vkShaderStages[1].module, NULL);//销毁片元着色器模块
	}
	void CreatePipeline()
	{
		VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];//动态状态启用标志
		memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);	//设置所有标志为false

		VkPipelineDynamicStateCreateInfo dynamicState = {};//管线动态状态创建信息
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;//结构体类型
		dynamicState.pNext = NULL;//自定义数据的指针
		dynamicState.pDynamicStates = dynamicStateEnables;//动态状态启用标志数组
		dynamicState.dynamicStateCount = 0;//启用的动态状态项数量

		VkPipelineVertexInputStateCreateInfo vi;//管线顶点数据输入状态创建信息
		vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vi.pNext = NULL;//自定义数据的指针
		vi.flags = 0;//供将来使用的标志
		vi.vertexBindingDescriptionCount = 1;//顶点输入绑定描述数量
		vi.pVertexBindingDescriptions = createInfo->getVertexBinding();//顶点输入绑定描述列表
		vi.vertexAttributeDescriptionCount = 2;//顶点输入属性数量
		vi.pVertexAttributeDescriptions = createInfo->getVertexAttributes();//顶点输入属性描述列表

		VkPipelineInputAssemblyStateCreateInfo ia;//管线图元组装状态创建信息
		ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		ia.pNext = NULL;//自定义数据的指针
		ia.flags = 0;//供将来使用的标志
		ia.primitiveRestartEnable = VK_FALSE;//关闭图元重启
		ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;	//采用三角形图元列表模式

		VkPipelineRasterizationStateCreateInfo rs;//管线光栅化状态创建信息
		rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rs.pNext = NULL;//自定义数据的指针
		rs.flags = 0;//供将来使用的标志
		rs.polygonMode = createInfo->getPolygonMode();//绘制方式为填充
		rs.cullMode = VK_CULL_MODE_NONE;//不使用背面剪裁
		rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;	//卷绕方向为逆时针
		rs.depthClampEnable = VK_FALSE;//深度截取
		rs.rasterizerDiscardEnable = VK_FALSE;//启用光栅化操作（若为TRUE则光栅化不产生任何片元）
		rs.depthBiasEnable = VK_FALSE;//不启用深度偏移
		rs.depthBiasConstantFactor = 0;	//深度偏移常量因子
		rs.depthBiasClamp = 0;//深度偏移值上下限（若为正作为上限，为负作为下限）
		rs.depthBiasSlopeFactor = 0;//深度偏移斜率因子
		rs.lineWidth = createInfo->getLineWidth();//线宽度（仅在线绘制模式起作用）

		VkPipelineColorBlendAttachmentState att_state[1];//管线颜色混合附件状态数组
		att_state[0].colorWriteMask = 0xf;//设置写入掩码
		att_state[0].blendEnable = VK_TRUE;//关闭混合
		att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;//设置Alpha通道混合方式
		att_state[0].colorBlendOp = VK_BLEND_OP_ADD;//设置RGB通道混合方式
		att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;//设置源颜色混合因子
		att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;//设置目标颜色混合因子
		att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;//设置源Alpha混合因子
		att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;//设置目标Alpha混合因子


		VkPipelineColorBlendStateCreateInfo cb;//管线的颜色混合状态创建信息
		cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		cb.pNext = NULL;//自定义数据的指针
		cb.flags = 0;//供未来使用的标志
		cb.attachmentCount = 1;	//颜色混合附件数量
		cb.pAttachments = att_state;//颜色混合附件列表
		cb.logicOpEnable = VK_TRUE;//启用逻辑操作，原来不启用
		cb.logicOp = VK_LOGIC_OP_NO_OP;//逻辑操作类型为无
		cb.blendConstants[0] = 1.0f;//混合常量R分量
		cb.blendConstants[1] = 1.0f;//混合常量G分量
		cb.blendConstants[2] = 1.0f;//混合常量B分量
		cb.blendConstants[3] = 1.0f;//混合常量A分量

		VkViewport viewports;//视口信息
		viewports.minDepth = 0.0f;//视口最小深度
		viewports.maxDepth = 1.0f;//视口最大深度
		viewports.x = 0;//视口X坐标
		viewports.y = 0;//视口Y坐标
		viewports.width = screenWidth;//视口宽度
		viewports.height = screenHeight;//视口高度

		VkRect2D scissor;//剪裁窗口信息
		scissor.extent.width = screenWidth;//剪裁窗口的宽度
		scissor.extent.height = screenHeight;//剪裁窗口的高度
		scissor.offset.x = 0;//剪裁窗口的X坐标
		scissor.offset.y = 0;//剪裁窗口的Y坐标

		VkPipelineViewportStateCreateInfo vp = {};//管线视口状态创建信息
		vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vp.pNext = NULL;//自定义数据的指针
		vp.flags = 0;//供将来使用的标志
		vp.viewportCount = 1;//视口的数量
		vp.scissorCount = 1;//剪裁窗口的数量
		vp.pScissors = &scissor;//剪裁窗口信息列表
		vp.pViewports = &viewports;//视口信息列表

		VkPipelineDepthStencilStateCreateInfo ds;//管线深度及模板状态创建信息
		ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		ds.pNext = NULL;//自定义数据的指针
		ds.flags = 0;//供将来使用的标志
		ds.depthTestEnable = createInfo->isDepthTestEnabled();//开启深度测试
		ds.depthWriteEnable = VK_TRUE;//开启深度值写入
		ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;//深度检测比较操作
		ds.depthBoundsTestEnable = VK_FALSE;//关闭深度边界测试
		ds.minDepthBounds = 0;//最小深度边界
		ds.maxDepthBounds = 0;//最大深度边界
		ds.stencilTestEnable = createInfo->isStencilTestEnabled();
		ds.back.failOp = VK_STENCIL_OP_KEEP;//未通过模板测试时的操作
		ds.back.passOp = VK_STENCIL_OP_KEEP;//模板测试、深度测试都通过时的操作
		ds.back.compareOp = VK_COMPARE_OP_ALWAYS;//模板测试的比较操作
		ds.back.compareMask = 0;//模板测试比较掩码
		ds.back.reference = 0;//模板测试参考值
		ds.back.depthFailOp = VK_STENCIL_OP_KEEP;//未通过深度测试时的操作
		ds.back.writeMask = 0;//写入掩码
		ds.front = ds.back;

		VkPipelineMultisampleStateCreateInfo ms;//管线多重采样状态创建信息
		ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		ms.pNext = NULL;//自定义数据的指针
		ms.flags = 0;//供将来使用的标志位
		ms.pSampleMask = NULL;//采样掩码
		ms.rasterizationSamples = createInfo->getMultiSampleLevelFlag();//光栅化阶段采样数量
		ms.sampleShadingEnable = VK_FALSE;//关闭采样着色
		ms.alphaToCoverageEnable = VK_FALSE;//不启用alphaToCoverage
		ms.alphaToOneEnable = VK_FALSE;//不启用alphaToOne
		ms.minSampleShading = 0.0;//最小采样着色

		VkGraphicsPipelineCreateInfo pipelineInfo;//图形管线创建信息
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = NULL;//自定义数据的指针
		pipelineInfo.layout = vkPipelineLayout;//指定管线布局
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;//基管线句柄
		pipelineInfo.basePipelineIndex = 0;//基管线索引
		pipelineInfo.flags = 0;	//标志
		pipelineInfo.pVertexInputState = &vi;//管线的顶点数据输入状态信息
		pipelineInfo.pInputAssemblyState = &ia;//管线的图元组装状态信息
		pipelineInfo.pRasterizationState = &rs;//管线的光栅化状态信息
		pipelineInfo.pColorBlendState = &cb;//管线的颜色混合状态信息
		pipelineInfo.pTessellationState = NULL;//管线的曲面细分状态信息
		pipelineInfo.pMultisampleState = &ms;//管线的多重采样状态信息
		pipelineInfo.pDynamicState = &dynamicState;//管线的动态状态信息
		pipelineInfo.pViewportState = &vp;//管线的视口状态信息
		pipelineInfo.pDepthStencilState = &ds; //管线的深度模板测试状态信息
		pipelineInfo.stageCount = 2;//管线的着色阶段数量
		pipelineInfo.pStages = vkShaderStages;//管线的着色阶段列表
		pipelineInfo.renderPass = vkRenderPass;//指定的渲染通道
		pipelineInfo.subpass = 0;//设置管线执行对应的渲染子通道

		VkPipelineCacheCreateInfo pipelineCacheInfo;//管线缓冲创建信息
		pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		pipelineCacheInfo.pNext = NULL;//自定义数据的指针
		pipelineCacheInfo.initialDataSize = 0;//初始数据尺寸
		pipelineCacheInfo.pInitialData = NULL;//初始数据内容，此处为NULL
		pipelineCacheInfo.flags = 0;//供将来使用的标志位

		VkResult result = vkCreatePipelineCache(vkDevice, &pipelineCacheInfo, NULL, &vkPipelineCache);//创建管线缓冲
		VkAssert(result == VK_SUCCESS, "vkCreatePipelineCache");//检查管线缓冲创建是否成功
		result = vkCreateGraphicsPipelines(vkDevice, vkPipelineCache, 1, &pipelineInfo, NULL, &vkPipeline);//创建管线
		VkAssert(result == VK_SUCCESS, "vkCreateGraphicsPipelines");//检查管线创建是否成功
	}
	void DestroyPipeline()//销毁管线
	{
		//销毁管线
		vkDestroyPipeline(vkDevice, vkPipeline, NULL);
		//销毁管线缓冲
		vkDestroyPipelineCache(vkDevice, vkPipelineCache, NULL);
	}

public:
	vector<VkDescriptorSet> vkDescriptorSet;//描述集列表
	VkPipelineLayout vkPipelineLayout;//管线布局
	VkPipeline vkPipeline;//管线
};