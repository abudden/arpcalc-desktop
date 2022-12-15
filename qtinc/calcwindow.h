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
#ifndef CALCWINDOW_H
#define CALCWINDOW_H

#include <QWidget>
#include <QTime>

#include "commands.h"

#include "ui_qcalcwindow.h"

class ClickableLabel;
class QSettings;
class QNetworkReply;
class ChoiceWindow;

class CalcWindow : public QWidget
{
	Q_OBJECT

	public:
		explicit CalcWindow(QWidget *parent = 0); //Constructor
		~CalcWindow(); // Destructor
	protected:
		bool eventFilter(QObject *object, QEvent *e);

	public slots:
		void conversionSelected(QString to, QStringList category_from);
		void selectToUnit(QString category, QStringList from);
		void selectFromUnit(QString category);
		void constantSelected(QString constant);
		void selectConstant(QString category);
		void removeChoiceWindow(int x);

	private slots:
		void buttonPressHandler();
		void optionClicked();
		void toOptPad1();
		void toOptPad2();
		void clearToast();
		void gotCurrencyData(QNetworkReply *reply);
		void copyVersionInfo();
		void copyAsValue();
		void copyFormatted();
		void copyUnicode();
		void pasteValue();

	private:
		Ui_qcalcwindow ui;
		void updateDisplays();
		void setupGrid(QString grid);
		void applyStyles();
		void populateKeyMaps();
		bool handleKeyPress(Qt::Key key, QString modifiers);
		void handleErrorCode(ErrorCode result);

		void buttonPress(QString command);

		void createContextMenu();

		void saveSettings();
		void restoreSettings();
		void createSettings();
		void saveStack();
		void restoreStack();
		void storePressed();
		void recallPressed();
		void tabSelect(QString tab);
		void showOptPad1();
		void showOptPad2();
		void optionClicked(QString name);
		void optionHelp(QString name);
		void updateOptionIcons();
		void createOptPad();

		void moreConversions();

		void constByName();

		void toggleHypMode();

		void setupWindowSize();

		void cancelPressed();
		void showToast(QString message, bool isHelp=false);
		void getCurrencyData();
		QString unzipByteArray(QByteArray data);

		QList<QList<ClickableLabel *> > buttonArray;
		QList<QHBoxLayout *>  layoutArray;
		CommandHandler calc;

		bool returnToNumPad = true;
		bool hypMode = false;
		QString storeMode = "None";
		QString lastTab = "numpad";
		QString lastStoreTab = "romanupperpad";
		QString sizeName = "Large";
		QString btnFontSize;
		QString entryFontSize;
		QString toastFontSize;
		QString btnBorderSize;

		QHash<QString, QPushButton *> optIcons;

		QHash<QString, Option> getOptions();

		void nextTab();
		QSettings *settings;

		QString currentToast;
		QLabel *toastLabel = NULL;
		QList<QString> toastMessages;

		const int toast_duration_msecs = 2000;

		void showToastImpl();

		ChoiceWindow * newChoiceWindow(QString name, QStringList items, QStringList savedFields = QStringList());
		QMap<int, ChoiceWindow *> diags;
};

#endif // CALCWINDOW_H
