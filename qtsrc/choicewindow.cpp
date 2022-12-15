/*
 * ARPCalc - Al's Reverse Polish Calculator (C++ Version)
 * Copyright (C) 2022 A. S. Budden
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <QListWidget>
#include <QListWidgetItem>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QKeySequence>

#include "choicewindow.h"

ChoiceWindow::ChoiceWindow(QWidget *parent, QString title) :
	QDialog(parent)
{
	setWindowTitle(title);
	QVBoxLayout *layout = new QVBoxLayout(this);
	list = new QListWidget(this);
	layout->addWidget(list);
	box = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
	layout->addWidget(box);

	list->setSelectionMode(QAbstractItemView::SingleSelection);
	list->setStyleSheet(""
			"QListWidget {"
				"color: rgb(255, 255, 255);"
				"font-size: 10pt;"
			"}"
			);
	box->setStyleSheet(""
			"QPushButton {"
				"color: rgb(255, 255, 255);"
			"}"
			);

	connect(list, SIGNAL(itemClicked(QListWidgetItem*)), this,
				SLOT(itemPicked(QListWidgetItem*)));
	connect(box, SIGNAL(rejected()), this, SLOT(cancelPressed()));

	this->setAttribute(Qt::WA_DeleteOnClose);

	setWindowModality(Qt::ApplicationModal);
}

ChoiceWindow::~ChoiceWindow()
{
}

void ChoiceWindow::selectItems(QStringList options, QStringList savedField)
{
	savedData = savedField;
	list->clear();
	list->addItems(options);

	for (int i=0 ; i < 26; i++) {
		QListWidgetItem *item = list->item(i);
		if (item == NULL) {
			break;
		}
		item->setText(item->text() + QString(" (%1)").arg(QChar('A' + i)));
	}

	// Make it twice as high as default
	this->adjustSize();
	QSize size = this->size();
	resize(size.width(), size.height()*2);
}

void ChoiceWindow::itemPicked(QListWidgetItem *item)
{
	QString result = item->text();
	if (result.contains(" (")) {
		result = result.split(" (")[0];
	}

	list->clear();

	reject();
	close();

	if (savedData.isEmpty()) {
		emit itemSelected(result);
	}
	else {
		emit itemSelected(result, savedData);
	}
}

void ChoiceWindow::cancelPressed()
{
	list->clear();
	reject();
	close();
}

void ChoiceWindow::handleKeyPress(QKeyEvent *e)
{
	int selectedIndex = -1;
	if ((e->key() >= Qt::Key_A) && (e->key() <= Qt::Key_Z)) {
		selectedIndex = e->key() - Qt::Key_A;
	}
	else if (e->key() == Qt::Key_Escape) {
		cancelPressed();
		e->accept();
		return;
	}

	if (selectedIndex >= 0) {
		QListWidgetItem *item = list->item(selectedIndex);
		if (item != NULL) {
			itemPicked(item);
		}
		e->accept();
	}
	else {
		QDialog::keyPressEvent(e);
	}
}
