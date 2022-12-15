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
#include <iostream> // For extra debugging with cerr
#include <QtWidgets>
#include <QDebug>
#include <QtNetwork>
#include <cmath>
#include <zip.h>
#include "calcwindow.h"
#include "choicewindow.h"
#include "clickablelabel.h"

extern const char * changeset;
extern const char * version;
extern const char * builddate;

// Constructor for main widget
CalcWindow::CalcWindow(QWidget *parent) :
	QWidget(parent)
{
	ui.setupUi(this);
	setFocusPolicy(Qt::StrongFocus);

	ui.stkButtonPads->setCurrentIndex(0);

	QList<QList<ClickableLabel *> > b({
		{ui.b00, ui.b01, ui.b02, ui.b03, ui.b04, ui.b05},
		{ui.b10, ui.b11, ui.b12, ui.b13, ui.b14, ui.b15},
		{ui.b20, ui.b21, ui.b22, ui.b23, ui.b24, ui.b25},
		{ui.b30, ui.b31, ui.b32, ui.b33, ui.b34, ui.b35},
		{ui.b40, ui.b41, ui.b42, ui.b43, ui.b44, ui.b45},
		{ui.b50, ui.b51, ui.b52, ui.b53, ui.b54, ui.b55}
	});
	buttonArray = b;

	QList<QHBoxLayout *>  l{
		ui.lytRow0, ui.lytRow1, ui.lytRow2, ui.lytRow3, ui.lytRow4, ui.lytRow5
	};
	layoutArray = l;

	for (int r=0;r<6;r++) {
		for (int c=0;c<6;c++) {
			connect(buttonArray[r][c], SIGNAL(clicked()), this, SLOT(buttonPressHandler()));
		}
	}

	connect(ui.btnNumPad,   &QPushButton::clicked, this, [this]{ tabSelect("numpad"); });
	connect(ui.btnFuncPad,  &QPushButton::clicked, this, [this]{ tabSelect("funcpad"); });
	connect(ui.btnConvPad,  &QPushButton::clicked, this, [this]{ tabSelect("convpad"); });
	connect(ui.btnConstPad, &QPushButton::clicked, this, [this]{ tabSelect("constpad"); });
	connect(ui.btnOptPad,   &QPushButton::clicked, this, [this]{ tabSelect("optpad1"); });

	QString changeset_s = QString(changeset);
	QString version_s = QString(version);
	if (changeset_s.endsWith("+")) {
		version_s += " (modified)";
	}
	ui.lblStatusExponent->setToolTip(QString(builddate));
	ui.lblStatusAngular->setToolTip(changeset_s);
	ui.lblStatusBase->setToolTip(version_s);

	createContextMenu();

	createSettings();
	restoreSettings();
	restoreStack();

	setupWindowSize();

	createOptPad();
	tabSelect("optpad1");
	adjustSize();
	cancelPressed();

	//setupGrid("numpad");
	updateDisplays();

	/*
	if (calc.getOption(ShowHelpOnStart)) {
		showToast("Long-press any key for help");
	}
	*/
	getCurrencyData();

	this->layout()->setSizeConstraint(QLayout::SetFixedSize);
	setWindowFlags(windowFlags() &(~Qt::WindowMaximizeButtonHint));

	qApp->installEventFilter(this);
}

// Destructor
CalcWindow::~CalcWindow()
{
	saveSettings();
	saveStack();
	delete settings;
}

