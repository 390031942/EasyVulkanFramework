#include "ConsoleWindow.h"
#include <QtWidgets/QApplication>

#include "Label.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ConsoleWindow w;
	w.printInfo(__FILE__, __FUNCTION__, "printInfo Test");
	w.printWarning(__FILE__, __FUNCTION__, "printWarning Test");
	w.printError(__FILE__, __FUNCTION__, "printError Test");
	w.printInfo(__FILE__, __FUNCTION__, "printInfo Test");
	w.printWarning(__FILE__, __FUNCTION__, "printWarning Test");
	w.printError(__FILE__, __FUNCTION__, "printError Test");
	w.printInfo(__FILE__, __FUNCTION__, "printInfo Test");
	w.printWarning(__FILE__, __FUNCTION__, "printWarning Test");
	w.printError(__FILE__, __FUNCTION__, "printError Test");
	w.show();

	//w.serach();

	return a.exec();
}
