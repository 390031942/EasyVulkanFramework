#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_EasyVulkanEditor.h"
#include <Python.h>
#include <boost/python.hpp>
#include <Windows.h>
#include <iostream>
#include <frameobject.h>
#include <thread>
using namespace std;
using namespace boost::python;
//×Ö·û´®·Ö¸îº¯Êý
static std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//À©Õ¹×Ö·û´®ÒÔ·½±ã²Ù×÷
	int size = str.size();

	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}
class EasyVulkanEditor : public QMainWindow
{
	Q_OBJECT

public:
	EasyVulkanEditor(QWidget *parent = Q_NULLPTR);
private:
	Ui::EasyVulkanEditorClass ui;
	dict                      MenuItemMap;
	QMap<QAction*, bool>      objectLock;

	PyGILState_STATE mystage;
	void closeEvent(QCloseEvent*)
	{
		PyGILState_Ensure();
		Py_Finalize();
		exit(0);
	}
public:
	void call()
	{
		QAction* action = qobject_cast<QAction*>(sender());
		try
		{
			object vlist = MenuItemMap[action->objectName().toStdString()];
			if (objectLock.contains(action))
			{
				if (objectLock[action])
				{
					return;
				}
			}
			this->mystage = PyGILState_Ensure();
			vlist();
			PyGILState_Release(this->mystage);

		}
		catch (error_already_set)
		{
			PyErr_Print();
		}
	}
	void addActionOp(QAction *action)
	{
		connect(action, &QAction::triggered, this, &EasyVulkanEditor::call);
	}
};