void CalcWindow::createContextMenu()
{
	QAction *copyValueAct = new QAction("Copy Value", this);
	copyValueAct->setStatusTip("Copy to the clipboard in a format that can be understood by other applications (Ctrl-C)");
	connect(copyValueAct, &QAction::triggered, this, &CalcWindow::copyAsValue);

	QAction *copyFormattedAct = new QAction("Copy Formatted", this);
	copyFormattedAct->setStatusTip("Copy to the clipboard as formatted text");
	connect(copyFormattedAct, &QAction::triggered, this, &CalcWindow::copyFormatted);

	QAction *copyUnicodeAct = new QAction("Copy Unicode", this);
	copyUnicodeAct->setStatusTip("Copy to the clipboard as unicode text");
	connect(copyUnicodeAct, &QAction::triggered, this, &CalcWindow::copyUnicode);

	QAction *pasteValueAct = new QAction("Paste Value", this);
	pasteValueAct->setStatusTip("Paste value from clipboard if possible (Ctrl-V)");
	connect(pasteValueAct, &QAction::triggered, this, &CalcWindow::pasteValue);

	ui.lblEntry->addAction(copyValueAct);
	ui.lblEntry->addAction(copyFormattedAct);
	ui.lblEntry->addAction(copyUnicodeAct);
	ui.lblEntry->addAction(pasteValueAct);

	ui.lblEntry->setContextMenuPolicy(Qt::ActionsContextMenu);

	QAction *copyVersionAct = new QAction("Copy version information", this);
	copyVersionAct->setStatusTip("Copy the the version information to the clipboard - go to https://www.cgtk.co.uk/contact to contact the developer.");
	connect(copyVersionAct, &QAction::triggered, this, &CalcWindow::copyVersionInfo);

	ui.lblStatusBase->addAction(copyVersionAct);
	ui.lblStatusBase->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void CalcWindow::copyVersionInfo()
{
	QClipboard *clipboard = QGuiApplication::clipboard();
	QString version_info = QString(version);
	version_info += "\n";
	version_info += QString(builddate);

	clipboard->setText(version_info);
}

void CalcWindow::copyAsValue()
{
	QClipboard *clipboard = QGuiApplication::clipboard();

	AF value = calc.getXValue();

	QString valueStr = QString::fromStdString(value.toString());

	clipboard->setText(valueStr);
}

void CalcWindow::copyFormatted()
{
	QClipboard *clipboard = QGuiApplication::clipboard();

	QString valueStr = QString::fromStdString(calc.getXDisplay());
	valueStr = valueStr.replace("<small>", "").replace("</small>", "");

	QMimeData *mime = new QMimeData();
	mime->setHtml(valueStr);

	clipboard->setMimeData(mime);
}

void CalcWindow::copyUnicode()
{
	// TODO: Consider moving the unicode conversion code into the commands.cpp
	QClipboard *clipboard = QGuiApplication::clipboard();

	QString valueStr = QString::fromStdString(calc.getXDisplay());

	valueStr = valueStr.replace("&ndash;", "-");

	QRegularExpression supre("(.*)<sup>(<small>)?(.*)(</small>)?</sup>");
	const QMap<QChar, QString> superscript = {
		{'-', QString::fromUtf8("\u207b")},
		{'0', QString::fromUtf8("\u2070")},
		{'1', QString::fromUtf8("\u00b9")},
		{'2', QString::fromUtf8("\u00b2")},
		{'3', QString::fromUtf8("\u00b3")},
		{'4', QString::fromUtf8("\u2074")},
		{'5', QString::fromUtf8("\u2075")},
		{'6', QString::fromUtf8("\u2076")},
		{'7', QString::fromUtf8("\u2077")},
		{'8', QString::fromUtf8("\u2078")},
		{'9', QString::fromUtf8("\u2079")},
	};

	QRegularExpressionMatch m = supre.match(valueStr);
	if (m.hasMatch()) {
		valueStr  = m.captured(1);
		for (QChar c : m.captured(3)) {
			valueStr.append(superscript.value(c, QString(c)));
		}
	}

	QString nbsp = QString::fromUtf8("\u00A0");
	QString mu = QString::fromUtf8("\u03BC");
	QString times = QString::fromUtf8("\u00D7");

	valueStr = valueStr.replace("&nbsp;", nbsp);
	valueStr = valueStr.replace("&mu;", mu);
	valueStr = valueStr.replace("&times;", times);

	clipboard->setText(valueStr);
}

void CalcWindow::pasteValue()
{
	QClipboard *clipboard = QGuiApplication::clipboard();
	QString clipboardText = clipboard->text();

	qDebug() << "Trying to paste '" << clipboardText << "'";

	std::string s = clipboardText.toStdString();
	if (AF::isValidString(s)) {
		calc.st.push(AF(s));
	}
	else {
		showToast("Cannot parse pasted value");
	}

	updateDisplays();
}

void CalcWindow::getCurrencyData()
{
	QString url = "https://www.ecb.int/stats/eurofxref/eurofxref.zip";
	QNetworkAccessManager *manager = new QNetworkAccessManager;
	manager->get(QNetworkRequest(QUrl(url)));
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotCurrencyData(QNetworkReply*)));
}

QString CalcWindow::unzipByteArray(QByteArray data)
{
	const char *buffer = data.constData();

	zip_error_t err;

	zip_source_t *zsource = zip_source_buffer_create(buffer, data.size(),
			0, &err);
	if (zsource == NULL) {
		qDebug() << "zip source not initialised";
		return "";
	}

	zip *z = zip_open_from_source(zsource, ZIP_RDONLY, &err);
	if (z == NULL) {
		qDebug() << "zip not opened";
		return "";
	}

	struct zip_stat st;
	zip_stat_init(&st);
	int e =zip_stat_index(z, 0, 0, &st);
	if (e != 0) {
		qDebug() << "No files found in zip";
		return "";
	}

	char *contents = new char[st.size];

	zip_file *f = zip_fopen_index(z, 0, 0);
	if (f == NULL) {
		qDebug() << "Couldn't open first file in archive";
		return "";
	}
	int bytes_read = zip_fread(f, contents, st.size);
	if (bytes_read <= 0) {
		qDebug() << "Reading uncompressed data failed";
		return "";
	}
	e = zip_fclose(f);
	if (e != 0) {
		qDebug() << "Couldn't close file";
		return "";
	}

	zip_discard(z);
	QString result(contents);
	std::destroy_at(contents);
	return result;
}

void CalcWindow::gotCurrencyData(QNetworkReply *reply)
{
	if (reply->error() != QNetworkReply::NoError) {
		qDebug() << "Couldn't download URL";
		return;
	}
	QByteArray data = reply->readAll();
	qDebug() << "Got data; data length = " << data.size();

	QString unzipped = unzipByteArray(data);

	if (unzipped.isEmpty()) {
		qDebug() << "Couldn't unzip data";
		return;
	}

	qDebug() << unzipped;

	QStringList lines = unzipped.split("\n");
	QList<QStringList> rows;
	for ( auto line : lines ) {
		if (line.trimmed().isEmpty()) {
			continue;
		}
		rows.append(line.split(","));
	}

	std::map<std::string, double> wrtEuro;
	QString datestr;

	for (int i=0;i<rows[0].size();i++) {
		if (rows[0][i].trimmed() == "Date") {
			datestr = rows[1][i].trimmed();
		}
		else if (rows[0][i].trimmed().isEmpty()) {
		}
		else {
			wrtEuro[rows[0][i].trimmed().toStdString()] = rows[1][i].trimmed().toDouble();
		}
	}

	// Might need to change to "dd MMMM yyyy"
	QDate date = QDate::fromString(datestr, "d MMMM yyyy");
	datestr = date.toString("yyyy-MM-dd");

#if 0
	qDebug() << "Parsed currency data:";
	for (auto const& [key, val] : wrtEuro) {
		qDebug() << QString::fromStdString(key) << val;
	}
	qDebug() << "Date was" << datestr;
#endif

	QString datecheck = QString::fromStdString(
			calc.processCurrencyData(wrtEuro, datestr.toStdString())
			);
	if ( ! datecheck.isEmpty() ) {
		showToast(datecheck);
	}
}

