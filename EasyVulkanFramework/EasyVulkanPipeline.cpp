#include "EasyVulkanPipeline.h"

void EasyVulkanPipeline::CreateUniformBuffer()//����һ�±�������
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

	const VkDeviceSize allocateSize = vsUniformBufferAllocSize + fsUniformBufferAllocSize;//һ�±�����������ֽ���

	if (allocateSize == 0)
		return;

	VkBufferCreateInfo buf_info = {};//����һ�±������崴����Ϣ�ṹ��ʵ��
	buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;	//�ṹ�������
	buf_info.pNext = NULL;//�Զ������ݵ�ָ��
	buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;//�������;
	buf_info.size = allocateSize;//�������ֽ���
	buf_info.queueFamilyIndexCount = 0;	//���м�������
	buf_info.pQueueFamilyIndices = NULL;//���м��������б�
	buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//����ģʽ
	buf_info.flags = 0;//��־

	VkResult result = vkCreateBuffer(vkDevice, &buf_info, NULL, &vkUniformBuffer);//����һ�±�������
	VkAssert(result == VK_SUCCESS, "vkCreateBuffer");//��鴴���Ƿ�ɹ�

	VkMemoryRequirements mem_reqs;
	vkGetBufferMemoryRequirements(vkDevice, vkUniformBuffer, &mem_reqs);//��ȡ�˻�����ڴ�����

	VkMemoryAllocateInfo alloc_info = {};//�����ڴ������Ϣ�ṹ��ʵ��
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;//�ṹ������
	alloc_info.pNext = NULL;//�Զ������ݵ�ָ��
	alloc_info.memoryTypeIndex = 0;//�ڴ���������
	alloc_info.allocationSize = mem_reqs.size;//�����ڴ�����ֽ���

	VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;//��Ҫ���ڴ���������
	bool flag = memoryTypeFromProperties(vkMemoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);	//��ȡ�����ڴ���������
	if (flag) { printf("ȷ���ڴ����ͳɹ� ��������Ϊ%d", alloc_info.memoryTypeIndex); }
	else { printf("ȷ���ڴ�����ʧ��!"); }

	result = vkAllocateMemory(vkDevice, &alloc_info, NULL, &vkUniformBufferMemory);//�����ڴ�
	VkAssert(result == VK_SUCCESS, "vkAllocateMemory");//����ڴ�����Ƿ�ɹ�
	result = vkBindBufferMemory(vkDevice, vkUniformBuffer, vkUniformBufferMemory, 0);//���ڴ�Ͷ�Ӧ�����
	VkAssert(result == VK_SUCCESS, "vkBindBufferMemory");//���󶨲����Ƿ�ɹ�

	vkUniformBufferInfo.buffer = vkUniformBuffer;//ָ��һ�±�������
	vkUniformBufferInfo.offset = 0;//��ʼƫ����
	vkUniformBufferInfo.range = allocateSize;//һ�±����������ֽ���
}

void EasyVulkanPipeline::DestoryUniformBuffer()//����һ�±�������
{
	if (vkUniformBuffer) {
		vkDestroyBuffer(vkDevice, vkUniformBuffer, NULL);
		vkUniformBuffer = VK_NULL_HANDLE;
	}

	if (vkUniformBufferMemory) {
		vkFreeMemory(vkDevice, vkUniformBufferMemory, NULL);
		vkUniformBufferMemory = VK_NULL_HANDLE;
	}
}

