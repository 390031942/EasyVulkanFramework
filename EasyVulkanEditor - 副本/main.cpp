#include "EasyVulkanEditor.h"
#include <QtWidgets/QApplication>
#include <Python.h>
#include <boost/python.hpp>
#include <Windows.h>
#include <frameobject.h>
#include <iostream>
#include <streambuf>
#include <fstream>

using namespace std;
using namespace boost::python;



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//typedef boost::shared_ptr<testClass> testClass_PTR; 
	EasyVulkanEditor* w = new EasyVulkanEditor;

	w->show();
	return a.exec();
}
