#pragma once

#include <QtWidgets/QWidget>
#include "ui_VulkanDisplayWindow.h"

class VulkanDisplayWindow : public QWidget
{
	Q_OBJECT

public:
	VulkanDisplayWindow(QWidget *parent = Q_NULLPTR);
	

private:
	Ui::VulkanDisplayWindowClass ui;
};