void EasyVulkanPipeline::CreatePipelineLayout()//�������߲���
{
	VkDescriptorSetLayoutCreateInfo descriptor_layout = {};	//�������������ִ�����Ϣ�ṹ��ʵ��
	descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;//�ṹ������
	descriptor_layout.pNext = NULL;//�Զ������ݵ�ָ��
	descriptor_layout.bindingCount = createInfo->getDescriptorSetLayoutBindingCount();;//���������ְ󶨵�����
	descriptor_layout.pBindings = createInfo->getDescriptorSetLayoutBinding();;//���������ְ�����

																			   //�����ռ�
	vkDescriptorLayouts.resize(createInfo->getDescriptorMaxCount());//���������������б�ߴ�
	VkResult result = vkCreateDescriptorSetLayout(vkDevice, &descriptor_layout, NULL, vkDescriptorLayouts.data());//��������������
	VkAssert(result == VK_SUCCESS, "vkCreateDescriptorSetLayout");//������������ִ����Ƿ�ɹ�

	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};//�������߲��ִ�����Ϣ�ṹ��ʵ��
	pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;	//�ṹ������
	pPipelineLayoutCreateInfo.pNext = NULL;//�Զ������ݵ�ָ��
	pPipelineLayoutCreateInfo.pushConstantRangeCount = createInfo->getPushConstantRangeCount();//���ͳ�����Χ������
	pPipelineLayoutCreateInfo.pPushConstantRanges = createInfo->getPushConstantRanges();//���ͳ�����Χ���б�
	pPipelineLayoutCreateInfo.setLayoutCount = createInfo->getDescriptorMaxCount();//���������ֵ�����
	pPipelineLayoutCreateInfo.pSetLayouts = vkDescriptorLayouts.data();//�����������б�

	result = vkCreatePipelineLayout(vkDevice, &pPipelineLayoutCreateInfo, NULL, &vkPipelineLayout);//�������߲���
	VkAssert(result == VK_SUCCESS, "vkCreatePipelineLayout");//��鴴���Ƿ�ɹ�
}

void EasyVulkanPipeline::DestoryPipelineLayout()	//���ٹ��߲���
{
	for (int i = 0; i < createInfo->getDescriptorSetLayoutBindingCount(); i++)//�����������б�
	{
		vkDestroyDescriptorSetLayout(vkDevice, vkDescriptorLayouts[i], NULL);//���ٶ�Ӧ����������
	}
	vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, NULL);//���ٹ��߲���
}

