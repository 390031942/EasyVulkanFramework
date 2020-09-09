#pragma once
#include <string>
#include <frameObject.h>
#include <python.h>

#include <QMessageBox>

static void getLastErrorMessageBox()
{
	std::string lastError;
	PyObject* type = NULL, * value = NULL, * traceback = NULL;

	PyErr_Fetch(&type, &value, &traceback);
	if (type)
	{
	    lastError =lastError + "Exception Class:[" + PyExceptionClass_Name(type) + "]\n";
	}

	if (value)
	{
		PyObject* line = PyObject_Str(value);
		if (line && (PyUnicode_Check(line)))
			lastError = lastError + "Line:" + reinterpret_cast<char*>(PyUnicode_1BYTE_DATA(line)) + "\n TraceBack:\n";
	}

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
			lastError = lastError + reinterpret_cast<char*>(PyUnicode_1BYTE_DATA(line)) + "\n";
		}
	}
	QMessageBox::information(NULL, "Python Error", lastError.c_str());
}