#pragma once

#include <QtWidgets/QWidget>
#include "ui_DisplayWidget.h"

class DisplayWidget : public QWidget
{
	Q_OBJECT

public:
	DisplayWidget(QWidget *parent = Q_NULLPTR);

private:
	Ui::DisplayWidgetClass ui;
};
