#include "PythonEditorExecutor.h"
_Singleton_impl_(PythonEditorExecutor);
_uuid_impl_(PythonEditorExecutor,"d79c4aba-4c52-4a29-a051-865f96222a39")

void PythonEditorExecutor::invokeMenuAction(std::string action)
{
	try
	{
		auto lock = PyGILState_Ensure();
		boost::python::dict menuItemMap = boost::python::extract<boost::python::dict>(PythonBuildInManager::getInstance()->getEditorMenuDict()["MenuItemMap"]);
		boost::python::object slot = menuItemMap[boost::python::str(action.c_str())];
		slot();
		PyGILState_Release(lock);
	}
	catch (boost::python::error_already_set)
	{
		getLastErrorMessageBox();
	}
}

std::vector<std::string> PythonEditorExecutor::getMenuActionTexts()
{
	menuItemDict = PythonBuildInManager::getInstance()->getEditorMenuDict();
	boost::python::dict menuItemMap = boost::python::extract<boost::python::dict>(menuItemDict["MenuItemMap"]);

	auto keys = menuItemMap.keys();

	std::vector<std::string> result;

	for (int i = 0; i < len(keys); i++)
	{
		result.push_back(boost::python::extract<std::string>(keys[i]));
	}
	return result;
}

bool PythonEditorExecutor::init()
{
	Py_Initialize();
	if (!Py_IsInitialized())
		return false;
	PyEval_InitThreads();
	boost::python::object main_module((boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("__main__")))));
	boost::python::object main_namespace = main_module.attr("__dict__");

	PyRun_SimpleString("import sys");

	PyRun_SimpleString(QString("sys.path.append('%1')").arg(Default_Build_In_Path).toStdString().c_str());
	PyRun_SimpleString(QString("sys.path.append('%1')").arg(Defalut_Script_Path).toStdString().c_str());

	try
	{
		PythonBuildInManager::getInstance()->init();
	}
	catch (boost::python::error_already_set)
	{
		getLastErrorMessageBox();
	}


	this->main_module = &main_module;
	this->main_namespace = &main_namespace;
	PythonScanner::getInstance()->doScan();

	auto pyScripts = PythonScanner::getInstance()->getPythonScripts();

	for (auto pyScript : pyScripts)
	{
		auto result = PythonScriptAnalyzer::getInstance()->analysis(pyScript);
		if (result == PythonScriptAnalyzer::Editor_Menu)
		{
			try
			{
				std::vector<std::string> actionNames = PythonBuildInManager::getInstance()->getLastMenuActionPaths();
				EditorMenuManager::getInstance()->addActions(actionNames);
			}
			catch (boost::python::error_already_set)
			{
				getLastErrorMessageBox();
			}
		}
	}
	return true;
}
