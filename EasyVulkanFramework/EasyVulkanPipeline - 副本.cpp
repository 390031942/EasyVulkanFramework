#include "EasyVulkanPipeline.h"

void TextureDrawPipeline::CreateUniformBuffer()
{
	/*ƬԪ��ɫ*/
	vkBufferByteCount_FS = sizeof(float) * 1;//һ�±�����������ֽ���

	VkBufferCreateInfo buf_info = {};//����һ�±������崴����Ϣ�ṹ��ʵ��
	buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;	//�ṹ�������
	buf_info.pNext = NULL;//�Զ������ݵ�ָ��
	buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;//�������;
	buf_info.size = vkBufferByteCount_FS;//�������ֽ���
	buf_info.queueFamilyIndexCount = 0;	//���м�������
	buf_info.pQueueFamilyIndices = NULL;//���м��������б�
	buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;//����ģʽ
	buf_info.flags = 0;//��־

	VkResult result = vkCreateBuffer(vkDevice, &buf_info, NULL, &vkUniformBuffer_FS);//����һ�±�������
	VkAssert(result == VK_SUCCESS, "vkCreateBuffer");//��鴴���Ƿ�ɹ�

	VkMemoryRequirements mem_reqs;
	vkGetBufferMemoryRequirements(vkDevice, vkUniformBuffer_FS, &mem_reqs);//��ȡ�˻�����ڴ�����

	VkMemoryAllocateInfo alloc_info = {};//�����ڴ������Ϣ�ṹ��ʵ��
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;//�ṹ������
	alloc_info.pNext = NULL;//�Զ������ݵ�ָ��
	alloc_info.memoryTypeIndex = 0;//�ڴ���������
	alloc_info.allocationSize = mem_reqs.size;//�����ڴ�����ֽ���

	VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;//��Ҫ���ڴ���������
	bool flag = memoryTypeFromProperties(vkMemoryProperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);	//��ȡ�����ڴ���������
	if (flag) { printf("ȷ���ڴ����ͳɹ� ��������Ϊ%d", alloc_info.memoryTypeIndex); }
	else { printf("ȷ���ڴ�����ʧ��!"); }

	result = vkAllocateMemory(vkDevice, &alloc_info, NULL, &vkUniformBufferMemory_FS);//�����ڴ�
	VkAssert(result == VK_SUCCESS, "vkAllocateMemory");//����ڴ�����Ƿ�ɹ�
	result = vkBindBufferMemory(vkDevice, vkUniformBuffer_FS, vkUniformBufferMemory_FS, 0);//���ڴ�Ͷ�Ӧ�����
	VkAssert(result == VK_SUCCESS, "vkBindBufferMemory");//���󶨲����Ƿ�ɹ�

	vkUniformBufferInfo_FS.buffer = vkUniformBuffer_FS;//ָ��һ�±�������
	vkUniformBufferInfo_FS.offset = 0;//��ʼƫ����
	vkUniformBufferInfo_FS.range = 1;//һ�±����������ֽ���
}

void TextureDrawPipeline::DestoryUniformBuffer()
{
	vkDestroyBuffer(vkDevice, vkUniformBuffer_FS, NULL);//����һ�±�������
	vkFreeMemory(vkDevice, vkUniformBufferMemory_FS, NULL);//�ͷ�һ�±��������Ӧ�豸�ڴ�
}

