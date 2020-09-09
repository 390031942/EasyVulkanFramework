#include "AbstractGameObjectTree.h"
#include <QtWidgets\qboxlayout.h>

AbstractGameObjectTree* AbstractGameObjectTree::instance;
AbstractGameObjectTree::AbstractGameObjectTree(QWidget *parent) : QTreeWidget(parent)
{
	init();	//界面数据初始化
	createItemMenu();
	this->header()->hide();
	connect(this, &AbstractGameObjectTree::currentItemChanged, this,
		[=](QTreeWidgetItem* current, QTreeWidgetItem* previous)
		{
			GameObjectItem* item = (GameObjectItem*)current;
			if (!item) return;
			GameObjectItem* item_parent = (GameObjectItem*)item->parent(); 
			GameObject* obj = item_parent ? item_parent->GetObjectPtr() : NULL;

			instance = this; }
	);

	GameObject* gameObject = new GameObject;
	gameObject->name = "12345";

	addTreeRoot(gameObject);

	gameObject = new GameObject;
	gameObject->name = "23456";

	addTreeRoot(gameObject);

	gameObject = new GameObject;
	gameObject->name = "34567";

	addTreeRoot(gameObject);

	gameObject = new GameObject;
	gameObject->name = "45678";

	addTreeRoot(gameObject);
}

void AbstractGameObjectTree::init()
{
	this->setMouseTracking(true);
	this->setDragEnabled(true);
	this->setAcceptDrops(true);
	this->setDefaultDropAction(Qt::MoveAction);
	this->setSelectionMode(QTreeWidget::SelectionMode::ExtendedSelection);
	this->setEditTriggers(AbstractGameObjectTree::EditTrigger::DoubleClicked);
	this->expandAll();
}

void AbstractGameObjectTree::dragEnterEvent(QDragEnterEvent *event)
{
	QTreeWidget::dragEnterEvent(event);
}
void AbstractGameObjectTree::dragMoveEvent(QDragMoveEvent *event)
{
	QTreeWidget::dragMoveEvent(event);
}
void AbstractGameObjectTree::dropEvent(QDropEvent *event)
{
	auto targetItem = itemAt(event->pos());
	auto item = selectedItems();

	QTreeWidget::dropEvent(event);
	for (int i = 0; i < item.count(); i++)
	{
		auto a = item[0]->parent();
		assert(targetItem == a);
	}
	//this->onParentChildRelationChanged((GameObjectItem*)targetItem,item.toVector().toStdVector());
}
void AbstractGameObjectTree::keyPressEvent(QKeyEvent * event)
{
	if (!hasFocus())
	{
		return;
	}
	if (event->key() == Qt::Key_C  &&  event->modifiers() == (Qt::ControlModifier))
	{
		onCopyItems();
	}
	else if (event->key() == Qt::Key_V  &&  event->modifiers() == (Qt::ControlModifier))
	{
		doCopySelectedItems();
	}
	else if (event->key() == Qt::Key_A  &&  event->modifiers() == (Qt::ControlModifier))
	{
		selectAll();
	}
	else if (event->key() == Qt::Key_Delete)
	{
		doDeleteSelected();
	}
}

void AbstractGameObjectTree::mousePressEvent(QMouseEvent * event)
{
	auto index = this->indexAt(event->pos());
	if (index.row() == -1)
	{
		emit cancelSelectionSignal();
		setCurrentIndex(index);
	}
	else
	{
		QTreeWidget::mousePressEvent(event);
	}
}

void AbstractGameObjectTree::commitData(QWidget * editor)
{
	auto item = selectedItems();
	for (int i = 0; i < item.count(); i++)
	{
		
	}

	QLineEdit * edit = (QLineEdit *)editor;
	QString strText = edit->text();
	if (strText.isEmpty())  //如果文本为空，则不提交
	{
		return; 
	}
	QTreeWidget::commitData(editor);
}

