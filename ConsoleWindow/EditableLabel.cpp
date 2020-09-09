#include "EditableLabel.h"

QEditLabel::QEditLabel(QWidget* parent)
	: QWidget(parent) {

	initCtrls();
}

bool QEditLabel::eventFilter(QObject* obj, QEvent* e) {
	if (obj == m_label) {
		if (QEvent::FocusIn == e->type()) {
			m_stackWidget->setCurrentWidget(m_lineEdit);
		}
	}
	else if (obj == m_lineEdit) {
		if (QEvent::FocusOut == e->type()) {
			m_stackWidget->setCurrentWidget(m_label);
		}
	}
	return QWidget::eventFilter(obj, e);
}

void QEditLabel::initCtrls(void) {
	m_stackWidget = new QStackedWidget(this);

	m_label = new QLabel("test");
	m_label->setStyleSheet("background-color:white");

	m_lineEdit = new QLineEdit();

	m_label->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

	m_stackWidget->setGeometry(0, 0, 50, 50);

	m_label->installEventFilter(this);
	m_lineEdit->installEventFilter(this);

	m_stackWidget->addWidget(m_label);
	m_stackWidget->addWidget(m_lineEdit);

	m_stackWidget->setCurrentWidget(m_label);
}