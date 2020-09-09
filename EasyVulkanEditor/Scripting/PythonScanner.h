#pragma once
#include "../EasyObject.h"
#include "PythonScriptAnalyzer.h"

#include <QDir>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <boost/python.hpp>
#include <Python.h>
#include <string>

#include <QFileInfo>

#define Defalut_Script_Path QString(QCoreApplication::applicationDirPath()+"/script")
#define Defalut_BuildIn_Path QString(QCoreApplication::applicationDirPath()+"/build-In")

class PythonScanner:public EasyObject
{
	_Singleton_declare_(PythonScanner);
	_uuid_declare_

private:
	std::vector<std::string> pyScripts;
	std::vector<std::string> pyBuildIns;
	std::map<std::string, std::string> pyFilesMD5;
	bool serachScript(QString& findPath, QString format = "*.py");
	void calculateMD5(QString filePath);

	enum ScanStage {NotYet,BuildIn,Script,Finished};
	ScanStage stage = NotYet;
public:
	void doScan();
	std::vector<std::string> getPythonScripts();
	std::vector<std::string> getPythonBuildIns();
};

	