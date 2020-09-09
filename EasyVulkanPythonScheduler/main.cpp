#include <QtCore/QCoreApplication>


int main(int argc, char *argv[])
{


	QCoreApplication a(argc, argv);
	//typedef boost::shared_ptr<testClass> testClass_PTR; 
	
	//Py_Finalize();
	return a.exec();
}
