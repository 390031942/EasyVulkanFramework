#include "EasyVulkanConsole.h"

VkInstance EasyVulkanConsole::vkInstance = VK_NULL_HANDLE;
VkDebugReportCallbackEXT  EasyVulkanConsole::vkDebugReportCallback;//调试报告回调
HANDLE    EasyVulkanConsole::m_Handle_Console = NULL;
std::vector<VkLayerProperties> EasyVulkanConsole::layerProperties;
void EasyVulkanConsole::OutputToConsole(const wchar_t* str)
{
	if (!WriteConsole(m_Handle_Console, str, wcslen(str), NULL, NULL))
		msgbox_err("\nCan not output to console");
}

VKAPI_ATTR VkBool32 VKAPI_CALL EasyVulkanConsole::debugFunction(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char* layerPrefix, const char* msg, void* pUserData)
{
	if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {	//错误信息
		LogError(QString("[VK_DEBUG_REPORT] ERROR: %1").arg(msg));
	}
	else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {//警告信息
		LogWarning(QString("[VK_DEBUG_REPORT] WARNING: %1").arg(msg));
	}
	else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {//消息信息
		Log(QString("[VK_DEBUG_REPORT] INFORMATION: %1").arg(msg));
	}
	else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {//性能警告信息
		Log(QString("[VK_DEBUG_REPORT] PERFORMANCE: %1").arg(msg));
	}
	else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {//调试信息
		Log(QString("[VK_DEBUG_REPORT] DEBUG: %1").arg(msg));
	}
	else {
		return VK_FALSE;//其他未知情况
	}
	return VK_SUCCESS;
}

void EasyVulkanConsole::Init()
{
	AllocConsole(); //Try to allocate a console
	m_Handle_Console = GetStdHandle(STD_OUTPUT_HANDLE);//Get the Output Console Handle
	VkAssert(m_Handle_Console != NULL, "EasyVulkanConsole::Init()");
}

void EasyVulkanConsole::Log(const char* text, bool RecordInfo, OUTPUT_TARGET target)
{
	SetConsoleTextAttribute(m_Handle_Console, fg_white);
	OutputToConsole(tchar(str_log_content + text + "\n"));
}

void EasyVulkanConsole::Log(const QString text, bool RecordInfo, OUTPUT_TARGET target)
{
	SetConsoleTextAttribute(m_Handle_Console, fg_white);
	OutputToConsole(tchar((RecordInfo ? str_log_content : "") + text + "\n"));
}

void EasyVulkanConsole::LogWarning(const QString text, OUTPUT_TARGET target)
{
	SetConsoleTextAttribute(m_Handle_Console, fg_intensity);
	OutputToConsole(tchar(str_log_content + text + "\n"));
}

void EasyVulkanConsole::LogError(const QString text, OUTPUT_TARGET target)
{
	SetConsoleTextAttribute(m_Handle_Console, fg_red);
	OutputToConsole(tchar(str_log_content + text + "\n"));
}

void EasyVulkanConsole::SetConsoleWindowTitle(const QString text)
{
	SetConsoleTitle(text.toStdWString().c_str());
}
void EasyVulkanConsole::CreateDebugReportCallbackSelf() { //创建调试报告回调相关
	VkDebugReportCallbackCreateInfoEXT dbgReportCreateInfo = {};//构建调试报告回调创建用信息结构体实例

	dbgReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	dbgReportCreateInfo.pfnCallback = debugFunction;//指定回调方法
	dbgReportCreateInfo.pUserData = NULL;//传递给回调的用户自定义数据
	dbgReportCreateInfo.pNext = NULL;//指向自定义数据的指针
	dbgReportCreateInfo.flags = //所需的触发消息回调的事件类型
		VK_DEBUG_REPORT_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_ERROR_BIT_EXT |
		VK_DEBUG_REPORT_DEBUG_BIT_EXT;
	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT_Function = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugReportCallbackEXT");
	VkResult  result = vkCreateDebugReportCallbackEXT_Function(vkInstance, &dbgReportCreateInfo, NULL, &vkDebugReportCallback);//创建调试报告回调实例
	if (result == VK_SUCCESS) {
		Log("调试报告回调对象创建成功！\n");
	}
}
void EasyVulkanConsole::DestroyDebugReportCallbackSelf()//销毁调试报告回调相关
{
	PFN_vkDestroyDebugReportCallbackEXT vkDestoryDebugReportCallbackEXT_Function = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugReportCallbackEXT");
	vkDestoryDebugReportCallbackEXT_Function(vkInstance, vkDebugReportCallback, NULL);
}
