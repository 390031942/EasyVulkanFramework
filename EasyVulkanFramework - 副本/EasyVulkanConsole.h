#pragma once
#include <QString>
#include <QTime>
#include <QMessageBox>
#include <vector>
#include <vulkan/vulkan.h>

#include "EasyVulkanAssert.h"

using namespace std;

#define Debug DebugSystem::getInstance()

#define OUTPUT_TARGET         int

#define OUTPUT_TARGET_CONSOLE 1
#define OUTPUT_TARGET_FILE    2

#define DOS_CONSOLE_MODE   

#ifdef  DOS_CONSOLE_MODE
#include <windows.h>
#endif

#define str_normal_title  ""
#define str_error_title  QString("Fatal Error!")
#define str_log_content          \
               QString("[%1 %2]:") \
               .arg(__DATE__)     \
               .arg(QTime::currentTime().toString())     \

#define str_logerror_content          \
               QString("[%1 %2]:File:%3 %4() at Line %5:") \
               .arg(__DATE__)     \
               .arg(QTime::currentTime().toString())     \
			   .arg(QString(__FILE__).split("\\").constLast())     \
               .arg(__FUNCTION__) \
               .arg(__LINE__) 
#define msgbox_err(text)          \
               QMessageBox::warning(0,str_error_title \
			   , str_logerror_content+text)

#define tchar(text)               \
              reinterpret_cast<const wchar_t *>(QString(text).utf16())

#define Log_Normal  31
#define Log_Warning 32
#define Log_Error   33

#define fg_white     FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN
#define fg_blue      FOREGROUND_BLUE
#define fg_red       FOREGROUND_RED
#define fg_green     FOREGROUND_GREEN
#define fg_intensity FOREGROUND_INTENSITY



typedef struct VkDeviceLayerAndExtensionType {
	std::vector<std::string*> layerNames;//支持的验证Layer名称列表
	std::vector<std::string*> extensionNames;//支持的验证Layer所需扩展的名称列表
}VkDeviceLayerAndExtension;

class EasyVulkanConsole
{
private:
	static VkInstance vkInstance;
	static VkDebugReportCallbackEXT vkDebugReportCallback;//调试报告回调
	static HANDLE    m_Handle_Console;
	static void      OutputToConsole(const wchar_t*);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(//用于被回调以打印验证信息的方法
	    VkFlags msgFlags,						//触发此回调执行的调试事件类型标志
		VkDebugReportObjectTypeEXT objType,		//由此回调处理的对象类型
		uint64_t srcObject,						//此回调创建或处理的对象的句柄
		size_t location,						//描述对应调试事件代码的位置
		int32_t msgCode,						//消息代码
		const char* layerPrefix,				//触发此回调的验证Layer
		const char* msg,						//消息字符串
		void* pUserData						//用户自定义数据
	);
	static std::vector<VkLayerProperties> layerProperties;
	static bool isContain(std::vector<const char*> inNames, char* inName) {//判断字符串是否在列表中的方法
		for (auto s : inNames) {//遍历字符串列表
			if (strcmp(s, inName) == 0) {//若给定字符串与当前字符串相同
				return true;//返回true，表示指定字符串在列表中
			}
		}
		return false;//返回false，表示指定字符串不在列表中
	}
	static bool isContain(std::vector<std::string*> inNames, char* inName) {//判断字符串是否在列表中的方法
		for (auto s : inNames) {//遍历字符串列表
			if (strcmp((*s).c_str(), inName) == 0) {//若给定字符串与当前字符串相同
				return true;//返回true，表示指定字符串在列表中
			}
		}
		return false;//返回false，表示指定字符串不在列表中
	}
public:
	static void  Init();
	static void  Log(const char* text, bool RecordInfo = true, OUTPUT_TARGET target = OUTPUT_TARGET_CONSOLE);
	static void  Log(const QString text, bool RecordInfo = true, OUTPUT_TARGET target = OUTPUT_TARGET_CONSOLE);
	static void  LogWarning(const QString text, OUTPUT_TARGET target = OUTPUT_TARGET_CONSOLE);
	static void  LogError(const QString text, OUTPUT_TARGET target = OUTPUT_TARGET_CONSOLE);
	static void  SetConsoleWindowTitle(const QString text);
	static void  CreateDebugReportCallbackSelf();
	static void  DestroyDebugReportCallbackSelf();
	static VkDeviceLayerAndExtension GetLayerProperties(std::vector<const char*> exceptedLayerNames) {
		VkDeviceLayerAndExtension result;//返回结果结构体实例
		uint32_t layerCount;//总的验证Layer的数量
		vkEnumerateInstanceLayerProperties(&layerCount, NULL);//获取总的验证Layer数量
		//LOGE("Layer的数量为 %d\n", layerCount);//打印总的验证Layer数量
		layerProperties.resize(layerCount);//更改列表长度
		vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());//获取总的验证Layer属性列表
		for (int i = 0; i < layerProperties.size(); i++) {//遍历验证Layer属性列表
			VkLayerProperties lp = layerProperties[i];//获取当前验证Layer属性
			bool flag = isContain(exceptedLayerNames, lp.layerName);//当前验证Layer是否需要
			if (flag) {//若需要，则将当前验证Layer名称记录到验证Layer名称结果列表
				result.layerNames.push_back(new std::string(lp.layerName));
			}
			uint32_t propertyCount;//此验证Layer对应的扩展属性数量
			vkEnumerateInstanceExtensionProperties(lp.layerName, &propertyCount, NULL);
			std::vector<VkExtensionProperties> propertiesList;//扩展属性列表
			propertiesList.resize(propertyCount);//调整列表长度
			vkEnumerateInstanceExtensionProperties(lp.layerName, &propertyCount, propertiesList.data());
			for (auto ep : propertiesList) {//遍历此验证Layer对应的扩展属性列表
				if (flag) {	//若当前验证Layer是需要的
					if (!isContain(result.extensionNames, ep.extensionName)) {
						result.extensionNames.push_back(new std::string(ep.extensionName));
					}
				}
			}
		}
		return result;//返回结果
	}
	friend class EasyVulkanRenderer;
};

