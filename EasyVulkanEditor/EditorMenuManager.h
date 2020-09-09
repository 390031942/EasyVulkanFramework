#pragma once

#include <QObject>
#include <QMenuBar>

#include <stack>

#include "Scripting/PythonEditorExecutor.h"
//×Ö·û´®·Ö¸îº¯Êý
static std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//À©Õ¹×Ö·û´®ÒÔ·½±ã²Ù×÷
	int size = str.size();

	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

class EditorMenuManager :public EasyObject
{
	_Singleton_declare_(EditorMenuManager);
	_uuid_declare_;
private:
	QMenuBar* pMenuBar;
public:
	EditorMenuManager();
	~EditorMenuManager();

	std::map<std::string, QAction*> actionMap;
	std::map<std::string, QMenu*> menuMap;

	void setMenuBar(QMenuBar* pMenuBar);
	void addAction(std::string action);
	QMenu* findMenu(std::string menu);
	QAction* findAction(std::string action);
	void addActions(std::vector<std::string> actions);
	void removeAction(std::string action);
};
