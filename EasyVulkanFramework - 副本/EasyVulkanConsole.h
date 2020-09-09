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
	std::vector<std::string*> layerNames;//֧�ֵ���֤Layer�����б�
	std::vector<std::string*> extensionNames;//֧�ֵ���֤Layer������չ�������б�
}VkDeviceLayerAndExtension;

class EasyVulkanConsole
{
private:
	static VkInstance vkInstance;
	static VkDebugReportCallbackEXT vkDebugReportCallback;//���Ա���ص�
	static HANDLE    m_Handle_Console;
	static void      OutputToConsole(const wchar_t*);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(//���ڱ��ص��Դ�ӡ��֤��Ϣ�ķ���
	    VkFlags msgFlags,						//�����˻ص�ִ�еĵ����¼����ͱ�־
		VkDebugReportObjectTypeEXT objType,		//�ɴ˻ص�����Ķ�������
		uint64_t srcObject,						//�˻ص���������Ķ���ľ��
		size_t location,						//������Ӧ�����¼������λ��
		int32_t msgCode,						//��Ϣ����
		const char* layerPrefix,				//�����˻ص�����֤Layer
		const char* msg,						//��Ϣ�ַ���
		void* pUserData						//�û��Զ�������
	);
	static std::vector<VkLayerProperties> layerProperties;
	static bool isContain(std::vector<const char*> inNames, char* inName) {//�ж��ַ����Ƿ����б��еķ���
		for (auto s : inNames) {//�����ַ����б�
			if (strcmp(s, inName) == 0) {//�������ַ����뵱ǰ�ַ�����ͬ
				return true;//����true����ʾָ���ַ������б���
			}
		}
		return false;//����false����ʾָ���ַ��������б���
	}
	static bool isContain(std::vector<std::string*> inNames, char* inName) {//�ж��ַ����Ƿ����б��еķ���
		for (auto s : inNames) {//�����ַ����б�
			if (strcmp((*s).c_str(), inName) == 0) {//�������ַ����뵱ǰ�ַ�����ͬ
				return true;//����true����ʾָ���ַ������б���
			}
		}
		return false;//����false����ʾָ���ַ��������б���
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
		VkDeviceLayerAndExtension result;//���ؽ���ṹ��ʵ��
		uint32_t layerCount;//�ܵ���֤Layer������
		vkEnumerateInstanceLayerProperties(&layerCount, NULL);//��ȡ�ܵ���֤Layer����
		//LOGE("Layer������Ϊ %d\n", layerCount);//��ӡ�ܵ���֤Layer����
		layerProperties.resize(layerCount);//�����б���
		vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());//��ȡ�ܵ���֤Layer�����б�
		for (int i = 0; i < layerProperties.size(); i++) {//������֤Layer�����б�
			VkLayerProperties lp = layerProperties[i];//��ȡ��ǰ��֤Layer����
			bool flag = isContain(exceptedLayerNames, lp.layerName);//��ǰ��֤Layer�Ƿ���Ҫ
			if (flag) {//����Ҫ���򽫵�ǰ��֤Layer���Ƽ�¼����֤Layer���ƽ���б�
				result.layerNames.push_back(new std::string(lp.layerName));
			}
			uint32_t propertyCount;//����֤Layer��Ӧ����չ��������
			vkEnumerateInstanceExtensionProperties(lp.layerName, &propertyCount, NULL);
			std::vector<VkExtensionProperties> propertiesList;//��չ�����б�
			propertiesList.resize(propertyCount);//�����б���
			vkEnumerateInstanceExtensionProperties(lp.layerName, &propertyCount, propertiesList.data());
			for (auto ep : propertiesList) {//��������֤Layer��Ӧ����չ�����б�
				if (flag) {	//����ǰ��֤Layer����Ҫ��
					if (!isContain(result.extensionNames, ep.extensionName)) {
						result.extensionNames.push_back(new std::string(ep.extensionName));
					}
				}
			}
		}
		return result;//���ؽ��
	}
	friend class EasyVulkanRenderer;
};