void TextureDrawPipeline::CreatePipelineLayout()
{
	//��2���3
	VkDescriptorSetLayoutBinding layout_bindings[2];//���������ְ�����

    layout_bindings[0].binding = 0;//�˰󶨵İ󶨵���
	layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//��������
	layout_bindings[0].descriptorCount = 1;//��������
	layout_bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;	//Ŀ����ɫ���׶�
	layout_bindings[0].pImmutableSamplers = NULL;

	layout_bindings[1].binding = 1;
	layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layout_bindings[1].descriptorCount = 1;
	layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	layout_bindings[1].pImmutableSamplers = NULL;

	VkDescriptorSetLayoutCreateInfo descriptor_layout = {};	//�������������ִ�����Ϣ�ṹ��ʵ��
	descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;//�ṹ������
	descriptor_layout.pNext = NULL;//�Զ������ݵ�ָ��
	descriptor_layout.bindingCount = 2;//���������ְ󶨵�����
	descriptor_layout.pBindings = layout_bindings;//���������ְ�����

	//�����ռ�
	vkDescriptorLayouts.resize(NUM_DESCRIPTOR_SETS);//���������������б�ߴ�
	VkResult result = vkCreateDescriptorSetLayout(vkDevice, &descriptor_layout, NULL, vkDescriptorLayouts.data());//��������������
	VkAssert(result == VK_SUCCESS,"vkCreateDescriptorSetLayout");//������������ִ����Ƿ�ɹ�

	const unsigned push_constant_range_count = 1;
	VkPushConstantRange push_constant_ranges[push_constant_range_count] = {};
	push_constant_ranges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	push_constant_ranges[0].offset = 0;
	push_constant_ranges[0].size = sizeof(float) * 16;

	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};//�������߲��ִ�����Ϣ�ṹ��ʵ��
	pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;	//�ṹ������
	pPipelineLayoutCreateInfo.pNext = NULL;//�Զ������ݵ�ָ��
	pPipelineLayoutCreateInfo.pushConstantRangeCount = 1;//���ͳ�����Χ������
	pPipelineLayoutCreateInfo.pPushConstantRanges = push_constant_ranges;//���ͳ�����Χ���б�
	pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;//���������ֵ�����
	pPipelineLayoutCreateInfo.pSetLayouts = vkDescriptorLayouts.data();//�����������б�

	result = vkCreatePipelineLayout(vkDevice, &pPipelineLayoutCreateInfo, NULL, &vkPipelineLayout);//�������߲���
	VkAssert(result == VK_SUCCESS,"vkCreatePipelineLayout");//��鴴���Ƿ�ɹ�
}

void TextureDrawPipeline::DestoryPipelineLayout()
{
	for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++)//�����������б�
	{
		vkDestroyDescriptorSetLayout(vkDevice, vkDescriptorLayouts[i], NULL);//���ٶ�Ӧ����������
	}
	vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, NULL);//���ٹ��߲���
}