bool CalcWindow::eventFilter(QObject *object, QEvent *e)
{
	(void) object;
	bool handled = false;

	if (e->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);

		if (diags.size() > 0) {
			// Dialog boxes present - pass the key onto the dialog boxes
			for (auto *diag : diags) {
				diag->handleKeyPress(keyEvent);

				if (keyEvent->isAccepted()) {
					return true;
				}
			}
		}

		//qDebug() << "Object name: " << object->objectName()
		//	<< object->metaObject()->className();

		Qt::Key k = (Qt::Key) keyEvent->key();
		Qt::KeyboardModifiers m = keyEvent->modifiers();
		//qDebug() << "Pressed" << k << ": " << QKeySequence(k).toString() << "; with modifiers:"
		//	<< Qt::hex << m;

		QString modifiers = "";
		if ((m & Qt::ControlModifier) != 0) {
			modifiers += "Ctrl";
		}
		if ((m & Qt::ShiftModifier) != 0) {
			modifiers += "Shift";
		}
		if ((m & Qt::AltModifier) != 0) {
			modifiers += "Alt";
		}

		if (modifiers.length() == 0) {
			modifiers = "plain";
		}
		else {
			modifiers = modifiers.left(1).toLower() + modifiers.mid(1);
		}

		handled = handleKeyPress(k, modifiers);
	}
	return handled;
}

bool CalcWindow::handleKeyPress(Qt::Key key, QString modifiers)
{
	std::string std_modifiers = modifiers.toStdString();
	std::string keyname = QKeySequence(key).toString().toStdString();
	ErrorCode ec = NoError;

	if (key == ((Qt::Key) 8364)) {
		keyname = "euro";
	}
	else if (key == Qt::Key_sterling) {
		keyname = "sterling";
	}
	else {
	}

	//qDebug() << "Running key handler for" << QString::fromStdString(keyname)
	//	<< QString::fromStdString(std_modifiers) << lastTab;
	keyHandlerResult result = calc.handleKey(keyname, std_modifiers, lastTab.toStdString(), ec);

	//qDebug() << "Result:" << result;

	switch (result) {
		default:
		case Key_NotHandled:
			return false;
		case Key_Handled:
			if (returnToNumPad) {
				tabSelect("numpad");
			}
			else {
				tabSelect(lastTab);
			}
			break;
		case Key_Cancel:
			cancelPressed();
			break;
		case Key_NextTab:
			nextTab();
			break;
		case Key_MoreConversions:
			moreConversions();
			break;
		case Key_ConstByName:
			constByName();
			break;
		case Key_SI:
			buttonPress("SI");
			break;
		case Key_Store:
			storePressed();
			break;
		case Key_Recall:
			recallPressed();
			break;
		case Key_CopyToClipboard:
			copyAsValue();
			break;
		case Key_PasteFromClipboard:
			pasteValue();
			break;
		case Key_Quit:
			close();
			break;
	}

	handleErrorCode(ec);

	updateDisplays();
	return true;
}

void CalcWindow::setupWindowSize()
{
	sizeName = QString::fromStdString(calc.getSizeName());

	int iconSize;
	if (sizeName == "Large") {
		iconSize = 100;
		btnFontSize = "font-size: 12pt;";
		entryFontSize = "font-size: 20pt;";
		toastFontSize = "font-size: 16pt;";
		btnBorderSize = "border: 2px solid #ffffff;";
	}
	else if (sizeName == "Medium") {
		iconSize = 90;
		btnFontSize = "font-size: 10pt;";
		entryFontSize = "font-size: 16pt;";
		toastFontSize = "font-size: 12pt;";
		btnBorderSize = "border: 1px solid #ffffff;";
	}
	else {
		iconSize = 80;
		btnFontSize = "font-size: 8pt;";
		entryFontSize = "font-size: 14pt;";
		toastFontSize = "font-size: 10pt;";
		btnBorderSize = "border: 1px solid #ffffff;";
	}
	ui.btnNumPad->setIconSize(QSize(iconSize, iconSize));
	ui.btnFuncPad->setIconSize(QSize(iconSize, iconSize));
	ui.btnConvPad->setIconSize(QSize(iconSize, iconSize));
	ui.btnConstPad->setIconSize(QSize(iconSize, iconSize));
	ui.btnOptPad->setIconSize(QSize(iconSize, iconSize));
}

