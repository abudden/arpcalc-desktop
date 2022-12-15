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
#ifndef CHOICEWINDOW_H
#define CHOICEWINDOW_H

#include <QDialog>

class QDialogButtonBox;
class QListWidget;
class QListWidgetItem;

class ChoiceWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit ChoiceWindow(QWidget *parent = 0, QString title=""); //Constructor
		~ChoiceWindow(); // Destructor
		void selectItems(QStringList options, QStringList savedFields = QStringList());
		void handleKeyPress(QKeyEvent *e);

	signals:
		void itemSelected(QString value);
		void itemSelected(QString value, QStringList savedField);
	private slots:
		void itemPicked(QListWidgetItem *item);
		void cancelPressed();
	private:
		QListWidget *list;
		QDialogButtonBox *box;
		QStringList savedData;
};


#endif