void EasyVulkanPipeline::InitDescriptorSet()	//��ʼ��������
{
	VkDescriptorPoolCreateInfo descriptor_pool = {};//�����������ش�����Ϣ�ṹ��ʵ��
	descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;//�ṹ������
	descriptor_pool.pNext = NULL;//�Զ������ݵ�ָ��
	descriptor_pool.maxSets = createInfo->getDescriptorMaxCount();//�������������
	descriptor_pool.poolSizeCount = createInfo->getDescriptorSetLayoutBindingCount();//�������سߴ�ʵ������
	descriptor_pool.pPoolSizes = createInfo->getDescriptorPoolSizes();//�������سߴ�ʵ������

	VkResult result = vkCreateDescriptorPool(vkDevice, &descriptor_pool, NULL, &vkDescriptorPool);//������������
	VkAssert(result == VK_SUCCESS, "vkCreateDescriptorPool");//����������ش����Ƿ�ɹ�

	std::vector<VkDescriptorSetLayout> layouts;//�����������б�
	for (int i = 0; i < createInfo->getDescriptorMaxCount(); i++)
	{
		layouts.push_back(vkDescriptorLayouts[0]);//���б������ָ������������
	}

	VkDescriptorSetAllocateInfo alloc_info[1];//����������������Ϣ�ṹ��ʵ������
	alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;//�ṹ������
	alloc_info[0].pNext = NULL;//�Զ������ݵ�ָ��
	alloc_info[0].descriptorPool = vkDescriptorPool;//ָ����������
	alloc_info[0].descriptorSetCount = createInfo->getDescriptorMaxCount();//����������
	alloc_info[0].pSetLayouts = layouts.data();//�����������б�

	maxDescriptorSetIndex = createInfo->getDescriptorMaxCount();

	vkDescriptorSet.resize(createInfo->getDescriptorMaxCount());//�����������б�ߴ�
	result = vkAllocateDescriptorSets(vkDevice, alloc_info, vkDescriptorSet.data());//����������
	VkAssert(result == VK_SUCCESS, "vkAllocateDescriptorSets");//��������������Ƿ�ɹ�

	vkWriteDescriptorSet = new VkWriteDescriptorSet[createInfo->getDescriptorSetLayoutBindingCount()];

	for (int i = 0; i < createInfo->getDescriptorSetLayoutBindingCount(); i++)
	{
		vkWriteDescriptorSet[i] = {}; //����һ�±���д��������ʵ������
		vkWriteDescriptorSet[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;//�ṹ������
		vkWriteDescriptorSet[i].pNext = NULL;	//�Զ������ݵ�ָ��
		vkWriteDescriptorSet[i].descriptorCount = 1;//��������
		vkWriteDescriptorSet[i].descriptorType = createInfo->getDescriptorSetLayoutBinding()[i].descriptorType;//��������
		vkWriteDescriptorSet[i].dstArrayElement = 0;//Ŀ��������ʼԪ��
		vkWriteDescriptorSet[i].dstBinding = i;//Ŀ��󶨱��

		if (vkWriteDescriptorSet[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		{
			vkWriteDescriptorSet[i].pBufferInfo = &vkUniformBufferInfo;
		}
	}
}

void EasyVulkanPipeline::CreateShader()//������ɫ��
{
	vkShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vkShaderStages[0].pNext = NULL;//�Զ������ݵ�ָ��
	vkShaderStages[0].pSpecializationInfo = NULL;//������Ϣ
	vkShaderStages[0].flags = 0;//������ʹ�õı�־
	vkShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;	//��ɫ���׶�Ϊ����
	vkShaderStages[0].pName = "main";//��ں���Ϊmain
	vkShaderStages[0].module = createInfo->getVertexShaderModule();

	vkShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;//�ṹ������
	vkShaderStages[1].pNext = NULL;//�Զ������ݵ�ָ��
	vkShaderStages[1].pSpecializationInfo = NULL;//������Ϣ
	vkShaderStages[1].flags = 0;//������ʹ�õı�־
	vkShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;//��ɫ���׶�ΪƬԪ
	vkShaderStages[1].pName = "main";//��ں���Ϊmain
	vkShaderStages[1].module = createInfo->getFragmentShaderModule();
}

void EasyVulkanPipeline::DestroyShader()
{
	vkDestroyShaderModule(vkDevice, vkShaderStages[0].module, NULL);//���ٶ�����ɫ��ģ��
	vkDestroyShaderModule(vkDevice, vkShaderStages[1].module, NULL);//����ƬԪ��ɫ��ģ��
}

void EasyVulkanPipeline::CreatePipeline()
{
	VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];//��̬״̬���ñ�־
	memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);	//�������б�־Ϊfalse

	VkPipelineDynamicStateCreateInfo dynamicState = {};//���߶�̬״̬������Ϣ
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;//�ṹ������
	dynamicState.pNext = NULL;//�Զ������ݵ�ָ��
	dynamicState.pDynamicStates = dynamicStateEnables;//��̬״̬���ñ�־����
	dynamicState.dynamicStateCount = 0;//���õĶ�̬״̬������

	VkPipelineVertexInputStateCreateInfo vi;//���߶�����������״̬������Ϣ
	vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vi.pNext = NULL;//�Զ������ݵ�ָ��
	vi.flags = 0;//������ʹ�õı�־
	vi.vertexBindingDescriptionCount = 1;//�����������������
	vi.pVertexBindingDescriptions = createInfo->getVertexBinding();//��������������б�
	vi.vertexAttributeDescriptionCount = 2;//����������������
	vi.pVertexAttributeDescriptions = createInfo->getVertexAttributes();//�����������������б�

	VkPipelineInputAssemblyStateCreateInfo ia;//����ͼԪ��װ״̬������Ϣ
	ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia.pNext = NULL;//�Զ������ݵ�ָ��
	ia.flags = 0;//������ʹ�õı�־
	ia.primitiveRestartEnable = VK_FALSE;//�ر�ͼԪ����
	ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;	//����������ͼԪ�б�ģʽ

	VkPipelineRasterizationStateCreateInfo rs;//���߹�դ��״̬������Ϣ
	rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.pNext = NULL;//�Զ������ݵ�ָ��
	rs.flags = 0;//������ʹ�õı�־
	rs.polygonMode = createInfo->getPolygonMode();//���Ʒ�ʽΪ���
	rs.cullMode = VK_CULL_MODE_NONE;//��ʹ�ñ������
	rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;	//���Ʒ���Ϊ��ʱ��
	rs.depthClampEnable = VK_FALSE;//��Ƚ�ȡ
	rs.rasterizerDiscardEnable = VK_FALSE;//���ù�դ����������ΪTRUE���դ���������κ�ƬԪ��
	rs.depthBiasEnable = VK_FALSE;//���������ƫ��
	rs.depthBiasConstantFactor = 0;	//���ƫ�Ƴ�������
	rs.depthBiasClamp = 0;//���ƫ��ֵ�����ޣ���Ϊ����Ϊ���ޣ�Ϊ����Ϊ���ޣ�
	rs.depthBiasSlopeFactor = 0;//���ƫ��б������
	rs.lineWidth = createInfo->getLineWidth();//�߿�ȣ������߻���ģʽ�����ã�

	VkPipelineColorBlendAttachmentState att_state[1];//������ɫ��ϸ���״̬����
	att_state[0].colorWriteMask = 0xf;//����д������
	att_state[0].blendEnable = VK_TRUE;//�رջ��
	att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;//����Alphaͨ����Ϸ�ʽ
	att_state[0].colorBlendOp = VK_BLEND_OP_ADD;//����RGBͨ����Ϸ�ʽ
	att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;//����Դ��ɫ�������
	att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;//����Ŀ����ɫ�������
	att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;//����ԴAlpha�������
	att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;//����Ŀ��Alpha�������


	VkPipelineColorBlendStateCreateInfo cb;//���ߵ���ɫ���״̬������Ϣ
	cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cb.pNext = NULL;//�Զ������ݵ�ָ��
	cb.flags = 0;//��δ��ʹ�õı�־
	cb.attachmentCount = 1;	//��ɫ��ϸ�������
	cb.pAttachments = att_state;//��ɫ��ϸ����б�
	cb.logicOpEnable = VK_TRUE;//�����߼�������ԭ��������
	cb.logicOp = VK_LOGIC_OP_NO_OP;//�߼���������Ϊ��
	cb.blendConstants[0] = 1.0f;//��ϳ���R����
	cb.blendConstants[1] = 1.0f;//��ϳ���G����
	cb.blendConstants[2] = 1.0f;//��ϳ���B����
	cb.blendConstants[3] = 1.0f;//��ϳ���A����

	VkViewport viewports;//�ӿ���Ϣ
	viewports.minDepth = 0.0f;//�ӿ���С���
	viewports.maxDepth = 1.0f;//�ӿ�������
	viewports.x = 0;//�ӿ�X����
	viewports.y = 0;//�ӿ�Y����
	viewports.width = screenWidth;//�ӿڿ��
	viewports.height = screenHeight;//�ӿڸ߶�

	VkRect2D scissor;//���ô�����Ϣ
	scissor.extent.width = screenWidth;//���ô��ڵĿ��
	scissor.extent.height = screenHeight;//���ô��ڵĸ߶�
	scissor.offset.x = 0;//���ô��ڵ�X����
	scissor.offset.y = 0;//���ô��ڵ�Y����

	VkPipelineViewportStateCreateInfo vp = {};//�����ӿ�״̬������Ϣ
	vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vp.pNext = NULL;//�Զ������ݵ�ָ��
	vp.flags = 0;//������ʹ�õı�־
	vp.viewportCount = 1;//�ӿڵ�����
	vp.scissorCount = 1;//���ô��ڵ�����
	vp.pScissors = &scissor;//���ô�����Ϣ�б�
	vp.pViewports = &viewports;//�ӿ���Ϣ�б�

	VkPipelineDepthStencilStateCreateInfo ds;//������ȼ�ģ��״̬������Ϣ
	ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	ds.pNext = NULL;//�Զ������ݵ�ָ��
	ds.flags = 0;//������ʹ�õı�־
	ds.depthTestEnable = createInfo->isDepthTestEnabled();//������Ȳ���
	ds.depthWriteEnable = VK_TRUE;//�������ֵд��
	ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;//��ȼ��Ƚϲ���
	ds.depthBoundsTestEnable = VK_FALSE;//�ر���ȱ߽����
	ds.minDepthBounds = 0;//��С��ȱ߽�
	ds.maxDepthBounds = 0;//�����ȱ߽�
	ds.stencilTestEnable = createInfo->isStencilTestEnabled();
	ds.back.failOp = VK_STENCIL_OP_KEEP;//δͨ��ģ�����ʱ�Ĳ���
	ds.back.passOp = VK_STENCIL_OP_KEEP;//ģ����ԡ���Ȳ��Զ�ͨ��ʱ�Ĳ���
	ds.back.compareOp = VK_COMPARE_OP_ALWAYS;//ģ����ԵıȽϲ���
	ds.back.compareMask = 0;//ģ����ԱȽ�����
	ds.back.reference = 0;//ģ����Բο�ֵ
	ds.back.depthFailOp = VK_STENCIL_OP_KEEP;//δͨ����Ȳ���ʱ�Ĳ���
	ds.back.writeMask = 0;//д������
	ds.front = ds.back;

	VkPipelineMultisampleStateCreateInfo ms;//���߶��ز���״̬������Ϣ
	ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ms.pNext = NULL;//�Զ������ݵ�ָ��
	ms.flags = 0;//������ʹ�õı�־λ
	ms.pSampleMask = NULL;//��������
	ms.rasterizationSamples = createInfo->getMultiSampleLevelFlag();//��դ���׶β�������
	ms.sampleShadingEnable = VK_FALSE;//�رղ�����ɫ
	ms.alphaToCoverageEnable = VK_FALSE;//������alphaToCoverage
	ms.alphaToOneEnable = VK_FALSE;//������alphaToOne
	ms.minSampleShading = 0.0;//��С������ɫ

	VkGraphicsPipelineCreateInfo pipelineInfo;//ͼ�ι��ߴ�����Ϣ
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = NULL;//�Զ������ݵ�ָ��
	pipelineInfo.layout = vkPipelineLayout;//ָ�����߲���
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;//�����߾��
	pipelineInfo.basePipelineIndex = 0;//����������
	pipelineInfo.flags = 0;	//��־
	pipelineInfo.pVertexInputState = &vi;//���ߵĶ�����������״̬��Ϣ
	pipelineInfo.pInputAssemblyState = &ia;//���ߵ�ͼԪ��װ״̬��Ϣ
	pipelineInfo.pRasterizationState = &rs;//���ߵĹ�դ��״̬��Ϣ
	pipelineInfo.pColorBlendState = &cb;//���ߵ���ɫ���״̬��Ϣ
	pipelineInfo.pTessellationState = NULL;//���ߵ�����ϸ��״̬��Ϣ
	pipelineInfo.pMultisampleState = &ms;//���ߵĶ��ز���״̬��Ϣ
	pipelineInfo.pDynamicState = &dynamicState;//���ߵĶ�̬״̬��Ϣ
	pipelineInfo.pViewportState = &vp;//���ߵ��ӿ�״̬��Ϣ
	pipelineInfo.pDepthStencilState = &ds; //���ߵ����ģ�����״̬��Ϣ
	pipelineInfo.stageCount = 2;//���ߵ���ɫ�׶�����
	pipelineInfo.pStages = vkShaderStages;//���ߵ���ɫ�׶��б�
	pipelineInfo.renderPass = vkRenderPass;//ָ������Ⱦͨ��
	pipelineInfo.subpass = 0;//���ù���ִ�ж�Ӧ����Ⱦ��ͨ��

	VkPipelineCacheCreateInfo pipelineCacheInfo;//���߻��崴����Ϣ
	pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipelineCacheInfo.pNext = NULL;//�Զ������ݵ�ָ��
	pipelineCacheInfo.initialDataSize = 0;//��ʼ���ݳߴ�
	pipelineCacheInfo.pInitialData = NULL;//��ʼ�������ݣ��˴�ΪNULL
	pipelineCacheInfo.flags = 0;//������ʹ�õı�־λ

	VkResult result = vkCreatePipelineCache(vkDevice, &pipelineCacheInfo, NULL, &vkPipelineCache);//�������߻���
	VkAssert(result == VK_SUCCESS, "vkCreatePipelineCache");//�����߻��崴���Ƿ�ɹ�
	result = vkCreateGraphicsPipelines(vkDevice, vkPipelineCache, 1, &pipelineInfo, NULL, &vkPipeline);//��������
	VkAssert(result == VK_SUCCESS, "vkCreateGraphicsPipelines");//�����ߴ����Ƿ�ɹ�
}

void EasyVulkanPipeline::DestroyPipeline()//���ٹ���
{
	//���ٹ���
	vkDestroyPipeline(vkDevice, vkPipeline, NULL);
	//���ٹ��߻���
	vkDestroyPipelineCache(vkDevice, vkPipelineCache, NULL);
}

bool EasyVulkanPipeline::create(EasyVulkanPipelineCreateInfo* createInfo)
{
	if (!createInfo || createInfo->checkInfoValid())
	{
		return false;
	}

	this->createInfo = createInfo;

	this->CreateUniformBuffer();
	this->CreatePipelineLayout();
	this->InitDescriptorSet();
	this->CreateShader();
	this->CreatePipeline();
	return true;
}

VkDescriptorSet* EasyVulkanPipeline::allocateDescriptorSet()
{
	if (currentDescriptorSetIndex == maxDescriptorSetIndex)
		return NULL;

	return &(this->vkDescriptorSet[currentDescriptorSetIndex++]);
}

void EasyVulkanPipeline::resetDescriptorSetIndex()
{
	currentDescriptorSetIndex = 0;
}

void EasyVulkanPipelineCreateInfo::addVertexInput(VkFormat vertexFormat)
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
		//�����Ч
		validInfo = false;
		errorMessage = "VK_FORMAT Error";
		return;
	}

	vkVertexInputBinding.binding = 0;//��Ӧ�󶨵�
	vkVertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;	//��������Ƶ��Ϊÿ����
	vkVertexInputBinding.stride = vertexOffset;//ÿ�����ݵĿ���ֽ���
}

