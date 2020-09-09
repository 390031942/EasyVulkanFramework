#pragma once
#include "PythonScriptComponent.h"
#include "PythonResourceManager.h"
#include "PythonScanner.h"
#include <queue>
#include <thread>
#include <boost/python.hpp>
#include "../EditorMenuManager.h"
#include "../PythonErrorUtility.h"
#include "../EasyObject.h"

#define PREVIOUS_STAGE bool

class PythonEditorExecutor:public QObject
{
	Q_OBJECT
	_Singleton_declare_(PythonEditorExecutor);
	_uuid_declare_
private:
	static std::thread             mainThread;

	static std::map<int, PREVIOUS_STAGE> enabledStageMap;
	static void run();

	static int  count;

	boost::python::object *main_module = NULL;
	boost::python::object *main_namespace = NULL;

	boost::python::object menuItemDict;
public:
	void invokeMenuAction(std::string action);
	std::vector<std::string> getMenuActionTexts();
	bool init();
Q_SIGNALS:
	void updateEditorSignal();
};

/*
	Ui::EasyVulkanEditorClass ui;
	dict MenuItemMap;
	QMap<QAction*, bool> objectLock;
	object obj_dict;
	object dictionary;

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
*/