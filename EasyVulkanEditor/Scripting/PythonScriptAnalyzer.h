#pragma once
#include <boost/python.hpp>
#include <python.h>
#include <QFileInfo>
#include <string>
#include "PythonBuildInManager.h"
#include "PythonResourceManager.h"
#include "../EasyObject.h"
#include <frameObject.h>
#include <iostream>
#include <fstream>

class PythonScriptAnalyzer
{
	_Singleton_declare_(PythonScriptAnalyzer);
	_uuid_declare_;

	enum PyResult {Editor_Menu ,Editor_PropertyView,Editor_Window,BehaviorComponent,ClassNotFound,ModuleError,Normal};
public:
	_NODISCARD PyResult analysis(std::string filePath);
	void analysisUncheckedProperty(PythonModuleInfo*& info);
	std::string lastError;
	std::string getLastError();
	std::string getPythonClassName(std::string filePath);
};

