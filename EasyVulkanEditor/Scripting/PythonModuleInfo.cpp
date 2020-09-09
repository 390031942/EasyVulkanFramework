#include "PythonModuleInfo.h"

 bool PythonModuleInfo::IsMethodDefined(std::string methodName)
{
	for (int i = 0; i < internalMethods.size(); i++)
	{
		if (internalMethods[i] == methodName)
			return true;
	}
	return false;
}

 void PythonModuleInfo::initModule()
{
	module = boost::python::import(moduleName.c_str());
	dict = module.attr("__dict__");
	thisClass = dict[moduleName.c_str()];

}

 boost::python::handle<> PythonModuleInfo::getHandle()
{
	return boost::python::extract<boost::python::handle<>>(thisClass());
}

 void PythonModuleInfo::addProperty(std::string propertyName, std::string propertyType, void* value)
{
	propertyTypeMap[propertyName] = propertyType;
	propertyMap[propertyName] = value;
}

void PythonModuleInfo::addInternalMethod(std::string methodName)
 {
	 internalMethods.push_back(methodName);
 }

void PythonModuleInfo::addMethod(std::string methodName)
 {
	 simpleMethods.push_back(methodName);
 }

void PythonModuleInfo::addUncheckedProperty(std::string propertyName, std::string propertyType)
{
	uncheckedPropertyTypeMap[propertyName] = propertyType;
}

bool PythonModuleInfo::isStartCallable()
{
	return IsMethodDefined("Start");
}

bool PythonModuleInfo::isUpdateCallable()
{
	return IsMethodDefined("Update");
}

bool PythonModuleInfo::isOnDestoryCallable()
{
	return IsMethodDefined("OnDestory");
}

bool PythonModuleInfo::isOnEnabledStageChangedCallable()
{
	return IsMethodDefined("OnEnabledStageChanged");
}

std::string PythonModuleInfo::queryPropertyType(std::string propertyName)
{
	return propertyMap.find(propertyName) != propertyMap.cend() ? propertyTypeMap[propertyName] : "NULL";
}
