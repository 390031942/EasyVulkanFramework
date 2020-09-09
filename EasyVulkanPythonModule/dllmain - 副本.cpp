#include "pch.h"
#include <Python.h>
#include <boost/python.hpp>
#include <string>
#include <queue>
#include <mutex>

using namespace std;
using namespace boost::python;

class Component;

class ComponentDestoryQueue
{
private:
	std::queue<int> destroyQueue;
public:
	void push(int uid)
	{
		destroyQueue.push(uid);
	}
	int pop()
	{
		//这里要改成pop
		int result = destroyQueue.empty() ? -1 : destroyQueue.front();

		if (result != -1) destroyQueue.pop();
		return result;
	}
	typedef boost::shared_ptr<ComponentDestoryQueue> Ptr;

	static Ptr get_instance() {

		// "double checked lock"
		if (m_instance_ptr == nullptr) {
			std::lock_guard<std::mutex> lk(m_mutex);
			if (m_instance_ptr == nullptr) {
				m_instance_ptr = boost::shared_ptr<ComponentDestoryQueue>(new ComponentDestoryQueue);
			}
		}
		return m_instance_ptr;
	}
	ComponentDestoryQueue()
	{
		str = std::to_string(rand());
	}
	static std::string get_string()
	{
		return str;
	}

	static std::string str;
	static Ptr m_instance_ptr;
	static std::mutex m_mutex;
};


ComponentDestoryQueue::Ptr ComponentDestoryQueue::m_instance_ptr;
std::mutex ComponentDestoryQueue::m_mutex;
std::string ComponentDestoryQueue::str;


class Component
{
private:
	int         uid;
public:
	bool        enabled;
	std::string name;

	Component()
	{
		srand((unsigned int)time(NULL));
		uid = rand() * rand() % 65535;
		enabled = true;
	}
	int  getUID()
	{
		return uid;
	}
	void setEnabled(bool enabled)
	{
		this->enabled = enabled;
	}
	bool getEnabled()
	{
		return this->enabled;
	}
	
	void setName(std::string name)
	{
		this->name = name;
	}

	std::string getName()
	{
		return this->name;
	}
	void selfDestory()
	{
		ComponentDestoryQueue::get_instance()->push(this->getUID());
	}
};
typedef boost::shared_ptr<ComponentDestoryQueue> ComponentDestoryQueue_PTR;
typedef boost::shared_ptr<Component> Component_PTR;

class EditorUtility :public EasyObject
{
	_Singleton_declare_(EditorUtility);
	_uuid_declare_;
public:
	void quit();
	void messageBox(std::string title, std::string msg);
};
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

typedef boost::shared_ptr<EasyVulkanEditorUtility> EasyVulkanEditorUtility_PTR;

BOOST_PYTHON_MODULE(EasyVulkanEditor)
{
	class_<EasyVulkanEditorUtility>("EditorUtility")
		.def("quit_Internal", &EasyVulkanEditorUtility::quit)
	    .def("alert_Internal", &EasyVulkanEditorUtility::messageBox);

	register_ptr_to_python<EasyVulkanEditorUtility_PTR>();
	
}