void AbstractGameObjectTree::doDeleteSelected()
{
	QList<GameObjectItem*> List = selectedItems();
	doCancelSelection();
    while (!List.empty())
	{
		GameObjectItem* currentItem = List.front();
		if (currentItem == Q_NULLPTR)
		{
			return;
		}
		QList<GameObjectItem*> allitems = getAllChilds(currentItem);
		for (int k = 0; k < allitems.count(); k++)
		{
			List.removeAt(List.indexOf(allitems[k], 0));
		}
		setCurrentIndex(indexFromItem(currentItem));
		//如果没有父节点就直接删除
		if (currentItem->parent() == Q_NULLPTR)
		{
			delete takeTopLevelItem(currentIndex().row());
		}
		else
		{
			int count = currentItem->childCount();
			for (int j = 0; j < count; j++)
			{
				currentItem->removeChild(currentItem->child(0));
			}
			int i=currentIndex().row();
			GameObjectItem* item_to_del=(GameObjectItem*)currentItem->parent()->takeChild(currentIndex().row());
			QString str = item_to_del->text(0);
			delete item_to_del;
		}
		List.pop_front();
	}

	copiedItems.clear();

	auto func = [=](int i = 1) {return "123"; };
}

void AbstractGameObjectTree::createItemMenu()
{
	menu = new QMenu(this);
	/*m_SubMenu_Create = new QMenu("创建对象",this);
	m_Menu->setMinimumSize(QSize(120, 10));
	m_SubMenu_Create->setMinimumSize(QSize(120, 10));
	connect(m_Menu, SIGNAL(triggered(QAction *)),
		this, SLOT(doMenuActionHandler(QAction *)));
	connect(m_SubMenu_Create, SIGNAL(triggered(QAction *)),
		this, SLOT(SubMenu_Create_Triggered(QAction *)));

	m_Copy      = new QAction(tr("复制"), this);
	m_Delete    = new QAction(tr("删除"), this);
	m_Duplicate = new QAction(tr("创建副本"), this);
	m_Paste     = new QAction(tr("粘贴"), this);

	m_CreateButton=new QAction(tr("按钮"), this);
	m_CreateSprite= new QAction(tr("2D精灵"), this);
	m_CreateLabel= new QAction(tr("文本"), this);
	m_CreateTextBox = new QAction(tr("编辑框"), this);*/
}
//重构contextMenuEvent函数，记得#include <QContextMenuEvent>
void AbstractGameObjectTree::contextMenuEvent(QContextMenuEvent *event)
{
	menu->exec(QCursor::pos());
}
void AbstractGameObjectTree::doMenuActionHandler(QAction *action)
{
	
}
void AbstractGameObjectTree::copyChildNode(GameObjectItem * srcitem, GameObjectItem * destitem)
{
	GameObjectItem *item = srcitem;
	int childcount = item->childCount();

	for (int i = 0; i < childcount; i++)
	{
		//GameObjectItem *item = new GameObjectItem(destitem);
		item->setText(0, srcitem->child(i)->text(0));
		item->setFlags(GameObjectItemFlag);
		addTopLevelItem(item);
		copyChildNode((GameObjectItem*)srcitem->child(i),item);
	}
}
GameObjectItem* AbstractGameObjectTree::copyRootNode(GameObjectItem * srcitem)
{
	GameObjectItem *item = new GameObjectItem(((GameObjectItem*)srcitem)->GetObjectPtr());
	item->setFlags(GameObjectItemFlag);
	item->setText(0, srcitem->text(0));
	this->addTopLevelItem(item);
	return item;
}
void AbstractGameObjectTree::onCopyItems()
{
	QList<GameObjectItem*> List = selectedItems();
	if (List.count() == 0)
	{
		MyMessageBox("Info", "No Item Selected");
	}
	else
	{
		copiedItems = List;
	}
}
 void AbstractGameObjectTree::doCopySelectedItems()
{
	if (copiedItems.count() == 0)
	{
		MyMessageBox("Info", "No Item Selected");
		return;
	}
	QMap<GameObjectItem*, bool> map = QMap<GameObjectItem*, bool>();
	if (selectedItems().count() == 0)
	{
		int copyCount = copiedItems.count();
		if (copyCount > 1)
		{
			for (int i = 0; i < copiedItems.count(); i++)
			{
				map[copiedItems[i]] = true;
			}
			for (int i = 0; i < copiedItems.count(); i++)
			{
				if (map[copiedItems[i]] == false)
					continue;
				for (int j = i + 1; j < copiedItems.count(); j++)
				{
					if (map[copiedItems[j]] == false)
						continue;
					int relation = isParentChildRelation(copiedItems[i], copiedItems[j]);
					if (relation == 1)
					{
						map[copiedItems[j]] = false;
						break;
					}
					else if (relation == 2)
					{
						map[copiedItems[i]] = false;
						goto end1;
					}
				}
			end1: {}
			}
			for (auto iter = map.begin(); iter != map.end(); iter++)
			{
				if (iter.value() == true)
				{
					GameObjectItem* item = iter.key();
					GameObjectItem* newItem= static_cast<GameObjectItem*>(item->clone());
					if (item->parent() == NULL)
						addTopLevelItem(newItem);
				}
			}
		}
		else
		{
			GameObjectItem* item = copiedItems[0];
			GameObjectItem* newItem = static_cast<GameObjectItem*>(item->clone());
				addTopLevelItem(newItem);
		}
		expandAll();
	}
	else
	{
		int copyCount = copiedItems.count();
		if (copyCount > 1)
		{
			for (int i = 0; i < copiedItems.count(); i++)
			{
				map[copiedItems[i]] = true;
			}
			for (int i = 0; i < copiedItems.count(); i++)
			{
				if (map[copiedItems[i]] == false)
					continue;
				for (int j = i + 1; j < copiedItems.count(); j++)
				{
					if (map[copiedItems[j]] == false)
						continue;
					int relation = isParentChildRelation(copiedItems[i], copiedItems[j]);
					if (relation == 1)
					{
						map[copiedItems[j]] = false;
						break;
					}
					else if (relation == 2)
					{
						map[copiedItems[i]] = false;
						goto end2;
					}
				}
			end2://nothing to do
				{

				}
			}
			for (auto iter = map.begin(); iter != map.end(); iter++)
			{
				if (iter.value() == true)
				{
					GameObjectItem* item = iter.key();
					GameObjectItem* newItem = static_cast<GameObjectItem*>(item->clone());
					selectedItems()[0]->addChild(newItem);

				}
			}
		}
		else
		{
			GameObjectItem* item = copiedItems[0];
			GameObjectItem* newItem = static_cast<GameObjectItem*>(item->clone());
			selectedItems()[0]->addChild(newItem);
		}
		expandAll();
	}
}
QList<GameObjectItem*> AbstractGameObjectTree::getAllChilds(GameObjectItem * root)
{
	QList<GameObjectItem*> itemlist;
	QQueue<GameObjectItem*> itemqueue;
	itemqueue.push_back(root);
	while (!itemqueue.empty())
	{
		GameObjectItem* item=itemqueue.front();
		itemqueue.pop_front();
		for (int i = 0; i < item->childCount(); i++)
		{
			itemlist.push_back((item->child(i)));
			itemqueue.push_back(item->child(i));
		}
	}
	return itemlist;
}
void AbstractGameObjectTree::doCancelSelection()
{
	QList<GameObjectItem*> List = selectedItems();
	for (int i = 0; i < List.count(); i++)
	{
		List[i]->setSelected(false);
	}
	auto index = this->indexAt(QPoint(-1,-1));
	if (index.row() == -1)
	{
		setCurrentIndex(index);
	}
}
GameObjectItem * AbstractGameObjectTree::addTreeRoot(GameObject *obj)
{
	GameObjectItem * item = new GameObjectItem(obj);
	addTopLevelItem(item);
	QPushButton* topLevelButton = new QPushButton("Top");
	setItemWidget(item,1, topLevelButton);
	return item;
}
GameObjectItem *AbstractGameObjectTree::addTreeNode(GameObjectItem *&parent,GameObject *obj)
{
	GameObjectItem * item = new GameObjectItem(obj);
	parent->addChild(item);
	return item;
}
bool AbstractGameObjectTree::CheckNameExist(QString name)
{
	QTreeWidgetItemIterator *it_this = new QTreeWidgetItemIterator(this);

	while (**it_this)
	{
		if (name == (**it_this)->text(0))
		{
			return true;
		}
		(*it_this).operator++();
	}
	return false;
}

QString AbstractGameObjectTree::AllocateName(QString name)
{
	QString formated_str;

	int count_ptr = 1;

	if (!CheckNameExist(name))
	{
		return name;
	}
	else
	{
		formated_str = name + "(%1)";
		while (true)
		{
			if (!CheckNameExist(formated_str.arg(count_ptr++)))
				return formated_str.arg(count_ptr - 1);
		}
	}
}

QList<GameObjectItem*> AbstractGameObjectTree::selectedItems() const
{
	QList<GameObjectItem*> newlist;
	QList<QTreeWidgetItem*> oldlist=QTreeWidget::selectedItems();
	for (auto iter = oldlist.begin(); iter != oldlist.end(); iter++)
	{
		newlist.append((GameObjectItem*)iter.i->t());
	}
	return newlist;
}
