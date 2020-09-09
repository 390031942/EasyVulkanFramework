#include "PythonModuleManager.h"

std::vector<QString>                                       PythonModuleManager::pyFiles;
std::map<QString, QString>                                 PythonModuleManager::pyFilesMD5;
std::map<std::string, int>                                 PythonModuleManager::internalMethodArgCountMap;
std::map<std::string, PythonModuleInfo>                    PythonModuleManager::pyModules;
std::map<std::string, std::vector<PythonScriptComponent*>> PythonModuleManager::pyModuleHandlesMap;
std::map<int, PythonScriptComponent*>                      PythonModuleManager::pyModuleHandlesUIDMap;
std::map<std::string, std::vector<PythonScriptComponent*>>::iterator  PythonModuleManager::pyModuleHandlesMapIter;


bool PythonModuleManager::serachScript(QString& findPath, QString format)
{
	QDir* dir = new QDir(findPath);
	if (!dir->exists())
		return false;

	QStringList filter;
	filter << format;

	QList<QFileInfo>* dirInfoList = new QList<QFileInfo>(dir->entryInfoList(QDir::Dirs));
	QList<QFileInfo>* fomatInfoList = new QList<QFileInfo>(dir->entryInfoList(filter));

	for (int i = 0; i < dirInfoList->count(); i++) {
		if (dirInfoList->at(i).fileName() == "." || dirInfoList->at(i).fileName() == "..")
			continue;
		QString dirTmp = dirInfoList->at(i).filePath();
		serachScript(dirTmp, format);
	}
	for (int i = 0; i < fomatInfoList->count(); i++) {
		pyFiles.push_back(fomatInfoList->at(i).filePath());
	}
	delete dirInfoList;
	delete dir;
	return true;
}

void PythonModuleManager::calculateMD5(QString filePath)
{
	QFile pyFile(filePath);
	pyFile.open(QIODevice::ReadOnly);
	QByteArray ba = QCryptographicHash::hash(pyFile.readAll(), QCryptographicHash::Md5);
	pyFile.close();

	//没有找到或不相等，重新获取Python文件的信息

	if (pyFilesMD5.find(filePath) == pyFilesMD5.cend() || QString(ba.toHex().constData()) != pyFilesMD5[filePath])
	{
		getPythonFileInfo(filePath.toStdString());
	}
	pyFilesMD5[filePath] = ba.toHex().constData();
}

int PythonModuleManager::getInternalMethodArgCount(std::string methodName)
{
	return internalMethodArgCountMap[methodName];
}

void PythonModuleManager::getPythonFileInfo(std::string filePath)
{
	QFileInfo fileInfo(filePath.c_str());
	std::string moduleName = fileInfo.fileName().toStdString();
	moduleName = moduleName.substr(0,moduleName.find_first_of('.'));
	boost::python::dict moduleDict = boost::python::dict();

	exec_file(filePath.c_str(), moduleDict, moduleDict);
	exec((std::string("inst=") + moduleName + "()\n"
		+ "variables=[attr for attr in dir(inst) if not callable(getattr(inst, attr)) and not attr.startswith(\"__\")]\n"
		+ "length=len(variables)\n"
		+ "current_key=0\n"
		+ "current_type=0\n"
		+ "current_variable=0\n").data(),
		moduleDict, moduleDict);

	boost::python::list varList= boost::python::extract<boost::python::list>(moduleDict["variables"]);

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

		exec(("current_key=variables["+std::to_string(i)+"]").data(), moduleDict, moduleDict);
		std::string key = boost::python::extract<std::string>(moduleDict["current_key"]);
		exec(("current_type=str(type(inst."+key+")).split(\"\'\")[1]").data(),moduleDict, moduleDict);

		exec((std::string("current_value=inst.")+key).data(), moduleDict, moduleDict); 
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
			for (auto pair : pyModules)
			{
				if (pair.first == type)
				{
					pyModule.addProperty(key, type, NULL);
					break;
				}
			}
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
	
		if (IsInternalMethod(method_name))
		{
			exec((std::string("current_method_argscount=") + moduleName + "." + method_name + ".__code__.co_argcount").data(), moduleDict, moduleDict);
			int arg_count = boost::python::extract<int>(moduleDict["current_method_argscount"])-1;
			int cmp_count = getInternalMethodArgCount(method_name);
			if (arg_count == cmp_count)
			{
				pyModule.addMethod(method_name);
			}
			else
			{
				return;
			}
		}
	}
	exec("inst = None", moduleDict, moduleDict);
	pyModule.initModule();
	pyModules[moduleName] = pyModule;
}

bool PythonModuleManager::IsInternalMethod(std::string methodName)
{
	for (auto pair : internalMethodArgCountMap)
	{
		if (methodName == pair.first)
			return true;
	}
	return false;
}

void PythonModuleManager::run()
{
	for (auto pair : pyModuleHandlesMap)
	{
		std::vector<PythonScriptComponent*> handles = pair.second;
		PythonModuleInfo moduleInfo = pyModules[pair.first];
		for (PythonScriptComponent* handle : handles)
		{
			if (moduleInfo.isStartCallable())
			{

			}
			else if (moduleInfo.isUpdateCallable())
			{

			}
		}
	}
}

void PythonModuleManager::init()
{
	internalMethodArgCountMap["Update"] = 0;
	internalMethodArgCountMap["Start"] = 0;
	internalMethodArgCountMap["OnEnabledStageChanged"] = 1;
	internalMethodArgCountMap["OnDestory"] = 0;

	serachScript(QCoreApplication::applicationDirPath(), "*.py");

	for (int i = 0; i < pyFiles.size(); i++)
	{
		getPythonFileInfo(pyFiles[i].toStdString());
	}

	boost::python::object module = boost::python::object(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("__main__"))));
	boost::python::object main_namespace = module.attr("__dict__");


	for (int i = 0; i < 1; i++)
	{
		instantiate("testClass1");
	}
	exec("import EasyVulkan", main_namespace, main_namespace);


	pyModuleHandlesMapIter = pyModuleHandlesMap.begin();
}

PythonScriptComponent* PythonModuleManager::instantiate(std::string moduleName)
{
	PythonScriptComponent* component = pyModules.find(moduleName) != pyModules.cend() ? new PythonScriptComponent(pyModules[moduleName]) : NULL;
	if (component)
	{
		pyModuleHandlesMap[moduleName].push_back(component);
		pyModuleHandlesUIDMap[component->uid] = component;
	}
	return component;
}

int PythonModuleManager::getModuleCount()
{
	return pyModuleHandlesMap.size();
}

std::vector<PythonScriptComponent*> PythonModuleManager::getNextPythonComponents()
{
	auto result = (*pyModuleHandlesMapIter++).second;
	if (pyModuleHandlesMapIter == pyModuleHandlesMap.end())
		pyModuleHandlesMapIter = pyModuleHandlesMap.begin();
	return result;
}