void CalcWindow::applyStyles()
{
	static QString button_style = ""
		"QLabel {"
			"color: rgb(255, 255, 255);"
			"background-color: rgb(57, 56, 58);"
			+ btnBorderSize +
			"border-radius: 5px;"
			+ btnFontSize +
		"}"
		;
	static QString entry_style = ""
		"QLabel {"
			"color: rgb(255, 255, 255);"
			+ entryFontSize +
			"padding: 5px;"
		"}"
		"QMenu {"
			"color: rgb(255, 255, 255);"
		"}"
		"QMenu::item:selected {"
			"background-color: rgb(133, 133, 133);"
		"}"
		;
	static QString stackbase_style = ""
		"QLabel {"
			"color: rgb(255, 255, 255);"
			+ btnFontSize +
		"}"
		;
	static QString status_style = ""
		"QLabel {"
			"color: rgb(255, 255, 255);"
			+ btnFontSize +
		"}"
		"QMenu {"
			"color: rgb(255, 255, 255);"
		"}"
		"QMenu::item:selected {"
			"background-color: rgb(133, 133, 133);"
		"}"
		;
	static QString tab_button_style = ""
		"padding: 0px;"
		;

	for (int r=0;r<6;r++) {
		for (int c=0;c<6;c++) {
			ClickableLabel *btn = buttonArray[r][c];
			btn->setTextFormat(Qt::RichText);
			btn->setStyleSheet(button_style);
			btn->setAlignment(Qt::AlignCenter);
		}
	}

	ui.lblEntry->setTextFormat(Qt::RichText);
	ui.lblEntry->setStyleSheet(entry_style);

	ui.lblStack->setTextFormat(Qt::RichText);
	ui.lblStack->setStyleSheet(stackbase_style);

	ui.lblEntry->setTextFormat(Qt::RichText);
	ui.lblBase->setStyleSheet(stackbase_style);

	ui.lblStatusExponent->setStyleSheet(status_style);
	ui.lblStatusAngular->setStyleSheet(status_style);
	ui.lblStatusBase->setStyleSheet(status_style);

	ui.btnNumPad->setStyleSheet(tab_button_style);
	ui.btnFuncPad->setStyleSheet(tab_button_style);
	ui.btnConvPad->setStyleSheet(tab_button_style);
	ui.btnConstPad->setStyleSheet(tab_button_style);
	ui.btnOptPad->setStyleSheet(tab_button_style);

	const QMargins margins(0, 0, 0, 0);
	ui.lytTabButtons->setContentsMargins(margins);
	ui.btnNumPad->setContentsMargins(margins);
	ui.btnFuncPad->setContentsMargins(margins);
	ui.btnConvPad->setContentsMargins(margins);
	ui.btnConstPad->setContentsMargins(margins);
	ui.btnOptPad->setContentsMargins(margins);
}

void CalcWindow::cancelPressed()
{
	storeMode = "None"; // Not really necessary as done on every button press
	tabSelect("numpad");
}


void CalcWindow::storePressed()
{
	storeMode = "store";
	tabSelect(lastStoreTab);
	showToast("Please select location");
}

void CalcWindow::recallPressed()
{
	storeMode = "recall";
	tabSelect(lastStoreTab);
	showToast("Please select location");
}

void CalcWindow::createSettings()
{
	QFileInfo exeDir(QCoreApplication::applicationDirPath());
	if (exeDir.isDir() && exeDir.isWritable()) {
		QDir dir = QCoreApplication::applicationDirPath();
		settings = new QSettings(dir.filePath("settings.ini"),
				QSettings::IniFormat);
	}
	else {
		settings = new QSettings("cgtk.co.uk", "ARPCalc");
	}
}

void CalcWindow::restoreSettings()
{
	// TODO: consider moving this into the commandhandler and working with
	// strings only for settings
	calc.setDefaultOptions();

	for (auto & name : calc.getOptionNames()) {
		QString qsName = QString::fromStdString(name);
		if (settings->contains(qsName)) {
			calc.setOptionByName(name, settings->value(qsName).toBool());
		}
	}

	if (settings->contains("DecimalPlaces")) {
		calc.setPlaces(settings->value("DecimalPlaces").toInt());
	}

	if (calc.getOption(SaveBaseOnExit)) {
		if (settings->contains("SavedBase")) {
			calc.setStatusBase(settings->value("SavedBase").toString().toStdString());
		}
	}

	if (settings->contains("WindowSize")) {
		calc.setSizeName(settings->value("WindowSize").toString().toStdString());
	}

	QStringList allkeys = settings->allKeys();

	QString key;
	// Use std::map to save converting later
	std::map<std::string, AF> vMap;
	std::map<std::string, double> cMap;
	foreach (key, allkeys) {
		//qDebug() << "Setting" << key << "value" << settings->value(key);
		if (key.startsWith("VARSTORE-")) {
			QString value = settings->value(key, "0.0").toString();
			vMap[key.mid(9).toStdString()] = AF(value.toStdString());
		}
		else if (key.startsWith("CURRENCY-")) {
			double value = settings->value(key, 0.0).toDouble();
			cMap[key.mid(9).toStdString()] = value;
		}
		else if (key == "CURRENCYDATE") {
			calc.last_currency_date = settings->value(key, "").toString().toStdString();
		}
		else {
		}
	}
	calc.loadVarStore(vMap);
	calc.st.registerCurrencies(cMap);

	if (settings->contains("BitCount")) {
		int bc = settings->value("BitCount").toInt();
		switch (bc) {
			case 8: calc.setBitCount(bc8); break;
			case 16: calc.setBitCount(bc16); break;
			default:
			case 32: calc.setBitCount(bc32); break;
			case 64: calc.setBitCount(bc64); break;
		}
	}
}

void CalcWindow::saveSettings()
{
	std::vector<std::string> optionNames = calc.getOptionNames();
	for (auto & name : optionNames) {
		settings->setValue(QString::fromStdString(name), calc.getOptionByName(name));
	}

	settings->setValue("DecimalPlaces", calc.getPlaces());

	if (calc.getOption(SaveBaseOnExit)) {
		settings->setValue("SavedBase", QString::fromStdString(calc.getStatusBase()));
	}
	else {
		settings->setValue("SavedBase", "DEC");
	}

	settings->setValue("WindowSize", QString::fromStdString(calc.getSizeName()));

	if (calc.varStoreHasChanged()) {
		for (auto & [key, value] : calc.varStore) {
			settings->setValue("VARSTORE-" + QString::fromStdString(key),
					QString::fromStdString(value.toString()));
		}
	}

	std::map<std::string, double> rawCurrencyData = calc.getRawCurrencyData();
	for (const auto & [name, value] : rawCurrencyData) {
		QString cKey = "CURRENCY-" + QString::fromStdString(name);
		settings->setValue(cKey, value);
	}
	settings->setValue("CURRENCYDATE", QString::fromStdString(calc.last_currency_date));

	switch (calc.getBitCount()) {
		case bc8: settings->setValue("BitCount", 8); break;
		case bc16: settings->setValue("BitCount", 16); break;
		default:
		case bc32: settings->setValue("BitCount", 32); break;
		case bc64: settings->setValue("BitCount", 64); break;
	}
}