void EasyVulkanPipelineCreateInfo::setVertexShader(std::string path)
{
	QFile file(path.c_str());

	if (!file.exists())
	{
		validInfo = false;
		errorMessage = "SetVertexShade Failed : can not find file" + path;
		return;
	}

	vkVertexShaderModule = EasyVulkanShaderCompiler::CompilerVertexShader(path.data());

	calledRecordBit = calledRecordBit | 0x02;
}

void EasyVulkanPipelineCreateInfo::setFragmentShader(std::string path)
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

void EasyVulkanPipelineCreateInfo::setDescriptorMaxCount(int count)
{
	vkDescriptorSetMaxCount = count;
	calledRecordBit = calledRecordBit | 0x08;
}

void EasyVulkanPipelineCreateInfo::addDescriptorLayoutBinding(VkDescriptorType type, VkShaderStageFlags stage)
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

void EasyVulkanPipelineCreateInfo::addPushConstantRange(int constantCount, VkShaderStageFlags stage)
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

void EasyVulkanPipelineCreateInfo::setMSAALevel(int level)
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

void EasyVulkanPipelineCreateInfo::setDepthTestEnable(bool enabled)
{
	this->enableDepthTest = true;

	calledRecordBit = calledRecordBit | 0x80;
}

void EasyVulkanPipelineCreateInfo::setStencilTestEnable(bool enabled)
{
	this->enableStencilTest = true;

	calledRecordBit = calledRecordBit | 0x100;
}

