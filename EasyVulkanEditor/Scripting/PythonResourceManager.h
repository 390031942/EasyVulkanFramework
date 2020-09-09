#pragma once
#include <map>
#include <string>
#include <vector>
#include "PythonModuleInfo.h"
#include "PythonScriptComponent.h"
#include "../EasyObject.h"
#include <QVector>

class PythonResourceManager:public EasyObject
{
	_Singleton_declare_(PythonResourceManager);
	_uuid_declare_;
private:
	std::map<std::string, PythonModuleInfo*> pyModules;
	std::map<std::string, std::vector<PythonScriptComponent*>> pyModuleHandlesMap;
	std::map<std::string, std::vector<PythonScriptComponent*>>::iterator pyModuleHandlesMapIter;
	std::map<std::string , PythonScriptComponent* > pyModuleHandlesUIDMap;
	
public:
	std::vector<PythonModuleInfo*> getModuleInfos()
	{
		std::vector<PythonModuleInfo*> result;
		for (auto iter = pyModules.end(); iter != pyModules.end(); iter++)
			result.push_back((*iter).second);
		return result;
	}

	bool containModules(std::string name)
	{
		return pyModules.find(name)!= pyModules.end();
;;	}
	PythonScriptComponent* instantiate(std::string moduleName)
	{
		PythonScriptComponent* component = pyModules.find(moduleName) != pyModules.cend() ? new PythonScriptComponent(pyModules[moduleName]) : NULL;
		if (component)
		{
			pyModuleHandlesMap[moduleName].push_back(component);
			pyModuleHandlesUIDMap[component->uid] = component;
		}
		return component;
	}
	void addModule(std::string name, PythonModuleInfo* info)
	{
		info->initModule();
		pyModules["name"] = info;
	}
	int getModuleCount()
	{
		return pyModuleHandlesMap.size();
	}
	void removeComponent(std::string uid)
	{

	}
	std::vector<PythonScriptComponent*> next()
	{
		auto result = (*pyModuleHandlesMapIter++).second;
		if (pyModuleHandlesMapIter == pyModuleHandlesMap.end())
			pyModuleHandlesMapIter = pyModuleHandlesMap.begin();
		return result;
	}

};

