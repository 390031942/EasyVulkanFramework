#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ConsoleWindow.h"
#include "ConsoleMessageItemWidget.h"
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include "IconButton.h"
#include "SingletonDialog.h"
#include <QKeyEvent>

class ConsoleWindow : public QMainWindow
{
	Q_OBJECT

public:
	ConsoleWindow(QWidget *parent = Q_NULLPTR);

private:
	Ui::ConsoleWindowClass ui;
	std::map <QListWidgetItem*, ConsoleMessageItemWidget*> itemMap;

	bool showInfo;
	bool showWarning;
	bool showError;

public:
	void printInfo(std::string source, std::string method, std::string message);
	void printWarning(std::string source, std::string method, std::string message);
	void printError(std::string source, std::string method, std::string message);
	void updateConsoleItemAfterShowFlagChanged();
	void keyPressEvent(QKeyEvent* e);
	void onClickSerach();
	void onClickClaer();
};