void TextureDrawPipeline::InitDescriptorSet()
{
	//��2��3
	VkDescriptorPoolSize type_count[2];//�������سߴ�ʵ������

	type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//��������
	type_count[0].descriptorCount = 1024;//��������

	//Ϊ�� ��Ⱦ��ͼ �������ӵ�
	type_count[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;//��������
	type_count[1].descriptorCount = 1024;//��������

	VkDescriptorPoolCreateInfo descriptor_pool = {};//�����������ش�����Ϣ�ṹ��ʵ��
	descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;//�ṹ������
	descriptor_pool.pNext = NULL;//�Զ������ݵ�ָ��
	descriptor_pool.maxSets = 1024;//�������������
	descriptor_pool.poolSizeCount = 2;//�������سߴ�ʵ������
	descriptor_pool.pPoolSizes = type_count;//�������سߴ�ʵ������

	VkResult result = vkCreateDescriptorPool(vkDevice, &descriptor_pool, NULL, &vkDescriptorPool);//������������
	VkAssert(result == VK_SUCCESS,"vkCreateDescriptorPool");//����������ش����Ƿ�ɹ�

	std::vector<VkDescriptorSetLayout> layouts;//�����������б�
	for (int i = 0; i < 1024; i++)
	{
		layouts.push_back(vkDescriptorLayouts[0]);//���б������ָ������������
	}


	VkDescriptorSetAllocateInfo alloc_info[1];//����������������Ϣ�ṹ��ʵ������
	alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;//�ṹ������
	alloc_info[0].pNext = NULL;//�Զ������ݵ�ָ��
	alloc_info[0].descriptorPool = vkDescriptorPool;//ָ����������
	alloc_info[0].descriptorSetCount = 1024;//����������
	alloc_info[0].pSetLayouts = layouts.data();//�����������б�

	vkDescriptorSet.resize(1024);//�����������б�ߴ�
	result = vkAllocateDescriptorSets(vkDevice, alloc_info, vkDescriptorSet.data());//����������
	VkAssert(result == VK_SUCCESS,"vkAllocateDescriptorSets");//��������������Ƿ�ɹ�

	vkWriteDescriptorSet[0] = {}; //����һ�±���д��������ʵ������
	vkWriteDescriptorSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;//�ṹ������
	vkWriteDescriptorSet[0].pNext = NULL;	//�Զ������ݵ�ָ��
	vkWriteDescriptorSet[0].pBufferInfo = &vkUniformBufferInfo_FS;//��Ӧһ�±����������Ϣ
	vkWriteDescriptorSet[0].descriptorCount = 1;//��������
	vkWriteDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//��������
	vkWriteDescriptorSet[0].dstArrayElement = 0;//Ŀ��������ʼԪ��
	vkWriteDescriptorSet[0].dstBinding = 0;//Ŀ��󶨱��

	vkWriteDescriptorSet[1] = {};
	vkWriteDescriptorSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vkWriteDescriptorSet[1].pNext = NULL;	//�Զ������ݵ�ָ��
	vkWriteDescriptorSet[1].dstBinding = 1;
	vkWriteDescriptorSet[1].descriptorCount = 1;
	vkWriteDescriptorSet[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	vkWriteDescriptorSet[1].pBufferInfo = &vkUniformBufferInfo_FS;//��Ӧһ�±����������Ϣ
	vkWriteDescriptorSet[1].dstArrayElement = 0;
}

void TextureDrawPipeline::CreateShader()
{
	vkShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vkShaderStages[0].pNext = NULL;//�Զ������ݵ�ָ��
	vkShaderStages[0].pSpecializationInfo = NULL;//������Ϣ
	vkShaderStages[0].flags = 0;//������ʹ�õı�־
	vkShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;	//��ɫ���׶�Ϊ����
	vkShaderStages[0].pName = "main";//��ں���Ϊmain
	vkShaderStages[0].module = EasyVulkanShaderCompiler::CompilerVertexShader("shaders/vertshadertext.vert");

	vkShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;//�ṹ������
	vkShaderStages[1].pNext = NULL;//�Զ������ݵ�ָ��
	vkShaderStages[1].pSpecializationInfo = NULL;//������Ϣ
	vkShaderStages[1].flags = 0;//������ʹ�õı�־
	vkShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;//��ɫ���׶�ΪƬԪ
	vkShaderStages[1].pName = "main";//��ں���Ϊmain
	vkShaderStages[1].module = EasyVulkanShaderCompiler::CompilerFragmentShader("shaders/fragshadertext.frag");

}

void TextureDrawPipeline::DestroyShader()
{
	vkDestroyShaderModule(vkDevice, vkShaderStages[0].module, NULL);//���ٶ�����ɫ��ģ��
	vkDestroyShaderModule(vkDevice, vkShaderStages[1].module, NULL);//����ƬԪ��ɫ��ģ��
}

void TextureDrawPipeline::InitVertexAttributeInfo()
{
	vkVertexBinding.binding = 0;//��Ӧ�󶨵�
	vkVertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;	//��������Ƶ��Ϊÿ����
	vkVertexBinding.stride = sizeof(float) * 5;//ÿ�����ݵĿ���ֽ���

	vkVertexAttribs[0].binding = 0;//��1�������������Եİ󶨵�
	vkVertexAttribs[0].location = 0;//��1�������������Ե�λ������
	vkVertexAttribs[0].format = VK_FORMAT_R32G32B32_SFLOAT;//��1�������������Ե����ݸ�ʽ
	vkVertexAttribs[0].offset = 0;//��1�������������Ե�ƫ����

	vkVertexAttribs[1].binding = 0;//��2�������������Եİ󶨵�
	vkVertexAttribs[1].location = 1;//��2�������������Ե�λ������
	vkVertexAttribs[1].format = VK_FORMAT_R32G32_SFLOAT;//��2�������������Ե����ݸ�ʽ
	vkVertexAttribs[1].offset = 12;//��2�������������Ե�ƫ����
}

void TextureDrawPipeline::CreatePipeline()
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
	vi.pVertexBindingDescriptions = &vkVertexBinding;//��������������б�
	vi.vertexAttributeDescriptionCount = 2;//����������������
	vi.pVertexAttributeDescriptions = vkVertexAttribs;//�����������������б�

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
	rs.polygonMode = VK_POLYGON_MODE_FILL;//���Ʒ�ʽΪ���
	rs.cullMode = VK_CULL_MODE_NONE;//��ʹ�ñ������
	rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;	//���Ʒ���Ϊ��ʱ��
	rs.depthClampEnable = VK_FALSE;//��Ƚ�ȡ
	rs.rasterizerDiscardEnable = VK_FALSE;//���ù�դ����������ΪTRUE���դ���������κ�ƬԪ��
	rs.depthBiasEnable = VK_FALSE;//���������ƫ��
	rs.depthBiasConstantFactor = 0;	//���ƫ�Ƴ�������
	rs.depthBiasClamp = 0;//���ƫ��ֵ�����ޣ���Ϊ����Ϊ���ޣ�Ϊ����Ϊ���ޣ�
	rs.depthBiasSlopeFactor = 0;//���ƫ��б������
	rs.lineWidth = 1.0f;//�߿�ȣ������߻���ģʽ�����ã�

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
	viewports.height =screenHeight;//�ӿڸ߶�

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
	ds.depthTestEnable = VK_TRUE;//������Ȳ���
	ds.depthWriteEnable = VK_TRUE;//�������ֵд��
	ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;//��ȼ��Ƚϲ���
	ds.depthBoundsTestEnable = VK_FALSE;//�ر���ȱ߽����
	ds.minDepthBounds = 0;//��С��ȱ߽�
	ds.maxDepthBounds = 0;//�����ȱ߽�
	ds.stencilTestEnable = VK_FALSE;
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
	ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;//��դ���׶β�������
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
	VkAssert(result == VK_SUCCESS,"vkCreatePipelineCache");//�����߻��崴���Ƿ�ɹ�
	result = vkCreateGraphicsPipelines(vkDevice, vkPipelineCache, 1, &pipelineInfo, NULL, &vkPipeline);//��������
	VkAssert(result == VK_SUCCESS,"vkCreateGraphicsPipelines");//�����ߴ����Ƿ�ɹ�
}

void TextureDrawPipeline::DestroyPipeline()
{
	//���ٹ���
	vkDestroyPipeline(vkDevice, vkPipeline, NULL);
	//���ٹ��߻���
	vkDestroyPipelineCache(vkDevice, vkPipelineCache, NULL);
}

TextureDrawPipeline::TextureDrawPipeline()
{

	CreateUniformBuffer();//����һ�±�������
	CreatePipelineLayout();//�������߲���
	InitDescriptorSet();//��ʼ��������
	EasyVulkanShaderCompiler::Init();
	CreateShader();//������ɫ��
	InitVertexAttributeInfo();//��ʼ������������Ϣ
	CreatePipeline();//��������
}

TextureDrawPipeline::~TextureDrawPipeline()
{
	DestroyPipeline();//���ٹ���
	DestroyShader();//������ɫ��ģ��
	DestoryPipelineLayout();//���ٹ��߲���
	DestoryUniformBuffer();//����һ�±�������
}


void BasicDrawPipeline::CreatePipelineLayout()
{
	VkDescriptorSetLayoutCreateInfo descriptor_layout = {};	//�������������ִ�����Ϣ�ṹ��ʵ��
	descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;//�ṹ������
	descriptor_layout.pNext = NULL;//�Զ������ݵ�ָ��
	descriptor_layout.bindingCount = 0;//���������ְ󶨵�����
	descriptor_layout.pBindings = NULL;//���������ְ�����

	//�����ռ�
	vkDescriptorLayouts.resize(NUM_DESCRIPTOR_SETS);//���������������б�ߴ�
	VkResult result = vkCreateDescriptorSetLayout(vkDevice, &descriptor_layout, NULL, vkDescriptorLayouts.data());//��������������
	VkAssert(result == VK_SUCCESS, "vkCreateDescriptorSetLayout");//������������ִ����Ƿ�ɹ�

	const unsigned push_constant_range_count = 1;
	VkPushConstantRange push_constant_ranges[push_constant_range_count] = {};
	push_constant_ranges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	push_constant_ranges[0].offset = 0;
	push_constant_ranges[0].size = sizeof(float) * 16;

	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};//�������߲��ִ�����Ϣ�ṹ��ʵ��
	pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;	//�ṹ������
	pPipelineLayoutCreateInfo.pNext = NULL;//�Զ������ݵ�ָ��
	pPipelineLayoutCreateInfo.pushConstantRangeCount = 1;//���ͳ�����Χ������
	pPipelineLayoutCreateInfo.pPushConstantRanges = push_constant_ranges;//���ͳ�����Χ���б�
	pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;//���������ֵ�����
	pPipelineLayoutCreateInfo.pSetLayouts = vkDescriptorLayouts.data();//�����������б�

	result = vkCreatePipelineLayout(vkDevice, &pPipelineLayoutCreateInfo, NULL, &vkPipelineLayout);//�������߲���
	VkAssert(result == VK_SUCCESS, "vkCreatePipelineLayout");//��鴴���Ƿ�ɹ�
}

