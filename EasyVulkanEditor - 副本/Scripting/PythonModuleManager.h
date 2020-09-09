#pragma once

#include <QDir>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <boost/python.hpp>
#include <Python.h>
#include <string>

#include "PythonScriptComponent.h"
#include "PythonModuleInfo.h"

#include <QFileInfo>
class PythonModuleManager
{
private:
	static std::vector<QString> pyFiles;
	static std::map<QString,QString> pyFilesMD5;
	static std::map<std::string, int> internalMethodArgCountMap;
	static std::map<std::string, PythonModuleInfo> pyModules;
	static std::map<std::string, std::vector<PythonScriptComponent*>> pyModuleHandlesMap;
	static std::map<std::string, std::vector<PythonScriptComponent*>>::iterator pyModuleHandlesMapIter;
	static std::map<int,PythonScriptComponent*> pyModuleHandlesUIDMap;
	static bool serachScript(QString& findPath, QString format = "*.py");

	static void calculateMD5(QString filePath);

	static int  getInternalMethodArgCount(std::string methodName);

	static void getPythonFileInfo(std::string filePath);

	static bool IsInternalMethod(std::string methodName);

public:
	static void run();

	static void init();

	static PythonScriptComponent* instantiate(std::string moduleName);

	static int getModuleCount();

	static std::vector<PythonScriptComponent*> getNextPythonComponents();

	static void removeComponent(int uid)
	{
		auto iter = pyModuleHandlesUIDMap.find(uid);

		PythonScriptComponent* com = (*iter).second;
		com->onDestory();

		auto coms = pyModuleHandlesMap[com->name];

		auto _iter = coms.begin();
		for (; _iter != coms.end(); _iter++)
		{
			if (*_iter == com)
				break;
		}
		coms.erase(_iter);
		pyModuleHandlesUIDMap.erase(uid);

		delete com;
		com = NULL;
	}

	
};

	