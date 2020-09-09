#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_EasyVulkanEditor.h"
class EasyVulkanEditor : public QMainWindow
{
	Q_OBJECT

public:
	EasyVulkanEditor(QWidget *parent = Q_NULLPTR);
	~EasyVulkanEditor()
	{
	
	}

	QMenuBar* getMenuBar()
	{
		return ui.menuBar;
	}
private:
	Ui::EasyVulkanEditorClass ui;
};