void CalcWindow::saveStack()
{
	std::vector<AF> storeStack;
	if (calc.getOption(SaveStackOnExit)) {
		// Abusing encapsulation here a lot - not the best way to do it!
		storeStack = calc.st.stack;
	}

	settings->setValue("SavedStackLength", (qulonglong) storeStack.size());
	for (qlonglong i=0;i<(int)storeStack.size();i++) {
		settings->setValue(QString("SavedStack-%1").arg((qlonglong) i,
					(int) 3, (int) 10, (QChar) '0'),
				QString::fromStdString(storeStack[i].toString()));
	}
}

void CalcWindow::restoreStack()
{
	qlonglong stackSize = settings->value("SavedStackLength", 0).toLongLong();
	for (qlonglong i=0;i<stackSize;i++) {
		QString value = settings->value(QString("SavedStack-%1").arg((qlonglong) i,
					(int) 3, (int) 10, (QChar) '0')).toString();
		AF v(value.toStdString());
		// Bypass history stuff by pushing directly
		calc.st.push(v);
	}
}

void CalcWindow::clearToast()
{
	if (currentToast.isEmpty()) {
		return;
	}

	if (toastMessages.size() > 0) {
		currentToast = toastMessages.front();
		toastMessages.pop_front();

		showToastImpl();

		QTimer::singleShot(toast_duration_msecs, this, SLOT(clearToast()));
	}
	else {
		currentToast.clear();
		toastLabel->setText("");
		toastLabel->setVisible(false);
	}
}

void CalcWindow::showToastImpl()
{
	toastLabel->setText(currentToast);

	QSize sw = size();
	toastLabel->adjustSize();
	QSize sl = toastLabel->size();
	toastLabel->setGeometry(
			(sw.width() - sl.width()) / 2,
			(sw.height() - sl.height()) / 2,
			sl.width(),
			sl.height()
			);

	toastLabel->setVisible(true);
}

void CalcWindow::showToast(QString message, bool isHelp)
{
	static QString label_style = ""
		"QLabel {"
			"background-color: rgba(0,0,0,0%);"
			"color: rgb(255, 255, 255);"
			"background-color: rgb(57, 56, 58);"
			"border: 1px solid #ffffff;"
			"border-radius: 5px;"
			+ toastFontSize +
			"padding: 10px;"
		"}"
		;
	(void) isHelp;

	if (toastLabel == NULL) {
		toastLabel = new QLabel(this);
		toastLabel->setFrameShape(QFrame::Panel);
		toastLabel->setFrameShadow(QFrame::Sunken);
		toastLabel->setAlignment(Qt::AlignCenter);
		toastLabel->setText("");
		toastLabel->setTextFormat(Qt::RichText);
		toastLabel->setStyleSheet(label_style);
		toastLabel->setVisible(false);
	}

	if (currentToast.isEmpty()) {
		currentToast = message;
		showToastImpl();
		QTimer::singleShot(toast_duration_msecs, this, SLOT(clearToast()));
	}
	else {
		toastMessages.append(message);
	}
}

void CalcWindow::conversionSelected(QString to, QStringList category_from)
{
	QString category = category_from[0];
	QString from = category_from[1];

	qDebug() << "Selected " << category << " and " << from << " to " << to;

	buttonPress("Convert_" + category + "_" + from + "_" + to);
}

void CalcWindow::removeChoiceWindow(int x)
{
	(void) x;
	ChoiceWindow *diag = qobject_cast<ChoiceWindow*>(sender());
	int index = diag->property("listIndex").toInt();
	if (diags.contains(index)) {
		diags.remove(index);
	}
}

ChoiceWindow * CalcWindow::newChoiceWindow(QString name, QStringList items, QStringList savedFields)
{
	QList<int> existing_keys = diags.keys();
	int max = 0;
	for (int i : existing_keys) {
		if (i > max) {
			max = i;
		}
	}

	ChoiceWindow *diag = new ChoiceWindow(this, name);
	diag->setProperty("listIndex", max+1);
	diags[max+1] = diag;

	connect(diag, SIGNAL(finished(int)), this, SLOT(removeChoiceWindow(int)));
	diag->selectItems(items, savedFields);

	return diag;
}

void CalcWindow::selectToUnit(QString from, QStringList category)
{
	QStringList items;
	QStringList savedFields;
	savedFields << category[0];
	savedFields << from;

	std::set<std::string> unitList = calc.st.getAvailableUnits(
			category[0].toStdString());
	for (auto &u: unitList) {
		items << QString::fromStdString(u);
	}
	items.sort();

	ChoiceWindow * diag = newChoiceWindow("Convert " + from + " to:", items, savedFields);
	connect(diag, SIGNAL(itemSelected(QString, QStringList)), this, SLOT(conversionSelected(QString, QStringList)));
	diag->show();
}

void CalcWindow::selectFromUnit(QString category)
{
	QStringList items;
	QStringList savedFields;
	savedFields << category;

	std::set<std::string> unitList = calc.st.getAvailableUnits(
			category.toStdString());
	for (auto &u: unitList) {
		items << QString::fromStdString(u);
	}
	items.sort();

	ChoiceWindow *diag = newChoiceWindow("Convert from:", items, savedFields);
	connect(diag, SIGNAL(itemSelected(QString, QStringList)), this, SLOT(selectToUnit(QString, QStringList)));
	diag->show();
}