void EasyVulkanPipelineCreateInfo::setPolygonMode(VkPolygonMode mode, int lineWidth_if_use_lineMode)
{
	this->vkPolygonMode = mode;
	if (mode == VK_POLYGON_MODE_LINE)
	{
		vkLineWidth = max(1, lineWidth_if_use_lineMode);
	}

	calledRecordBit = calledRecordBit | 0x200;
}

void EasyVulkanPipelineCreateInfo::setUniformBytesCount(int stage_vertex_count, int stage_fragment_count)
{
	this->vsUniformBytesCount = stage_vertex_count;
	this->fsUniformBytesCount = stage_fragment_count;
}

bool EasyVulkanPipelineCreateInfo::checkInfoValid()
{
	return validInfo && (calledRecordBit == 0x3ff);
}

int EasyVulkanPipelineCreateInfo::getVertexAttributeCount()
{
	return vertexAttributeDescriptionCount;
}

VkVertexInputAttributeDescription* EasyVulkanPipelineCreateInfo::getVertexAttributes()
{
	return vkVertexAttribs;
}

VkVertexInputBindingDescription* EasyVulkanPipelineCreateInfo::getVertexBinding()
{
	return &vkVertexInputBinding;
}

VkShaderModule EasyVulkanPipelineCreateInfo::getVertexShaderModule()
{
	return vkVertexShaderModule;
}

