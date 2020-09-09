#pragma once

#include "boostpythonmodule_global.h"

#include <Python.h>
#include <boost/python.hpp>
#include <string>

using namespace std;
using namespace boost::python;


class testClass
{
private:
	string str;
public:
	string GetString()
	{
		return str;
	}

	void SetString(string str)
	{
		this->str = str;
	}
};

#define DECLARE_PTR(className) typedef boost::shared_ptr<className> className##_PTR;
#define REGISTER_PTR(className) register_ptr_to_python<className##_PTR>()

typedef boost::shared_ptr<testClass> testClass_PTR;

BOOST_PYTHON_MODULE(Easy)
{
	class_<testClass>("testClass")
		.def("GetString", &testClass::GetString)
		.def("SetString", &testClass::SetString);
	;
	register_ptr_to_python<testClass_PTR>();
}