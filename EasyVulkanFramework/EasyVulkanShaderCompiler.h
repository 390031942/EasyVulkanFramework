#pragma once
/*SPRIV*/
#include "SPIRV/GlslangToSpv.h"
/*EASY_VULKAN*/
#include "EasyVulkanGlobal.h"
/*STD*/
#include <fstream>
using namespace std;

class EasyVulkanShaderCompiler
{
private:
	static EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type) {
		switch (shader_type) {
		case VK_SHADER_STAGE_VERTEX_BIT:
			return EShLangVertex;

		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			return EShLangTessControl;

		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			return EShLangTessEvaluation;

		case VK_SHADER_STAGE_GEOMETRY_BIT:
			return EShLangGeometry;

		case VK_SHADER_STAGE_FRAGMENT_BIT:
			return EShLangFragment;

		case VK_SHADER_STAGE_COMPUTE_BIT:
			return EShLangCompute;

		sefault:
			return EShLangVertex;
		}
	}
	static void init_resources(TBuiltInResource& Resources) {
		Resources.maxLights = 32;
		Resources.maxClipPlanes = 6;
		Resources.maxTextureUnits = 32;
		Resources.maxTextureCoords = 32;
		Resources.maxVertexAttribs = 64;
		Resources.maxVertexUniformComponents = 4096;
		Resources.maxVaryingFloats = 64;
		Resources.maxVertexTextureImageUnits = 32;
		Resources.maxCombinedTextureImageUnits = 80;
		Resources.maxTextureImageUnits = 32;
		Resources.maxFragmentUniformComponents = 4096;
		Resources.maxDrawBuffers = 32;
		Resources.maxVertexUniformVectors = 128;
		Resources.maxVaryingVectors = 8;
		Resources.maxFragmentUniformVectors = 16;
		Resources.maxVertexOutputVectors = 16;
		Resources.maxFragmentInputVectors = 15;
		Resources.minProgramTexelOffset = -8;
		Resources.maxProgramTexelOffset = 7;
		Resources.maxClipDistances = 8;
		Resources.maxComputeWorkGroupCountX = 65535;
		Resources.maxComputeWorkGroupCountY = 65535;
		Resources.maxComputeWorkGroupCountZ = 65535;
		Resources.maxComputeWorkGroupSizeX = 1024;
		Resources.maxComputeWorkGroupSizeY = 1024;
		Resources.maxComputeWorkGroupSizeZ = 64;
		Resources.maxComputeUniformComponents = 1024;
		Resources.maxComputeTextureImageUnits = 16;
		Resources.maxComputeImageUniforms = 8;
		Resources.maxComputeAtomicCounters = 8;
		Resources.maxComputeAtomicCounterBuffers = 1;
		Resources.maxVaryingComponents = 60;
		Resources.maxVertexOutputComponents = 64;
		Resources.maxGeometryInputComponents = 64;
		Resources.maxGeometryOutputComponents = 128;
		Resources.maxFragmentInputComponents = 128;
		Resources.maxImageUnits = 8;
		Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
		Resources.maxCombinedShaderOutputResources = 8;
		Resources.maxImageSamples = 0;
		Resources.maxVertexImageUniforms = 0;
		Resources.maxTessControlImageUniforms = 0;
		Resources.maxTessEvaluationImageUniforms = 0;
		Resources.maxGeometryImageUniforms = 0;
		Resources.maxFragmentImageUniforms = 8;
		Resources.maxCombinedImageUniforms = 8;
		Resources.maxGeometryTextureImageUnits = 16;
		Resources.maxGeometryOutputVertices = 256;
		Resources.maxGeometryTotalOutputComponents = 1024;
		Resources.maxGeometryUniformComponents = 1024;
		Resources.maxGeometryVaryingComponents = 64;
		Resources.maxTessControlInputComponents = 128;
		Resources.maxTessControlOutputComponents = 128;
		Resources.maxTessControlTextureImageUnits = 16;
		Resources.maxTessControlUniformComponents = 1024;
		Resources.maxTessControlTotalOutputComponents = 4096;
		Resources.maxTessEvaluationInputComponents = 128;
		Resources.maxTessEvaluationOutputComponents = 128;
		Resources.maxTessEvaluationTextureImageUnits = 16;
		Resources.maxTessEvaluationUniformComponents = 1024;
		Resources.maxTessPatchComponents = 120;
		Resources.maxPatchVertices = 32;
		Resources.maxTessGenLevel = 64;
		Resources.maxViewports = 16;
		Resources.maxVertexAtomicCounters = 0;
		Resources.maxTessControlAtomicCounters = 0;
		Resources.maxTessEvaluationAtomicCounters = 0;
		Resources.maxGeometryAtomicCounters = 0;
		Resources.maxFragmentAtomicCounters = 8;
		Resources.maxCombinedAtomicCounters = 8;
		Resources.maxAtomicCounterBindings = 1;
		Resources.maxVertexAtomicCounterBuffers = 0;
		Resources.maxTessControlAtomicCounterBuffers = 0;
		Resources.maxTessEvaluationAtomicCounterBuffers = 0;
		Resources.maxGeometryAtomicCounterBuffers = 0;
		Resources.maxFragmentAtomicCounterBuffers = 1;
		Resources.maxCombinedAtomicCounterBuffers = 1;
		Resources.maxAtomicCounterBufferSize = 16384;
		Resources.maxTransformFeedbackBuffers = 4;
		Resources.maxTransformFeedbackInterleavedComponents = 64;
		Resources.maxCullDistances = 8;
		Resources.maxCombinedClipAndCullDistances = 8;
		Resources.maxSamples = 4;
		Resources.limits.nonInductiveForLoops = 1;
		Resources.limits.whileLoops = 1;
		Resources.limits.doWhileLoops = 1;
		Resources.limits.generalUniformIndexing = 1;
		Resources.limits.generalAttributeMatrixVectorIndexing = 1;
		Resources.limits.generalVaryingIndexing = 1;
		Resources.limits.generalSamplerIndexing = 1;
		Resources.limits.generalVariableIndexing = 1;
		Resources.limits.generalConstantMatrixVectorIndexing = 1;
	}
	static bool GLSLtoSPV(const VkShaderStageFlagBits shader_type, const char* pshader, std::vector<unsigned int>& spirv)
	{
		EShLanguage stage = FindLanguage(shader_type);
		glslang::TShader shader(stage);
		glslang::TProgram program;
		const char* shaderStrings[1];
		TBuiltInResource Resources;
		init_resources(Resources);

		// Enable SPIR-V and Vulkan rules when parsing GLSL
		EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

		shaderStrings[0] = pshader;
		shader.setStrings(shaderStrings, 1);

		if (!shader.parse(&Resources, 100, false, messages)) {
			QString str_1 = QString(shader.getInfoLog());
			//ERROR: 0:8: '':syntax error, unexpected RIGHT_BRACE, expecting COMMA or SEMICOLONERROR: 1 compilation errors.No code generated.
			QString str_2 = QString(shader.getInfoDebugLog());
			return false;  // something didn't work
		}
		program.addShader(&shader);

		//
		// Program-level processing...
		//

		if (!program.link(messages)) {
			puts(shader.getInfoLog());
			puts(shader.getInfoDebugLog());
			fflush(stdout);
			return false;
		}

		glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);
		return true;
	}
	static string loadAssetStr(string fname)
	{
		ifstream infile;
		infile.open(fname.data());   //将文件流对象与文件连接起来 

		assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 
		string ss;
		string s;
		while (getline(infile, s))
		{
			ss += s;
			ss += '\n';
		}
		infile.close();             //关闭文件输入流 
		return ss;
	}
