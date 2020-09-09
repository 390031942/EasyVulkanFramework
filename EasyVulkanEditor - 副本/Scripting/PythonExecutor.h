#pragma once
#include "PythonScriptComponent.h"
#include "PythonModuleManager.h"
#include <queue>
#include <thread>
#include <boost/python.hpp>

#define PREVIOUS_STAGE bool

struct PythonEvent
{
	PythonScriptComponent* pyComponent;
	std::string            eventName;
};
class PythonExecutor
{
private:
	static std::thread             mainThread;
	static std::queue<PythonEvent> eventQueue;

	static std::map<int, PREVIOUS_STAGE> enabledStageMap;
	static void run();

	static int  count;

	static void doExecuteStart()
	{
		for (int i = 0; i < count; i++)
		{
			auto pyComs = PythonModuleManager::getNextPythonComponents();

			for (int j = 0; j < pyComs.size(); j++)
			{
				pyComs.at(j)->start();
			}
		}
	}
	static void doEnableStageInit()
	{
		for (int i = 0; i < count; i++)
		{
			auto pyComs = PythonModuleManager::getNextPythonComponents();

			for (int j = 0; j < pyComs.size(); j++)
			{
				auto com = pyComs.at(i);
				int uid = com->uid;

				bool init_stage = com->getProperty<bool>("enabled");
				enabledStageMap[uid] = init_stage;
				com->onEnabledStageChanged(init_stage);
			}
		}
	}

	static void doEnabledStageCheck()
	{
		for (int i = 0; i < count; i++)
		{
			auto pyComs = PythonModuleManager::getNextPythonComponents();

			for (int j = 0; j < pyComs.size(); j++)
			{
				auto com = pyComs.at(i);
				int uid = com->uid;
				bool current_stage = com->getProperty<bool>("enabled");
				if(enabledStageMap[uid] != current_stage)
				{
					com->onEnabledStageChanged(current_stage);
					enabledStageMap[uid] = current_stage;
				}
			}
		}
	}
	static void doDestory()
	{
		int uid = -1;
		boost::python::object module = boost::python::object(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("__main__"))));
		boost::python::object main_namespace = module.attr("__dict__");
		while (true)
		{

			boost::python::exec("uid = EasyVulkan.ComponentDestoryQueue.get_instance().pop()", main_namespace, main_namespace);
			int uid = boost::python::extract<int>(main_namespace["uid"]);
			if (uid != -1)
			{
				PythonModuleManager::removeComponent(uid);
			}
			else
			{
				break;
			}
		}
	}
	static void doExecuteUpdate()
	{
		for (int i = 0; i < count; i++)
		{
			auto pyComs = PythonModuleManager::getNextPythonComponents();

			for (int j = 0; j < pyComs.size(); j++)
			{
				bool current_stage = pyComs.at(j)->getProperty<bool>("enabled");
				if(current_stage)
					pyComs.at(j)->update();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
public:
	static void init();

	static void shutDown();
};