void CalcWindow::moreConversions()
{
	QStringList items;
	std::vector<std::string> categories = calc.st.getConversionCategories();

	for (const auto &i : categories) {
		items << QString::fromStdString(i);
	}

	ChoiceWindow *diag = newChoiceWindow("Select Category", items);
	connect(diag, SIGNAL(itemSelected(QString)), this, SLOT(selectFromUnit(QString)));
	diag->show();
}

void CalcWindow::constantSelected(QString constant)
{
	buttonPress("Const-" + constant);
}

void CalcWindow::selectConstant(QString category)
{
	QStringList items;

	for ( const auto &c : calc.st.constants) {
		QString thisCategory = QString::fromStdString(c.category);
		QString name = QString::fromStdString(c.name);
		if (category == thisCategory) {
			items.append(name);
		}
	}

	ChoiceWindow *diag = newChoiceWindow("Select Constant", items);
	connect(diag, SIGNAL(itemSelected(QString)), this, SLOT(constantSelected(QString)));
	diag->show();
}

void CalcWindow::constByName()
{
	QStringList items;
	for ( const auto &c : calc.st.constants) {
		QString category = QString::fromStdString(c.category);
		if ( ! items.contains(category)) {
			items.append(category);
		}
	}

	ChoiceWindow *diag = newChoiceWindow("Select Category", items);
	connect(diag, SIGNAL(itemSelected(QString)), this, SLOT(selectConstant(QString)));
	diag->show();
}

void CalcWindow::toggleHypMode()
{
	hypMode = ! hypMode;
	tabSelect("funcpad");
}

void CalcWindow::buttonPressHandler()
{
	ClickableLabel *btn = qobject_cast<ClickableLabel*>(sender());

	QString command = btn->property("name").toString();
	buttonPress(command);
}

void CalcWindow::buttonPress(QString command)
{
	bool handled = true;

	if (command == "SI") {
		tabSelect("sipad");
	}
	else if (command == "hypmode") {
		toggleHypMode();
	}
	else if (command == "store") {
		storePressed();
	}
	else if (command == "recall") {
		recallPressed();
	}
	else if (command == "NOP") {
	}
	else if (command == "CANCEL") {
		cancelPressed();
	}
	else if (command == "MoreConversions") {
		moreConversions();
	}
	else if (command == "ConstByName") {
		constByName();
	}
	else {
		handled = false;
	}

	if (( ! handled) && (command.startsWith("Store"))) {
		handled = true;
		if (storeMode == "store") {
			calc.store(command.toStdString());
			saveSettings();
		}
		else if (storeMode == "recall") {
			calc.recall(command.toStdString());
		}
		else {
			cancelPressed();
		}
		tabSelect("numpad");
	}

	if ((command != "store") && (command != "recall")) {
		storeMode = "None";
	}

	if ( ! handled) {
		hypMode = false;
		qDebug() << "Running command " << command;
		ErrorCode result = calc.keypress(command.toStdString());

		//qDebug() << "Error code is " << result;

		handleErrorCode(result);

		if (returnToNumPad) {
			tabSelect("numpad");
		}
		else {
			tabSelect(lastTab);
		}
	}

	updateDisplays();
}

void CalcWindow::handleErrorCode(ErrorCode result)
{
	switch (result) {
		case NoFunction:
			showToast("Unknown function");
			break;
		case NotImplemented:
			showToast("Not implemented yet");
			break;
		case DivideByZero:
			showToast("Divide by zero error");
			break;
		case InvalidRoot:
			showToast("Invalid root error");
			break;
		case InvalidLog:
			showToast("Invalid log error");
			break;
		case InvalidTan:
			showToast("Invalid tangent error");
			break;
		case InvalidInverseTrig:
			showToast("Invalid inverse trigonometry error");
			break;
		case InvalidInverseHypTrig:
			showToast("Invalid inverse hyperbolic trigonometry error");
			break;
		case UnknownConstant:
			showToast("Unknown constant error");
			break;
		case UnknownConversion:
			showToast("Unknown conversion error");
			break;
		case InvalidConversion:
			showToast("Conversion error");
			break;
		case UnknownSI:
			showToast("Unknown SI unit");
			break;
		default:
			break;
	}
}

void CalcWindow::updateDisplays()
{
	QString xdisplay = QString::fromStdString(calc.getXDisplay());

	// TODO: Try removing <small> and </small>
	ui.lblEntry->setText(xdisplay);

	QString stackdisplay = QString::fromStdString(calc.getStackDisplay());
	ui.lblStack->setText(stackdisplay.replace("<small>", "").replace("</small>", ""));

	QString basedisplay = QString::fromStdString(calc.getBaseDisplay());
	ui.lblBase->setText(basedisplay);

	QString status_exponent = QString::fromStdString(calc.getStatusExponent());
	ui.lblStatusExponent->setText(status_exponent);

	QString status_base = QString::fromStdString(calc.getStatusBase());
	ui.lblStatusBase->setText(status_base);

	QString status_angular = QString::fromStdString(calc.getStatusAngularUnits());
	ui.lblStatusAngular->setText(status_angular);
}

