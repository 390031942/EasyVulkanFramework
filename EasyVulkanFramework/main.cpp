#include "VulkanDisplayWindow.h"
#include "EasyVulkanWindow.h"

#include <QtWidgets/QApplication>
#include <QTimer>
#include <Windows.h>

int main(int argc, char* argv[]){
	QApplication a(argc, argv);

	EasyVulkanWindow *w = new EasyVulkanWindow;
	w->setWidth(1280);
	w->setHeight(720);
	w->InitVulkan();
	w->show();
	thread t1(&EasyVulkanWindow::StartNextFrame, w);

	t1.detach();

	return a.exec();
}
