#include "PythonScriptAnalyzer.h"
#include "PythonModuleInfo.h"

_Singleton_impl_(PythonScriptAnalyzer);
_uuid_impl_(PythonScriptAnalyzer, "58fcceb3-1026-457d-9fdc-1c0cc15cca1d");

PythonScriptAnalyzer::PyResult PythonScriptAnalyzer::analysis(std::string filePath)
{
	std::streambuf* coutBuf = std::cout.rdbuf();

	std::ofstream of("out.txt");
	// 获取文件out.txt流缓冲区指针
	std::streambuf* fileBuf = of.rdbuf();

	// 设置cout流缓冲区指针为out.txt的流缓冲区指针
	std::cout.rdbuf(fileBuf);
	try
	{
		//First,we get the python class name,which has to be equaled with the filename;
		std::string moduleName = getPythonClassName(filePath);

		//use a temporary dict to get the variables and methods of one python's class 
		boost::python::dict moduleDict = boost::python::dict();
		exec_file(filePath.c_str(), moduleDict, moduleDict);
		//run some python statement to get information
		//Here,we should firstly judge the type of class,in order to distinguish between Editor-script and Component-script
		//inst represents a instance of class
		exec("flag = True", moduleDict, moduleDict);
		exec("inst = None", moduleDict, moduleDict);

		exec((std::string("try:inst=") + moduleName + "()\nexcept:flag = False").data(), moduleDict, moduleDict);

		bool flag = boost::python::extract<bool>(moduleDict["flag"]);

		if (!flag)
		{
			return PyResult::ClassNotFound;
		}
		exec(("instType = "+ moduleName+".__base__.__name__").data(), moduleDict, moduleDict);

		std::string instType = boost::python::extract<std::string>(moduleDict["instType"]);

		if (PythonBuildInManager::getInstance()->isEditorMenuScript(instType))
		{
			return PyResult::Editor_Menu;
		}
		if (PythonBuildInManager::getInstance()->isEditorPropertyViewScript(instType))
		{
			return PyResult::Editor_PropertyView;
		}
		if (PythonBuildInManager::getInstance()->isEditorWindowScript(instType))
		{
			return PyResult::Editor_Window;
		}
		else if (PythonBuildInManager::getInstance()->isComponentScript(instType))
		{
			//Then delcare some vars to record the infos
			exec((std::string("current_key=0\n")
				+ "current_type=0\n"
				+ "current_variable=0\n").data(), moduleDict, moduleDict);

			//display all variables
			exec((std::string("variables=[attr for attr in dir(inst) if not callable(getattr(inst, attr)) and not attr.startswith(\"__\")]\n")
				+ "length=len(variables)\n").data(), moduleDict, moduleDict);

			//convert into boost::python::list for C++ to traverse 
			boost::python::list varList = boost::python::extract<boost::python::list>(moduleDict["variables"]);

			int length = boost::python::extract<int>(moduleDict["length"]);

			PythonModuleInfo pyModule;
			pyModule.moduleName = moduleName;
			for (int i = 0; i < length; i++)
			{
				/*exec((std::string("current_key=keys[") + std::to_string(i) + "]").data(), moduleDict, moduleDict);
				std::string key = boost::python::extract<std::string>(moduleDict["current_key"]);

				exec((std::string("current_type=str(type(inst."+key+")).split(\"\'\")[1]")).data(), moduleDict, moduleDict);
				std::string type = boost::python::extract<std::string>(moduleDict["current_type"]);
				exec((std::string("current_value=values[") + std::to_string(i) + "]").data(), moduleDict, moduleDict);*/

				exec(("current_key=variables[" + std::to_string(i) + "]").data(), moduleDict, moduleDict);
				std::string key = boost::python::extract<std::string>(moduleDict["current_key"]);
				exec(("current_type=str(type(inst." + key + ")).split(\"\'\")[1]").data(), moduleDict, moduleDict);

				exec((std::string("current_value=inst.") + key).data(), moduleDict, moduleDict);
				std::string type = boost::python::extract<std::string>(moduleDict["current_type"]);
				if (type == "int")
				{
					int value = boost::python::extract<int>(moduleDict["current_value"]);
					pyModule.addProperty(key, type, &value);
				}
				else if (type == "float")
				{
					double value = boost::python::extract<double>(moduleDict["current_value"]);
					pyModule.addProperty(key, type, &value);
				}
				else if (type == "bool")
				{
					bool value = boost::python::extract<bool>(moduleDict["current_value"]);
					pyModule.addProperty(key, type, &value);
				}
				else if (type == "str")
				{
					std::string value = boost::python::extract<std::string>(moduleDict["current_value"]);
					pyModule.addProperty(key, type, &value);
				}
				else if (type == "list")
				{
					boost::python::list value = boost::python::extract<boost::python::list>(moduleDict["current_value"]);
					pyModule.addProperty(key, type, &value);
				}
				else if (type == "tuple")
				{
					boost::python::tuple value = boost::python::extract<boost::python::tuple>(moduleDict["current_value"]);
					for (int j = 0; j < len(value); j++)
					{
						//cout << extract<int>(value[j])<< endl;
					}
					pyModule.addProperty(key, type, &value);
				}
				else if (type == "dict")
				{
					boost::python::dict value = boost::python::extract<boost::python::dict>(moduleDict["current_value"]);
					for (int j = 0; j < len(value); j++)
					{
						//cout << extract<int>(value[j])<< endl;
					}
					pyModule.addProperty(key, type, &value);
				}
				else if (type == "None")
				{

				}

				else
				{
					pyModule.addUncheckedProperty(key, type);
				}
			}
			char* get_methods_str = new char[128];
			strcpy(get_methods_str, (std::string("method_list=list(filter(lambda m: not m.startswith(\"__\")"
				"and not m.endswith(\"__\") "
				"and callable(getattr(inst,m)),dir(inst)))").data()));

			exec(get_methods_str, moduleDict, moduleDict);

			boost::python::list method_list = boost::python::extract<boost::python::list>(moduleDict["method_list"]);
			int size = len(method_list);

			for (int j = 0; j < len(method_list); j++)
			{
				std::string method_name = boost::python::extract<std::string>(method_list[j]);

				if (PythonBuildInManager::getInstance()->isInternalMethod(method_name))
				{
					exec((std::string("current_method_argscount=") + moduleName + "." + method_name + ".__code__.co_argcount").data(), moduleDict, moduleDict);
					int arg_count = boost::python::extract<int>(moduleDict["current_method_argscount"]) - 1;
					int cmp_count = PythonBuildInManager::getInstance()->getIntenalMethodArgCount(method_name);
					if (arg_count == cmp_count)
					{
						pyModule.addMethod(method_name);
					}
					else
					{
						//return -1;
					}
				}
			}
			exec("inst = None", moduleDict, moduleDict);
			PythonResourceManager::getInstance()->addModule(moduleName, &pyModule);
			return PyResult::BehaviorComponent; 
		}
		else
		{
			exec("inst = None", moduleDict, moduleDict);
			return PyResult::Normal;
		}
	}
	catch (boost::python::error_already_set)
	{

		PyObject* type = NULL, * value = NULL, * traceback = NULL;
		PyErr_Fetch(&type, &value, &traceback);
		if (type)
		{
			std::cout << PyExceptionClass_Name(type) << ": ";
		}

		if (value)
		{
			PyObject* line = PyObject_Str(value);
			if (line && (PyUnicode_Check(line)))
				std::cout << PyUnicode_1BYTE_DATA(line);
		}

		std::cout << std::endl;

		if (traceback)
		{
			for (PyTracebackObject* tb = (PyTracebackObject*)traceback;
				NULL != tb;
				tb = tb->tb_next)
			{
				PyObject* line = PyUnicode_FromFormat("  File \"%U\", line %d, in %U\n",
					tb->tb_frame->f_code->co_filename,
					tb->tb_lineno,
					tb->tb_frame->f_code->co_name);
				std::cout << PyUnicode_1BYTE_DATA(line) << std::endl;
			}
		}
		return PyResult::ModuleError;
	}

}
void PythonScriptAnalyzer::analysisUncheckedProperty(PythonModuleInfo*& info)
{
	for (auto iter = info->uncheckedPropertyTypeMap.begin(); iter != info->uncheckedPropertyTypeMap.end(); iter++)
	{
		std::string name = (*iter).first;
		std::string type = (*iter).second;
		if (PythonBuildInManager::getInstance()->isBuildInModule(type) || PythonResourceManager::getInstance()->containModules(type))
		{
			info->addProperty(name, type, NULL);
		}
	}
	info->uncheckedPropertyTypeMap.clear();
}
std::string PythonScriptAnalyzer::getLastError()
{
	return  this->lastError;
}
std::string PythonScriptAnalyzer::getPythonClassName(std::string filePath)
{
	QFileInfo fileInfo(filePath.c_str());

	//get full-fileName
	std::string className = fileInfo.fileName().toStdString();
	//use string-split method to get moduleName
	return className.substr(0, className.find_first_of('.'));
}
/*
this->lastError.clear();

		PyObject* type = NULL, * value = NULL, * traceback = NULL;
		PyErr_Fetch(&type, &value, &traceback);
		if (type)
		{
			this->lastError = this->lastError + "Exception Class:[" + PyExceptionClass_Name(type) + "]\n";
		}

		if (value)
		{
			PyObject* line = PyObject_Str(value);
			if (line && (PyUnicode_Check(line)))
				this->lastError = this->lastError + "Line:" + reinterpret_cast<char*>(PyUnicode_1BYTE_DATA(line)) + "\n TraceBack:\n";
		}

		std::cout << std::endl;

		if (traceback)
		{
			for (PyTracebackObject* tb = (PyTracebackObject*)traceback;
				NULL != tb;
				tb = tb->tb_next)
			{
				PyObject* line = PyUnicode_FromFormat("  File \"%U\", line %d, in %U\n",
					tb->tb_frame->f_code->co_filename,
					tb->tb_lineno,
					tb->tb_frame->f_code->co_name);
				this->lastError = this->lastError + reinterpret_cast<char*>(PyUnicode_1BYTE_DATA(line)) + "\n";
			}
		}*/