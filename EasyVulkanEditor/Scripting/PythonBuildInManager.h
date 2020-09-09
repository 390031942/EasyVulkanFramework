#pragma once

#include <string>
#include <boost/python.hpp>

#include "../EditorUtility.h"

#include "../EasyObject.h"

#define _module_declare_(module) 	\
                boost::python::object module_##module = boost::python::import(#module);  \
                boost::python::object dict_##module = module_##module.attr("__dict__");\
                public:  boost::python::object get##module##Dict() {return this->dict_##module ;}

#define _inject_ptr_to_python_(module,value_name,value_ptr) dict_##module[value_name] = value_ptr

#define _get_module_dict_(module) dict_##module

#define Default_Build_In_Path  QString(QCoreApplication::applicationDirPath()+"/build-In")

class PythonBuildInManager:public EasyObject
{
	_Singleton_declare_(PythonBuildInManager);
	_uuid_declare_

private:
	/*Module*/
	_module_declare_(EditorMenu);

	/*Internal Method*/
	std::map<std::string, int> internalMethodsMap;

	int lastMenuActionIndex;
private:
	void initInternalMethodMap()
	{
		internalMethodsMap["Start"] = 0;
		internalMethodsMap["Update"] = 0;
		internalMethodsMap["OnDestory"] = 0;
		internalMethodsMap["OnEnabledStageChanged"] = 0;
	}
public:
	std::vector<std::string> getLastMenuActionPaths()
	{
		std::vector<std::string> result;
		boost::python::object menuItemDict = getEditorMenuDict();
		boost::python::dict menuItemMap = boost::python::extract<boost::python::dict>(menuItemDict["MenuItemMap"]);

		int lastIndex = len(menuItemMap.keys());
		
		for (int i = lastMenuActionIndex; i < lastIndex;i++)
		{
			boost::python::object key = menuItemMap.keys()[i];
			result.push_back(boost::python::extract <std::string>(key));
		}
		return result;
	}

	void init()
	{
		auto ptr = EditorUtility::getInstance();
		_inject_ptr_to_python_(EditorMenu, "editorUtility", ptr);

		lastMenuActionIndex = 0;
	}
	void destory()
	{

	}
	bool isEditorMenuScript(std::string type)
	{
		return type == "EditorMenu";
	}
	bool isEditorPropertyViewScript(std::string type)
	{
		return type == "EditorPropertyViewer" ;
	}

	bool isEditorWindowScript(std::string type)
	{
		return type == "EditorWindow";
	}

	bool isComponentScript(std::string type)
	{
		return type == "BehaviorComponent";
	}

	bool isBuildInModule(std::string module)
	{
		return module == "GameObject";
	}
	std::string getMenuItemTexts()
	{
		
	}
public:

	bool isInternalMethod(std::string methodName)
	{
		for(auto pair: internalMethodsMap)
		{
			if (pair.first == methodName)
				return true;
		}
		return false;
	}
	int getIntenalMethodArgCount(std::string methodName)
	{
		return internalMethodsMap[methodName];
	}
};

