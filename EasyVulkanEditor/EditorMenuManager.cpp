#include "EditorMenuManager.h"

_Singleton_impl_(EditorMenuManager);
_uuid_impl_(EditorMenuManager,"b40b4bfb-9748-40a3-b903-6d048757dec1");

EditorMenuManager::EditorMenuManager()
{
}

EditorMenuManager::~EditorMenuManager()
{
}

void EditorMenuManager::setMenuBar(QMenuBar* pMenuBar)
{
	this->pMenuBar = pMenuBar;
}

void EditorMenuManager::addAction(std::string action)
{
	auto strs = split(action, "/");
	std::string current_path;

	QMenu* menu = findMenu(strs[0]);
	if (!menu)
	{
		menu = new QMenu(strs[0].c_str());

		pMenuBar->addMenu(menu);
		menuMap[strs[0]] = menu;
	}
	current_path = strs[0];
	std::stack<QAction*> actionsStack;
	if (strs.size() > 1)
	{
		std::string menuName = strs[0];
		for (int i = 1; i < strs.size(); i++)
		{
			current_path += ("/" + strs[i]);
			QAction* pAction = findAction(strs[i]);
			if (!pAction)
			{
				pAction = new QAction(strs[i].c_str());
				actionMap[current_path.c_str()] = pAction;

				if (i == 1)
				{
					menu->addAction(pAction);
				}
			}
			actionsStack.push(pAction);
		}
		QObject::connect(actionsStack.top(), &QAction::triggered, [=]()
			{
				PythonEditorExecutor::getInstance()->invokeMenuAction(action);
			});
	}
}

QMenu* EditorMenuManager::findMenu(std::string menu)
{
	auto iter = menuMap.find(menu);
	if (iter == menuMap.end())
		return NULL;
	return menuMap.find(menu).operator*().second;
}

QAction* EditorMenuManager::findAction(std::string action)
{
	auto iter = actionMap.find(action);
	if (iter == actionMap.end())
		return NULL;
	return actionMap.find(action).operator*().second;
}

void EditorMenuManager::addActions(std::vector<std::string> actions)
{
	for (int i = 0; i < actions.size(); i++)
	{
		addAction(actions[i]);
	}
}

void EditorMenuManager::removeAction(std::string action)
{

}
