#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SimpleGameEditor.h"

class SimpleGameEditor : public QMainWindow
{
    Q_OBJECT

public:
    SimpleGameEditor(QWidget *parent = Q_NULLPTR);

private:
    Ui::SimpleGameEditorClass ui;
};
