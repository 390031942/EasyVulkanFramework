#include "FileLabelEdit.h"

FileLabelEdit::FileLabelEdit(QWidget *parent)
	: QLineEdit(parent)
{
	installEventFilter(this);
	setObjectName("filelabeledit");
	setAlignment(Qt::AlignCenter);

	setStyleSheet("color:black");
}

FileLabelEdit::~FileLabelEdit()
{
}

