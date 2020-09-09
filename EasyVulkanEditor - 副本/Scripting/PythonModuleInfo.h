#pragma once

#include <string>
#include <map>
#include <vector>

#include <boost/python.hpp>

class PythonModuleInfo
{
private:
	std::map<std::string, std::string> propertyTypeMap;
	std::map<std::string, void*> propertyMap;
	std::vector<std::string> methods;

	bool IsMethodDefined(std::string methodName);
	boost::python::object module;
	boost::python::object dict;
	boost::python::object thisClass;
public:
	std::string  moduleName;

	void initModule();
	boost::python::handle<> getHandle();
	void addProperty(std::string propertyName, std::string propertyType, void* value);
	void addMethod(std::string methodName);
	bool isStartCallable();
	bool isUpdateCallable();
	bool isOnDestoryCallable();
	bool isOnEnabledStageChangedCallable();
	std::string queryPropertyType(std::string propertyName);
};
