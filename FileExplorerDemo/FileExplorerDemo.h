#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FileExplorerDemo.h"
#include <QFileSystemModel>
#include <QDebug>
#include <QListView>
class FileExplorerDemo : public QMainWindow
{
	Q_OBJECT

public:
	FileExplorerDemo(QWidget *parent = Q_NULLPTR);

    void pictureShow(const QModelIndex& index)
    {
        qDebug("clicked");

        QString filepath = index.data(Qt::DisplayRole).toString();
        QStringList str;
        str.clear();
        //str.append("/mnt/yaffs/");
        str.append("/mnt/xuechao/src/");
        str.append(filepath);
        filepath = str.join("");
        qDebug() << "filepath" << filepath;
        str.clear();
        //ui->label->setPixmap(QPixmap(filepath));
        //ui->label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        // QVBoxLayout *layout;
         //layout= new QVBoxLayout(ui->label);
    }
private:
	Ui::FileExplorerDemoClass ui;
};
