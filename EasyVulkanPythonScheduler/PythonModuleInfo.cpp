#include "PythonModuleInfo.h"

 bool PythonModuleInfo::IsMethodDefined(std::string methodName)
{
	for (int i = 0; i < methods.size(); i++)
	{
		if (methods[i] == methodName)
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
	return boost::python::extract<boost::python::handle<> >(thisClass());
}

 void PythonModuleInfo::addProperty(std::string propertyName, std::string propertyType, void* value)
{
	propertyTypeMap[propertyName] = propertyType;
	propertyMap[propertyName] = value;
}

 void PythonModuleInfo::addMethod(std::string methodName)
{
	methods.push_back(methodName);
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
