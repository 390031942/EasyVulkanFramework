#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ObjectTreeTestWidget.h"

class ObjectTreeTestWidget : public QMainWindow
{
	Q_OBJECT

public:
	ObjectTreeTestWidget(QWidget *parent = Q_NULLPTR);

private:
	Ui::ObjectTreeTestWidgetClass ui;
};
