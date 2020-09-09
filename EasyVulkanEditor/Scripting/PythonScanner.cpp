#include "PythonScanner.h"

_Singleton_impl_(PythonScanner);
_uuid_impl_(PythonScanner, "9fc85c3e-8581-4788-b21d-95677ded03a5");

bool PythonScanner::serachScript(QString& findPath, QString format)
{
	QDir* dir = new QDir(findPath);
	if (!dir->exists())
		return false;

	QStringList filter;
	filter << format;

	QList<QFileInfo>* dirInfoList = new QList<QFileInfo>(dir->entryInfoList(QDir::Dirs));
	QList<QFileInfo>* fomatInfoList = new QList<QFileInfo>(dir->entryInfoList(filter));

	for (int i = 0; i < dirInfoList->count(); i++) {
		if (dirInfoList->at(i).fileName() == "." || dirInfoList->at(i).fileName() == "..")
			continue;
		QString dirTmp = dirInfoList->at(i).filePath();
		serachScript(dirTmp, format);
	}
	for (int i = 0; i < fomatInfoList->count(); i++) {
		auto str = fomatInfoList->at(i).filePath();

		if(stage == ScanStage::BuildIn)
			pyBuildIns.push_back(str.toStdString());

		else if (stage == ScanStage::Script)
			pyScripts.push_back(str.toStdString());
	}
	delete dirInfoList;
	delete dir;
	return true;
}

void PythonScanner::calculateMD5(QString filePath)
{
	QFile pyFile(filePath);
	pyFile.open(QIODevice::ReadOnly);
	QByteArray ba = QCryptographicHash::hash(pyFile.readAll(), QCryptographicHash::Md5);
	pyFile.close();

	//没有找到或不相等，重新获取Python文件的信息

	if (pyFilesMD5.find(filePath.toStdString()) == pyFilesMD5.cend() || QString(ba.toHex().constData()).toStdString() != pyFilesMD5[filePath.toStdString()])
	{
		//getPythonFileInfo(filePath.toStdString());
	}
	pyFilesMD5[filePath.toStdString()] = QString(ba.toHex().constData()).toStdString();
}

void PythonScanner::doScan()
{
	if (stage == ScanStage::Finished)
		return;
	stage = ScanStage::BuildIn;
	serachScript(Defalut_BuildIn_Path, "*.py");
	stage = ScanStage::Script;
	serachScript(Defalut_Script_Path, "*.py");
	stage = ScanStage::Finished;
}
std::vector<std::string> PythonScanner::getPythonBuildIns()
{
	return this->pyBuildIns;
}
std::vector<std::string> PythonScanner::getPythonScripts()
{
	return this->pyScripts;
}