VkShaderModule EasyVulkanPipelineCreateInfo::getFragmentShaderModule()
{
	return vkFragmentShaderModule;
}

int EasyVulkanPipelineCreateInfo::getDescriptorMaxCount()
{
	return vkDescriptorSetMaxCount;
}

int EasyVulkanPipelineCreateInfo::getDescriptorSetLayoutBindingCount()
{
	return vkDescriptorSetLayoutBindingCount;
}

VkDescriptorSetLayoutBinding* EasyVulkanPipelineCreateInfo::getDescriptorSetLayoutBinding()
{
	return vkLayoutBinding;
}

VkDescriptorPoolSize* EasyVulkanPipelineCreateInfo::getDescriptorPoolSizes()
{
	return vkDescriptPoolSize;
}

int EasyVulkanPipelineCreateInfo::getPushConstantRangeCount()
{
	return pushConstantRangeCount;
}

VkPushConstantRange* EasyVulkanPipelineCreateInfo::getPushConstantRanges()
{
	return vkPushConstantRanges;
}

VkSampleCountFlagBits EasyVulkanPipelineCreateInfo::getMultiSampleLevelFlag()
{
	return vkMultiSampleLevelFlag;
}

bool EasyVulkanPipelineCreateInfo::isDepthTestEnabled()
{
	return enableDepthTest;
}

bool EasyVulkanPipelineCreateInfo::isStencilTestEnabled()
{
	return enableStencilTest;
}

VkPolygonMode EasyVulkanPipelineCreateInfo::getPolygonMode()
{
	return vkPolygonMode;
}

int EasyVulkanPipelineCreateInfo::getLineWidth()
{
	return  vkLineWidth;
}

int EasyVulkanPipelineCreateInfo::getVertexStageUniformBytesCount()
{
	return this->vsUniformBytesCount;
}

int EasyVulkanPipelineCreateInfo::getFragmentStageUniformBytesCount()
{
	return this->fsUniformBytesCount;
}