void CalcWindow::tabSelect(QString tab)
{
	QString local_tab = tab;
	const QList<QString> store_tabs = { "romanupperpad", "romanlowerpad", "greekupperpad", "greeklowerpad" };
	const QString helpSuffix = " (double press to stay on this tab)";

	if ((local_tab == lastTab) && (local_tab != "numpad")) {
		returnToNumPad = false;
	}
	else {
		returnToNumPad = true;
	}

	if (store_tabs.contains(local_tab) || ((storeMode != "None") && (local_tab != "numpad"))) {
		returnToNumPad = true;
		ui.btnFuncPad->setIcon(QIcon(":/res/romanupper.png"));
		ui.btnConvPad->setIcon(QIcon(":/res/romanlower.png"));
		ui.btnConstPad->setIcon(QIcon(":/res/greekupper.png"));
		ui.btnOptPad->setIcon(QIcon(":/res/greeklower.png"));

		ui.btnNumPad->setToolTip("Go back to the main keypad");
		ui.btnFuncPad->setToolTip("Use Roman Upper Case storage locations");
		ui.btnConvPad->setToolTip("Use Roman Lower Case storage locations");
		ui.btnConstPad->setToolTip("Use Greek Upper Case storage locations");
		ui.btnOptPad->setToolTip("Use Greek Lower Case storage locations");

		if (local_tab == "funcpad") {
			local_tab = "romanupperpad";
		}
		else if (local_tab == "convtab") {
			local_tab = "romanlowerpad";
		}
		else if (local_tab == "constpad") {
			local_tab = "greekupperpad";
		}
		else if (local_tab == "optpad1") {
			local_tab = "greeklowerpad";
		}
		else {
		}

		lastStoreTab = local_tab;
	}
	else {
		storeMode = "None";

		ui.btnFuncPad->setIcon(QIcon(":/res/funcpad.png"));
		ui.btnConvPad->setIcon(QIcon(":/res/convpad.png"));
		ui.btnConstPad->setIcon(QIcon(":/res/constpad.png"));
		ui.btnOptPad->setIcon(QIcon(":/res/optpad.png"));

		ui.btnNumPad->setToolTip("Main keypad");
		ui.btnFuncPad->setToolTip("Maths functions" + helpSuffix);
		ui.btnConvPad->setToolTip("Unit conversion" + helpSuffix);
		ui.btnConstPad->setToolTip("Scientific constants" + helpSuffix);
		ui.btnOptPad->setToolTip("Configuration");

	}

	if (local_tab == "optpad1") {
		showOptPad1();
		lastTab = local_tab;
		return;
	}
	if (local_tab == "optpad2") {
		showOptPad2();
		lastTab = local_tab;
		return;
	}

	if (hypMode && (local_tab == "funcpad")) {
		local_tab = "hypfuncpad";
	}

	setupGrid(local_tab);

	ui.stkButtonPads->setCurrentIndex(0);

	lastTab = local_tab;
}

void CalcWindow::setupGrid(QString gridname)
{
	static QString hidden_button_style = ""
		"color: rgb(255, 255, 255);"
		"background-color: rgb(57, 56, 58);"
		"border: none;"
		+ btnFontSize
		;
	std::vector< std::vector<BI> > grid = calc.getGrid(gridname.toStdString());

	applyStyles();

	int buttoncount;
	for (int r=0; r<buttonArray.size();r++) {
		buttoncount = 6;
		std::vector<BI> row = grid[r];
		QHBoxLayout *lytRow = layoutArray[r];
		for (int c=0;c<buttonArray.size();c++) {
			ClickableLabel *btn = buttonArray[r][c];
			if ((c >= buttoncount) || (c >= (int) grid.size())) {
				btn->setVisible(false);
				lytRow->setStretch(c, 0);
				continue;
			}

			BI cell = row[c];
			btn->setText(QString::fromStdString(cell.display));
			btn->setProperty("name", QString::fromStdString(cell.name));
			if (cell.hidden == true) {
				btn->setStyleSheet(hidden_button_style);
			}
			btn->setVisible(true);
			QString helpText = QString::fromStdString(cell.helpText);
			std::vector<std::string> skeys = calc.getShortcutKeys(cell.name);
			QStringList shortcuts;
			shortcuts.reserve(skeys.size());
			for (const auto & s : skeys) {
				shortcuts.append(QString::fromStdString(s));
			}

			//qDebug() << QString::fromStdString(cell.name) << shortcuts;

			if ((shortcuts.size() > 0) && (helpText.length() > 0)) {
				helpText += " (";
				QString joined = shortcuts.join(" or ");
				helpText += joined + ")";
			}

			helpText.replace(QRegularExpression("</?small>"), "");

			if (helpText.length() > 0) {
				QTextDocument text;
				text.setHtml(helpText);

				btn->setToolTip(text.toHtml());
			}
			else {
				btn->setToolTip("");
			}

			if (cell.doubleWidth) {
				buttoncount -= 1;
				lytRow->setStretch(c, 2);
			}
			else {
				lytRow->setStretch(c, 1);
			}
		}
	}
}

void CalcWindow::showOptPad1()
{
	lastTab = "numpad";
	returnToNumPad = true;
	updateOptionIcons();
	ui.stkButtonPads->setCurrentIndex(1);
}

void CalcWindow::showOptPad2()
{
	lastTab = "numpad";
	returnToNumPad = true;
	updateOptionIcons();
	ui.stkButtonPads->setCurrentIndex(2);
}

QHash<QString, Option> CalcWindow::getOptions()
{
	std::map<std::string, Option> optListStd = calc.getOptions();
	// Convert to QHash so we work in "Qt world"
	// Probably inefficient, but that shouldn't matter here
	QHash<QString, Option> optList;
	for (auto const& [key, val] : optListStd) {
		optList[QString::fromStdString(key)] = val;
	}

	return optList;
}

void CalcWindow::optionClicked(QString name)
{
	QHash<QString, Option> optList = getOptions();

	if ( ! optList.contains(name)) {
		return;
	}

	optList[name].clickAction(&calc);
	updateOptionIcons();
	updateDisplays();
}

void CalcWindow::optionHelp(QString name)
{
	QHash<QString, Option> optList = getOptions();

	if ( ! optList.contains(name)) {
		return;
	}
	showToast(QString::fromStdString(optList[name].helpText), true);
}