void BasicDrawPipeline::DestoryPipelineLayout()
{
	for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++)//�����������б�
	{
		vkDestroyDescriptorSetLayout(vkDevice, vkDescriptorLayouts[i], NULL);//���ٶ�Ӧ����������
	}
	vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, NULL);//���ٹ��߲���
}

void BasicDrawPipeline::InitDescriptorSet()
{
	//��2��3
	VkDescriptorPoolSize type_count[2];//�������سߴ�ʵ������

	type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//��������
	type_count[0].descriptorCount = 1024;//��������

	//Ϊ�� ��Ⱦ��ͼ �������ӵ�
	type_count[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;//��������
	type_count[1].descriptorCount = 1024;//��������

	VkDescriptorPoolCreateInfo descriptor_pool = {};//�����������ش�����Ϣ�ṹ��ʵ��
	descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;//�ṹ������
	descriptor_pool.pNext = NULL;//�Զ������ݵ�ָ��
	descriptor_pool.maxSets = 1024;//�������������
	descriptor_pool.poolSizeCount = 2;//�������سߴ�ʵ������
	descriptor_pool.pPoolSizes = type_count;//�������سߴ�ʵ������

	VkResult result = vkCreateDescriptorPool(vkDevice, &descriptor_pool, NULL, &vkDescriptorPool);//������������
	VkAssert(result == VK_SUCCESS, "vkCreateDescriptorPool");//����������ش����Ƿ�ɹ�

	std::vector<VkDescriptorSetLayout> layouts;//�����������б�
	for (int i = 0; i < 1024; i++)
	{
		layouts.push_back(vkDescriptorLayouts[0]);//���б������ָ������������
	}

	VkDescriptorSetAllocateInfo alloc_info[1];//����������������Ϣ�ṹ��ʵ������
	alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;//�ṹ������
	alloc_info[0].pNext = NULL;//�Զ������ݵ�ָ��
	alloc_info[0].descriptorPool = vkDescriptorPool;//ָ����������
	alloc_info[0].descriptorSetCount = 1024;//����������
	alloc_info[0].pSetLayouts = layouts.data();//�����������б�

	vkDescriptorSet.resize(1024);//�����������б�ߴ�
	result = vkAllocateDescriptorSets(vkDevice, alloc_info, vkDescriptorSet.data());//����������
	VkAssert(result == VK_SUCCESS, "vkAllocateDescriptorSets");//��������������Ƿ�ɹ�
}

void BasicDrawPipeline::CreateShader()
{
	vkShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vkShaderStages[0].pNext = NULL;//�Զ������ݵ�ָ��
	vkShaderStages[0].pSpecializationInfo = NULL;//������Ϣ
	vkShaderStages[0].flags = 0;//������ʹ�õı�־
	vkShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;	//��ɫ���׶�Ϊ����
	vkShaderStages[0].pName = "main";//��ں���Ϊmain
	vkShaderStages[0].module = EasyVulkanShaderCompiler::CompilerVertexShader("shaders/basic.vert");

	vkShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;//�ṹ������
	vkShaderStages[1].pNext = NULL;//�Զ������ݵ�ָ��
	vkShaderStages[1].pSpecializationInfo = NULL;//������Ϣ
	vkShaderStages[1].flags = 0;//������ʹ�õı�־
	vkShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;//��ɫ���׶�ΪƬԪ
	vkShaderStages[1].pName = "main";//��ں���Ϊmain
	vkShaderStages[1].module = EasyVulkanShaderCompiler::CompilerFragmentShader("shaders/basic.frag");

}

void BasicDrawPipeline::DestroyShader()
{
	vkDestroyShaderModule(vkDevice, vkShaderStages[0].module, NULL);//���ٶ�����ɫ��ģ��
	vkDestroyShaderModule(vkDevice, vkShaderStages[1].module, NULL);//����ƬԪ��ɫ��ģ��
}

void BasicDrawPipeline::InitVertexAttributeInfo()
{
	vkVertexBinding.binding = 0;//��Ӧ�󶨵�
	vkVertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;	//��������Ƶ��Ϊÿ����
	vkVertexBinding.stride = sizeof(float) * 7;//ÿ�����ݵĿ���ֽ���

	vkVertexAttribs[0].binding = 0;//��1�������������Եİ󶨵�
	vkVertexAttribs[0].location = 0;//��1�������������Ե�λ������
	vkVertexAttribs[0].format = VK_FORMAT_R32G32B32_SFLOAT;//��1�������������Ե����ݸ�ʽ
	vkVertexAttribs[0].offset = 0;//��1�������������Ե�ƫ����

	vkVertexAttribs[1].binding = 0;//��2�������������Եİ󶨵�
	vkVertexAttribs[1].location = 1;//��2�������������Ե�λ������
	vkVertexAttribs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;//��2�������������Ե����ݸ�ʽ
	vkVertexAttribs[1].offset = 12;//��2�������������Ե�ƫ����
}

void BasicDrawPipeline::CreatePipeline()
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
	vi.pVertexBindingDescriptions = &vkVertexBinding;//��������������б�
	vi.vertexAttributeDescriptionCount = 2;//����������������
	vi.pVertexAttributeDescriptions = vkVertexAttribs;//�����������������б�

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
	rs.polygonMode = VK_POLYGON_MODE_FILL;//���Ʒ�ʽΪ���
	rs.cullMode = VK_CULL_MODE_NONE;//��ʹ�ñ������
	rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;	//���Ʒ���Ϊ��ʱ��
	rs.depthClampEnable = VK_FALSE;//��Ƚ�ȡ
	rs.rasterizerDiscardEnable = VK_FALSE;//���ù�դ����������ΪTRUE���դ���������κ�ƬԪ��
	rs.depthBiasEnable = VK_FALSE;//���������ƫ��
	rs.depthBiasConstantFactor = 0;	//���ƫ�Ƴ�������
	rs.depthBiasClamp = 0;//���ƫ��ֵ�����ޣ���Ϊ����Ϊ���ޣ�Ϊ����Ϊ���ޣ�
	rs.depthBiasSlopeFactor = 0;//���ƫ��б������
	rs.lineWidth = 1.0f;//�߿�ȣ������߻���ģʽ�����ã�

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
	cb.logicOpEnable = VK_FALSE;//�������߼�����
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
	ds.depthTestEnable = VK_TRUE;//������Ȳ���
	ds.depthWriteEnable = VK_TRUE;//�������ֵд��
	ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;//��ȼ��Ƚϲ���
	ds.depthBoundsTestEnable = VK_FALSE;//�ر���ȱ߽����
	ds.minDepthBounds = 0;//��С��ȱ߽�
	ds.maxDepthBounds = 0;//�����ȱ߽�
	ds.stencilTestEnable = VK_FALSE;//�ر�ģ�����
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
	ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;//��դ���׶β�������
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

void BasicDrawPipeline::DestroyPipeline()
{
	//���ٹ���
	vkDestroyPipeline(vkDevice, vkPipeline, NULL);
	//���ٹ��߻���
	vkDestroyPipelineCache(vkDevice, vkPipelineCache, NULL);
}

BasicDrawPipeline::BasicDrawPipeline()
{
	CreatePipelineLayout();//�������߲���
	InitDescriptorSet();//��ʼ��������
	EasyVulkanShaderCompiler::Init();
	CreateShader();//������ɫ��
	InitVertexAttributeInfo();//��ʼ������������Ϣ
	CreatePipeline();//��������
}

BasicDrawPipeline::~BasicDrawPipeline()
{
	DestroyPipeline();//���ٹ���
	DestroyShader();//������ɫ��ģ��
	DestoryPipelineLayout();//���ٹ��߲���
}



