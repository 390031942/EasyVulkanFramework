#pragma once

#include <string>
#include <map>
#include <vector>

#include <boost/python.hpp>

class PythonModuleInfo
{
private:
	std::map<std::string, std::string> propertyTypeMap;
	std::map<std::string, std::string> uncheckedPropertyTypeMap;
	std::map<std::string, void*> propertyMap;
	std::vector<std::string> internalMethods;
	std::vector<std::string> simpleMethods;
	bool IsMethodDefined(std::string methodName);
	boost::python::object module;
	boost::python::object dict;
	boost::python::object thisClass;

	void addProperty(std::string propertyName, std::string propertyType, void* value);
	void addInternalMethod(std::string methodName);
	void addMethod(std::string methodName);
	void addUncheckedProperty(std::string propertyName, std::string propertyType);
public:
	std::string  moduleName;

	void initModule();
	boost::python::handle<> getHandle();

	bool isStartCallable();
	bool isUpdateCallable();
	bool isOnDestoryCallable();
	bool isOnEnabledStageChangedCallable();
	std::string queryPropertyType(std::string propertyName);

	friend class PythonScriptAnalyzer;
};
