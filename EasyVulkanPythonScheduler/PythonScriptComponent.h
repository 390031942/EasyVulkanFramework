#pragma once
#include "Component.h"
#include "PythonModuleInfo.h"

#include <map>
#include <string>
#include <python.h>
#include <boost/python.hpp>

class PythonScriptComponent:public Component
{
private:
	PythonModuleInfo info;
public:

	boost::python::handle<> handle;

	PythonScriptComponent(PythonModuleInfo info)
	{
		this->info = info;
		this->name = info.moduleName;
		this->handle = info.getHandle();

		this->uid = boost::python::call_method<int>(this->handle.get(), "getUID");
	}

	template<typename T>
	void setProperty(std::string propertyName, T value)
	{
		if (this->info.queryPropertyType(propertyName) == "NULL")
			return;

		boost::python::setattr(boost::python::api::object(this->handle), propertyName.c_str(), object(value));
	}

	std::string getPropertyType(std::string propertyName)
	{
		return this->info.queryPropertyType(propertyName);
	}

	template<typename T>
	T getProperty(std::string propertyName)
	{
		return boost::python::extract<T>(boost::python::getattr(boost::python::api::object(this->handle), boost::python::api::object(propertyName)));
	}

	void start()
	{
		if (!info.isStartCallable())
			return;
		boost::python::call_method<void>(this->handle.get(), "Start");
	}

	void update()
	{
		if (!info.isUpdateCallable())
			return;
		boost::python::call_method<void>(this->handle.get(), "Update");
	}

	void onDestory()
	{
		if (!info.isOnDestoryCallable())
			return;
		boost::python::call_method<void>(this->handle.get(), "OnDestory");
	}

	void onEnabledStageChanged(bool stage)
	{
		if (!info.isOnEnabledStageChangedCallable())
			return;
		boost::python::call_method<void, bool>(this->handle.get(), "OnEnabledStageChanged", stage);
	}
};