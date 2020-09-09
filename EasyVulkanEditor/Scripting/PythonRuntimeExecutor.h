#pragma once

#include "PythonComponentManager.h"
class PythonRuntimeExecutor
{
	static std::map<std::string, bool> enabledStageMap;
	void doExecuteStart()
	{
		int count = PythonComponentManager::getInstance()->getModuleCount();
		for (int i = 0; i < count; i++)
		{
			auto pyComs = PythonComponentManager::getInstance()->next();

			for (int j = 0; j < pyComs.size(); j++)
			{
				pyComs.at(j)->start();
			}
		}
	}
	void doEnableStageInit()
	{
		int count = PythonComponentManager::getInstance()->getModuleCount();
		for (int i = 0; i < count; i++)
		{
			auto pyComs = PythonComponentManager::getInstance()->next();

			for (int j = 0; j < pyComs.size(); j++)
			{
				auto com = pyComs.at(i);
				std::string uid = com->uid;

				bool init_stage = com->getProperty<bool>("enabled");
				enabledStageMap[uid] = init_stage;
				com->onEnabledStageChanged(init_stage);
			}
		}
	}

	void doEnabledStageCheck()
	{
		int count = PythonComponentManager::getInstance()->getModuleCount();
		for (int i = 0; i < count; i++)
		{
			auto pyComs = PythonComponentManager::getInstance()->next();

			for (int j = 0; j < pyComs.size(); j++)
			{
				auto com = pyComs.at(i);
				std::string uid = com->uid;
				bool current_stage = com->getProperty<bool>("enabled");
				if (enabledStageMap[uid] != current_stage)
				{
					com->onEnabledStageChanged(current_stage);
					enabledStageMap[uid] = current_stage;
				}
			}
		}
	}
	void doDestory()
	{
		int uid = -1;
		boost::python::object module = boost::python::object(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("__main__"))));
		boost::python::object main_namespace = module.attr("__dict__");
		while (true)
		{

			boost::python::exec("uid = EasyVulkan.ComponentDestoryQueue.get_instance().pop()", main_namespace, main_namespace);
			std::string uid = boost::python::extract<std::string>(main_namespace["uid"]);
			if (uid != "NULL")
			{
				PythonComponentManager::getInstance()->removeComponent(uid);
			}
			else
			{
				break;
			}
		}
	}
	void doExecuteUpdate()
	{
		int count = PythonComponentManager::getInstance()->getModuleCount();

		for (int i = 0; i < count; i++)
		{
			auto pyComs = PythonComponentManager::getInstance()->next();

			for (int j = 0; j < pyComs.size(); j++)
			{
				bool current_stage = pyComs.at(j)->getProperty<bool>("enabled");
				if (current_stage)
					pyComs.at(j)->update();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
};

