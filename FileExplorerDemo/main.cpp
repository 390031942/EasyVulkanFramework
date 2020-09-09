#include "FileExplorerDemo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FileExplorerDemo w;
	w.show();
	return a.exec();
}
