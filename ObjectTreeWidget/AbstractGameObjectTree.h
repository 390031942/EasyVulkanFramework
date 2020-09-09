#ifndef  GameObjectTreeWidget_H
#define  GameObjectTreeWidget_H
#pragma execution_character_set("utf-8")
#pragma region Header
#include <QWidget>
#include <QTreeWidget>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <qmessagebox.h>
#include <qmenu.h>
#include <QQueue>
#include <QStack>
#include <qlineedit.h>
#include "GameObjectItem.h"
#include "qheaderview.h"
#include <QPushButton>
#include <QCheckBox>
#include <QMimeData>
#pragma endregion

#define ItemList                    QList<GameObjectItem*>
#define Item                        GameObjectItem*
#define MyMessageBox(title,content) QMessageBox::information(NULL,title,content)
#define ObjectTree                  AbstractGameObjectTree::getInstance()

class AbstractGameObjectTree : public QTreeWidget
{
	Q_OBJECT
//构造函数与初始化部分
public:
	explicit    AbstractGameObjectTree(QWidget *parent = 0);    //构造函数
	void        init();                           	 //界面数据初始化
	QMenu* menu;
//拖动节点部分：
protected:
	void        dragEnterEvent(QDragEnterEvent *event); 
	void        dragMoveEvent(QDragMoveEvent *event);
	void        dropEvent(QDropEvent *event);
	void        keyPressEvent(QKeyEvent *event);
	void        mousePressEvent(QMouseEvent * event);
	void        commitData(QWidget * editor);
//节点添加部分:
public:
	Item        addTreeRoot(GameObject *obj);
	Item        addTreeNode(GameObjectItem *&parent, GameObject *obj);

	bool        CheckNameExist(QString name);
	QString     AllocateName(QString name);
	QList<GameObjectItem*> selectedItems() const;

	static      AbstractGameObjectTree* getInstance()
	{
		return instance;
	}
//节点复制部分:
private:
	ItemList    copiedItems;
	Item        copyRootNode(GameObjectItem* srcitem);
	void        copyChildNode(GameObjectItem * srcitem, GameObjectItem * destitem);
	void        onCopyItems();
	void        doCopySelectedItems();
//节点选择部分:
	ItemList    getAllChilds(GameObjectItem* root);
	ItemList    getAllNodes()
	{
		QList<GameObjectItem*> itemlist;

		return itemlist;
	}
	void        doCancelSelection();
	int         isParentChildRelation(GameObjectItem* root, GameObjectItem* target)
	{
		auto children = getAllChilds(root);
		for (auto child : children)
		{
			if (child == target)
				return 1;
		}
		children = getAllChilds(target);
		for (auto child : children)
		{
			if (child == root)
				return 2;
		}
		return 0;
	}
//节点删除部分
	void        doDeleteSelected();
//菜单添加部分
private slots:
	void        doMenuActionHandler(QAction *action);
	//void        SubMenu_Create_Triggered(QAction *action);
Q_SIGNALS:
	void        cancelSelectionSignal();
	void        Create_Buildin_Objects(int code);
private:
	void        createItemMenu(); //构造函数中调用此函数
protected:
	void        contextMenuEvent(QContextMenuEvent * event);
	static      AbstractGameObjectTree* instance;

public:
	/*virtual void onParentChildRelationChanged(GameObjectItem* parent, std::vector<GameObjectItem*> childrens) = 0;
	virtual void onItemsNameChanged(std::vector<GameObjectItem*> items) = 0;
	virtual void doMenuDisplayed(GameObjectItem* targetItem) = 0 ;
	virtual void addGameObject(GameObject* gameObject) = 0;
	virtual void addGameObjects(std::vector<GameObject*> gameObjects) = 0 ;
	virtual void setGameObjectActiveStage(GameObject* gameObject, bool stage) = 0;
	virtual void setGameObjectsActiveStage(std::vector<GameObject*> gameObjects, bool stage) = 0 ;
	virtual void doGameObjectDestory(GameObject* gameObject) = 0 ;
	virtual void doGameObjectsDestory(GameObject* gameObject) = 0 ;
	virtual void addAction(std::string path, void(__cdecl* slot)(void)) = 0;*/
};

#endif // ! GameObjectTreeWidget_H