public:
	static void Init()
	{
		glslang::InitializeProcess();
	}
	static VkShaderModule CompilerVertexShader(const char * path)
	{
		vector<unsigned int> vtx_spv;//将顶点着色器脚本编译为SPV
		VkShaderModule module;

		bool retVal = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, loadAssetStr(path).c_str(), vtx_spv);
		VkAssert(retVal == true,"CompilerVertexShader GLSLtoSPV");//检查编译是否成功

		VkShaderModuleCreateInfo shaderCreateInfo;

		shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderCreateInfo.pNext = NULL;//自定义数据的指针
		shaderCreateInfo.flags = 0;//供将来使用的标志
		shaderCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);//顶点着色器SPV数据总字节数
		shaderCreateInfo.pCode = vtx_spv.data();//顶点着色器SPV数据

		VkResult result = vkCreateShaderModule(vkDevice, &shaderCreateInfo, NULL, &module);//创建顶点着色器模块
		VkAssert(result == VK_SUCCESS, "CompilerVertexShader vkCreateShaderModule");

		return module;
	}
	static VkShaderModule CompilerFragmentShader(const char* path)
	{
		vector<unsigned int> frag_spv;//将顶点着色器脚本编译为SPV
		VkShaderModule module;

		bool retVal = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, loadAssetStr(path).c_str(), frag_spv);
		VkAssert(retVal == true, "CompilerFragmentShader GLSLtoSPV");//检查编译是否成功


		VkShaderModuleCreateInfo shaderCreateInfo;

		shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderCreateInfo.pNext = NULL;//自定义数据的指针
		shaderCreateInfo.flags = 0;//供将来使用的标志
		shaderCreateInfo.codeSize = frag_spv.size() * sizeof(unsigned int);//顶点着色器SPV数据总字节数
		shaderCreateInfo.pCode = frag_spv.data();//顶点着色器SPV数据

		VkResult result = vkCreateShaderModule(vkDevice, &shaderCreateInfo, NULL, &module);//创建顶点着色器模块
		VkAssert(result == VK_SUCCESS, "CompilerFragmentShader vkCreateShaderModule");

		return module;
	}
};