void CalcWindow::updateOptionIcons()
{
	QHash<QString, Option> optList = getOptions();

	QHashIterator<QString, Option> o(optList);
	while (o.hasNext()) {
		o.next();
		QString name = o.key();
		Option opt = o.value();

		if (opt.hideOnPC) {
			continue;
		}
		QString newicon = ":/res/"
			+ QString::fromStdString(opt.iconAction(&calc)) + ".png";
		if ( ! optIcons.contains(name)) {
			qDebug() << "Missing entry in optIcons:" << name;
		}
		else {
			optIcons[name]->setIcon(QIcon(newicon));
		}
	}
	saveSettings();
}

void CalcWindow::createOptPad()
{
	static QString txt_style = ""
		"QLabel {"
			"color: rgb(255, 255, 255);"
			"background-color: rgb(57, 56, 58);"
			"border: none;"
			+ btnFontSize +
			"text-align: left;"
		"}"
		;
	static QString im_style = ""
		"QPushButton {"
			"color: rgb(255, 255, 255);"
			"background-color: rgb(57, 56, 58);"
			"border: none;"
			+ btnFontSize +
		"}"
		;
	static QString button_style = ""
		"QLabel {"
			"color: rgb(255, 255, 255);"
			"background-color: rgb(57, 56, 58);"
			+ btnBorderSize +
			"border-radius: 5px;"
			+ btnFontSize +
		"}"
		;
	QHash<QString, Option> optList = getOptions();

	QList<QGridLayout *> optGrids = {new QGridLayout(), new QGridLayout()};

	QHashIterator<QString, Option> o(optList);
	while (o.hasNext()) {
		o.next();
		QString name = o.key();
		Option opt = o.value();

		if (opt.hideOnPC) {
			continue;
		}

		int imCol = opt.location.col;
		int txtCol = imCol;

		if (txtCol != -1) {
			txtCol += 1;
		}

		if (name != "BLANK") {
			QPushButton *imLabel = new QPushButton();
			ClickableLabel *txtLabel = new ClickableLabel();

			txtLabel->setText(name);
			txtLabel->setTextFormat(Qt::RichText);
			txtLabel->setStyleSheet(txt_style);
			txtLabel->setProperty("name", name);

			QString helpText = QString::fromStdString(opt.helpText);
			helpText.replace(QRegularExpression("</?small>"), "");
			QTextDocument text;
			text.setHtml(helpText);

			txtLabel->setToolTip(text.toHtml());

			connect(txtLabel, SIGNAL(clicked()), this, SLOT(optionClicked()));

			imLabel->setStyleSheet(im_style);
			imLabel->setText("");
			imLabel->setFlat(true);
			imLabel->setIconSize(QSize(32, 32));
			imLabel->setProperty("name", name);
			imLabel->setToolTip(text.toHtml());
			optIcons[name] = imLabel;

			connect(imLabel, SIGNAL(clicked()), this, SLOT(optionClicked()));

			optGrids[opt.location.page]->addWidget(
					imLabel, opt.location.row, imCol);
			optGrids[opt.location.page]->addWidget(
					txtLabel, opt.location.row, txtCol);
		}
	}

	ClickableLabel *toOpt1Btn = new ClickableLabel();
	ClickableLabel *toOpt2Btn = new ClickableLabel();

	toOpt2Btn->setText("More...");
	toOpt2Btn->setTextFormat(Qt::RichText);
	toOpt2Btn->setStyleSheet(button_style);
	toOpt2Btn->setAlignment(Qt::AlignCenter);
	toOpt2Btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(toOpt2Btn, SIGNAL(clicked()), this, SLOT(toOptPad2()));

	toOpt1Btn->setText("Back...");
	toOpt1Btn->setTextFormat(Qt::RichText);
	toOpt1Btn->setAlignment(Qt::AlignCenter);
	toOpt1Btn->setStyleSheet(button_style);
	toOpt1Btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(toOpt1Btn, SIGNAL(clicked()), this, SLOT(toOptPad1()));

	optGrids[0]->addWidget(
			toOpt2Btn, 5, 4);
	optGrids[1]->addWidget(
			toOpt1Btn, 5, 4);

	const int buttonStretch = 2;
	const int textStretch = 10;

	QGridLayout *lyt;
	foreach (lyt, optGrids) {
		lyt->setColumnStretch(0, buttonStretch);
		lyt->setColumnStretch(1, textStretch);
		// Middle column is empty (to tie up with the way the Android app works)
		lyt->setColumnStretch(2, 0);
		lyt->setColumnStretch(3, buttonStretch);
		lyt->setColumnStretch(4, textStretch);
	}

	ui.optpad1->setLayout(optGrids[0]);
	ui.optpad2->setLayout(optGrids[1]);
}

void CalcWindow::nextTab()
{
	if (lastTab == "numpad") {
		tabSelect("funcpad");
	}
	else if ((lastTab == "funcpad") || (lastTab == "hypfuncpad")) {
		tabSelect("convpad");
	}
	else if (lastTab == "convpad") {
		tabSelect("constpad");
	}
	else if (lastTab == "constpad") {
		tabSelect("optpad1");
	}
	else if (lastTab == "optpad1") {
		tabSelect("optpad2");
	}
	else if (lastTab == "romanupperpad") {
		tabSelect("romanlowerpad");
	}
	else if (lastTab == "romanlowerpad") {
		tabSelect("greekupperpad");
	}
	else if (lastTab == "greekupperpad") {
		tabSelect("greeklowerpad");
	}
	else if (lastTab == "greeklowerpad") {
		tabSelect("romanupperpad");
	}
	else {
		cancelPressed();
	}
}

void CalcWindow::optionClicked()
{
	QWidget *btn = qobject_cast<QWidget*>(sender());
	QString name = btn->property("name").toString();
	optionClicked(name);
}

void CalcWindow::toOptPad1()
{
	tabSelect("optpad1");
}

void CalcWindow::toOptPad2()
{
	tabSelect("optpad2");
}

