#pragma once
#include <string>
#include <queue>
#include <boost/shared_ptr.hpp>
#include <mutex>
class Component
{
public:
	bool        enabled;
	std::string name;
	int         uid;
};
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


typedef boost::shared_ptr<ComponentDestoryQueue> ComponentDestoryQueue_PTR;
typedef boost::shared_ptr<Component> Component_PTR;