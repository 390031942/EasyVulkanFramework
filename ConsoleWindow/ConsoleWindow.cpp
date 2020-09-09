#include "ConsoleWindow.h"
#include <QProgressBar>
#include <QPushButton>
#include <QtWidgets\qmessagebox.h>

ConsoleWindow::ConsoleWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.listWidget->setSelectionMode(QListWidget::SelectionMode::ExtendedSelection);
	ui.listWidget->setStyleSheet("QListWidget::item:selected{background: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 #6ea1f1, stop: 1 #567dbc); //Ñ¡ÖÐ±³¾°} ");

	connect(ui.toolButton_3, &QToolButton::clicked, this, &ConsoleWindow::onClickSerach);
	connect(ui.toolButton, &QToolButton::clicked, this, &ConsoleWindow::onClickClaer);
	QMenu* menu = new QMenu;
	
	QAction* action_info = new QAction("Infomation");
	action_info->setCheckable(true);
	action_info->setChecked(true);

	QAction* action_warning = new QAction("Warning");
	action_warning->setCheckable(true);
	action_warning->setChecked(true);

	QAction* action_eror = new QAction("Error");
	action_eror->setCheckable(true);
	action_eror->setChecked(true);

	connect(action_info, &QAction::toggled, this, [=](bool value)
		{
			this->showInfo = value;
			this->updateConsoleItemAfterShowFlagChanged();
		});
	connect(action_warning, &QAction::toggled, this, [=](bool value)
		{
			this->showWarning = value;
			this->updateConsoleItemAfterShowFlagChanged();
		});

	connect(action_eror, &QAction::toggled, this, [=](bool value)
		{
			this->showError = value;
			this->updateConsoleItemAfterShowFlagChanged();
		});

	menu->addActions({ action_info ,action_warning,action_eror });

	ui.pushButton_2->setMenu(menu);
}

void ConsoleWindow::printInfo(std::string source, std::string method, std::string message)
{
	ConsoleMessageItemWidget* widget = new ConsoleMessageItemWidget(ui.listWidget, ConsoleMessageItemWidget::MessageType::Info, source, method, message);
	widget->setAttribute(Qt::WA_DeleteOnClose);
	QListWidgetItem* item = new QListWidgetItem;
	item->setSizeHint(QSize(item->sizeHint().width(), 60));
	ui.listWidget->addItem(item);
	ui.listWidget->setItemWidget(item, widget);

	itemMap[item] = widget;
}

void ConsoleWindow::printWarning(std::string source, std::string method, std::string message)
{
	ConsoleMessageItemWidget* widget = new ConsoleMessageItemWidget(ui.listWidget, ConsoleMessageItemWidget::MessageType::Warning, source, method, message);
	widget->setAttribute(Qt::WA_DeleteOnClose);
	QListWidgetItem* item = new QListWidgetItem;
	item->setSizeHint(QSize(item->sizeHint().width(), 60));
	ui.listWidget->addItem(item);
	ui.listWidget->setItemWidget(item, widget);

	itemMap[item] = widget;
}

void ConsoleWindow::printError(std::string source, std::string method, std::string message)
{
	ConsoleMessageItemWidget* widget = new ConsoleMessageItemWidget(ui.listWidget, ConsoleMessageItemWidget::MessageType::Error, source, method, message);
	widget->setAttribute(Qt::WA_DeleteOnClose);
	QListWidgetItem* item = new QListWidgetItem;
	item->setSizeHint(QSize(item->sizeHint().width(), 60));
	ui.listWidget->addItem(item);
	ui.listWidget->setItemWidget(item, widget);

	itemMap[item] = widget;
}

void ConsoleWindow::updateConsoleItemAfterShowFlagChanged()
{
	for (auto pair : itemMap)
	{
		if ((pair.second)->getType() == ConsoleMessageItemWidget::MessageType::Info)
		{
			if (!showInfo)
			{
				ui.listWidget->setItemHidden(pair.first, true);
			}
			else
			{
				ui.listWidget->setItemHidden(pair.first, false);
			}
		}
		else if ((pair.second)->getType() == ConsoleMessageItemWidget::MessageType::Warning)
		{
			if (!showWarning)
			{
				ui.listWidget->setItemHidden(pair.first, true);
			}
			else
			{
				ui.listWidget->setItemHidden(pair.first, false);
			}
		}
		else if ((pair.second)->getType() == ConsoleMessageItemWidget::MessageType::Error)
		{
			if (!showError)
			{
				ui.listWidget->setItemHidden(pair.first, true);
			}
			else
			{
				ui.listWidget->setItemHidden(pair.first, false);
			}
		}
	}
}

void ConsoleWindow::keyPressEvent(QKeyEvent* e)
{
	if (ui.listWidget->hasFocus())
	{
		if (e->modifiers() == Qt::KeyboardModifiers::enum_type::ControlModifier && e->key() == Qt::Key::Key_F)
		{
			this->onClickSerach();
		}
	}
}

void ConsoleWindow::onClickSerach()
{
	std::string content;
	SingletonDialog::setInstanceType("ConsoleSerach");
	SingletonDialog* dialog = new SingletonDialog;

	dialog->setParent(ui.listWidget);

	dialog->setStyleSheet("QDialog{background-color: rgb(255, 255, 255);border: 1px solid gray}");
	dialog->setGeometry(ui.listWidget->size().width() - 250, 0, 230, 50);


	QLabel* l = new QLabel("Find");
	l->setParent(dialog);
	l->setGeometry(10, 15, 100, 20);
	l->setAttribute(Qt::WA_DeleteOnClose);

	QLineEdit* e = new QLineEdit;
	e->setGeometry(50, 15, 100, 20);
	e->setParent(dialog);
	e->setAttribute(Qt::WA_DeleteOnClose);

	IconButton* icon_Serach = new IconButton("icons/console/serach.png");
	IconButton* icon_Close = new IconButton("icons/console/close.png");

	icon_Serach->setParent(dialog);
	icon_Serach->setPosition(170, 15);
	icon_Serach->setFixedSize(20, 20);

	icon_Close->setParent(dialog);
	icon_Close->setPosition(200, 15);
	icon_Close->setFixedSize(20, 20);


	dialog->show();

	icon_Close->addClickEvent([=]() {
		dialog->close();
		});

	icon_Serach->addClickEvent([=]() {
		ui.listWidget->clearSelection();
		for (auto pair : itemMap)
		{
			if (pair.second->serach(e->text().toStdString()))
			{
				(pair.first)->setSelected(true);
			}
		}
		});
}

void ConsoleWindow::onClickClaer()
{
	itemMap.clear();
	ui.listWidget->clear();
}
