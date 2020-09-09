#pragma once

#include <QApplication>

#include "Scripting/PythonEditorExecutor.h"
#include "EasyVulkanEditor.h"
#include "EditorMenuManager.h"

class EasyEditorApplication : public QApplication
{
	Q_OBJECT

public:
	EasyEditorApplication(int argc, char** argv);
	~EasyEditorApplication();

	int exec()
	{
		EasyVulkanEditor* editor = new EasyVulkanEditor;
		EditorMenuManager::getInstance()->setMenuBar(editor->getMenuBar());

		if (!PythonEditorExecutor::getInstance()->init())
			return -1;


		editor->show();
		return QApplication::exec();
	}
};
