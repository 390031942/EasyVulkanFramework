#include "FileExplorerDemo.h"

FileExplorerDemo::FileExplorerDemo(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

    QFileSystemModel* model = new QFileSystemModel();
    model->setNameFilterDisables(false);
    model->setFilter(QDir::Dirs | QDir::Drives | QDir::Files | QDir::NoDotAndDotDot);
    QStringList list_name;
    list_name << "*.jpg" << "*.gif" << "*.png";
    //model->setNameFilters(list_name);
    model->setRootPath("C:/");
    ui.listView->setMovement(QListView::Movement::Static);
    ui.listView->setViewMode(QListView::IconMode);
    ui.listView->setGridSize(QSize(80, 80));
   
    ui.listView->setEditTriggers(QAbstractItemView::EditTrigger::AllEditTriggers);
    ui.listView->setModel(model);
    ui.listView->setRootIndex(model->setRootPath("C:/"));
    connect(ui.listView, SIGNAL(clicked(QModelIndex)), this, SLOT(pictureShow(QModelIndex)));
}


