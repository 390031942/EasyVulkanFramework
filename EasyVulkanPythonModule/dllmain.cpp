#include "pch.h"
#include <Python.h>
#include <boost/python.hpp>
#include <string>
#include <queue>
#include <mutex>

using namespace std;
using namespace boost::python;

template<typename T>
static boost::shared_ptr<T> get_shared_ptr()
{
	return new T;
}

#define _Singleton_declare_(cls)  public:static cls* getInstance();static cls* __instance; static QMutex __mutex;

#define _Singleton_impl_(cls)     cls* cls::__instance = 0;                      \
                                  QMutex cls::__mutex; cls* cls::getInstance() \
                                  {                                                              \
                                      if (!__instance)                    \
                                      {                                                          \
                                          QMutexLocker locker(&__mutex);                         \
                                          if(!__instance)                                        \
                                          {                                                      \
                                                 cls* instance = new cls;                        \
                                                __instance = instance;                            \
                                          }                                                      \
                                      }                                                          \
                                      return __instance;                                         \
                                  }

#define _uuid_declare_         std::string getUUID();
#define _uuid_impl_(cls,uuid)  std::string cls::getUUID(){ return uuid;}

class EasyObject
{
public:
	virtual std::string getUUID() = 0;
	virtual std::string toString();
};

std::string EasyObject::toString()
{
	return std::string("class EasyObject at") + std::to_string(reinterpret_cast<long>(this));
}

class EditorUtility :public EasyObject
{
	_Singleton_declare_(EditorUtility);
	_uuid_declare_;
public:
	void quit();
	void messageBox(std::string title, std::string msg);
};

_Singleton_impl_(EditorUtility);
_uuid_impl_(EditorUtility, "e61511cb-0aed-4b23-be8d-e9717f76f5c4");

void EditorUtility::quit()
{
	exit(0);
}
void EditorUtility::messageBox(std::string title, std::string msg)
{
	MessageBox(NULL, string2wstring(msg).c_str(), string2wstring(title).c_str(), MB_OK);
}

void EditorUtility::quit()
{
	exit(0);
}

static wstring string2wstring(string str)
{
	wstring result;
	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//多字节编码转换成宽字节编码  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //添加字符串结尾  
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}
void EditorUtility::messageBox(std::string title, std::string msg)
{
	MessageBox(NULL, string2wstring(msg).c_str(), string2wstring(title).c_str(),MB_OK);
}

/*BOOST_PYTHON_MODULE(EasyVulkan)
{
	class_<ComponentDestoryQueue>("ComponentDestoryQueue")
		.def("get_instance", &ComponentDestoryQueue::get_instance)
		.staticmethod("get_instance")
		.def("get_string", &ComponentDestoryQueue::get_string)
		.staticmethod("get_string")
		.def("pop", &ComponentDestoryQueue::pop);

	class_<Component>("Component")
		.add_property("enabled", &Component::getEnabled, &Component::setEnabled)
		.add_property("name", &Component::getName, &Component::setName)
		.def("selfDestory", &Component::selfDestory)
	    .def("getUID", &Component::getUID);

	register_ptr_to_python<Component_PTR>();
	register_ptr_to_python<ComponentDestoryQueue_PTR>();
}*/

typedef boost::shared_ptr<EditorUtility> EditorUtility_PTR;

BOOST_PYTHON_MODULE(EasyVulkanEditor)
{
	class_<EditorUtility>("EditorUtility")
		.def("quit_Internal", &EditorUtility::quit)
	    .def("alert_Internal", &EditorUtility::messageBox);

	register_ptr_to_python<EditorUtility_PTR>();
	
}