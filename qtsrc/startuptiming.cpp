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
#include <QtWidgets>
#include <cstring>
#include "startuptiming.h"

#ifndef ARPCALC_BUILD_NAME
#define ARPCALC_BUILD_NAME "unknown build"
#endif

#ifdef Q_OS_WIN
#include <QtCore/qt_windows.h>

static qint64 fileTimeToMs(const FILETIME &t)
{
	ULARGE_INTEGER v;
	v.LowPart = t.dwLowDateTime;
	v.HighPart = t.dwHighDateTime;
	return (qint64) (v.QuadPart / 10000); // 100 ns units
}

// Milliseconds since the process was created
static qint64 msSinceProcessCreation()
{
	FILETIME creation, exitTime, kernel, user, now;
	if ( ! GetProcessTimes(GetCurrentProcess(), &creation, &exitTime, &kernel, &user)) {
		return -1;
	}
	GetSystemTimeAsFileTime(&now);
	return fileTimeToMs(now) - fileTimeToMs(creation);
}

// CPU time used so far: if this is much less than the elapsed time, start-up
// is mostly waiting (for the disk, antivirus scanning, the network, ...)
static qint64 cpuTimeMs()
{
	FILETIME creation, exitTime, kernel, user;
	if ( ! GetProcessTimes(GetCurrentProcess(), &creation, &exitTime, &kernel, &user)) {
		return -1;
	}
	return fileTimeToMs(kernel) + fileTimeToMs(user);
}
#else
static qint64 msSinceProcessCreation()
{
	return -1;
}

static qint64 cpuTimeMs()
{
	return -1;
}
#endif

StartupTiming *StartupTiming::instance = NULL;

StartupTiming::StartupTiming(int argc, char *argv[])
{
	for (int i=1;i<argc;i++) {
		if (strcmp(argv[i], "--startup-timing") == 0) {
			enabled = true;
		}
		else {
			// Kept from here as QApplication removes the options it handles
			// (e.g. -platform windows:fontengine=gdi) from arguments()
			options << QString::fromLocal8Bit(argv[i]);
		}
	}
	if ( ! enabled) {
		return;
	}
	instance = this;
	beforeMain = msSinceProcessCreation();
	clock.start();
	if (beforeMain >= 0) {
		stages << QString("main() %1 ms").arg(beforeMain);
	}
}

StartupTiming::~StartupTiming()
{
	if (instance == this) {
		instance = NULL;
	}
}

void StartupTiming::markStage(QString stage)
{
	if (instance != NULL) {
		instance->mark(stage);
	}
}

void StartupTiming::mark(QString stage)
{
	if ( ! enabled) {
		return;
	}
	qint64 t = clock.elapsed() + qMax(beforeMain, (qint64) 0);
	stages << QString("%1 %2 ms").arg(stage).arg(t);
	// Before any style sheets are applied: after that, the style's name is
	// that of the style sheet style (which is empty)
	if (styleName.isEmpty() && (qobject_cast<QApplication *>(QCoreApplication::instance()) != NULL)) {
		styleName = QApplication::style()->name();
	}
}

void StartupTiming::probe()
{
	if ( ! enabled) {
		return;
	}

	// Enumerating the installed fonts
	QFontDatabase::families();
	mark("font database");

	// Every non-ASCII character that the buttons and displays can show
	const QString symbols = QString::fromUtf8(
				"<>£°±·Å×÷łΑΒΓΔΕ"
				"ΖΗΘΙΚΛΜΝΞΟΠΡΣΤ"
				"ΥΦΧΨΩαβγδεζηθι"
				"κλμνξοπρστυφχψ"
				"ω –€←↑→↓⇔√∞");

	// Which of them the main font doesn't have (and so need a fallback
	// font).  The font used to check doesn't do any font fallback itself.
	QFont mainFont(QApplication::font().families().value(0));
	mainFont.setStyleStrategy(QFont::NoFontMerging);
	QFontMetrics mainMetrics(mainFont);
	QString missing;
	for (QChar c : symbols) {
		if ( ! mainMetrics.inFont(c)) {
			missing += c;
		}
	}
	mark("glyph check");
	stages << QString("(missing from %1: %2)").arg(mainFont.family(),
			missing.isEmpty() ? QString("none") : missing);

	// Loading the font itself
	QFontMetrics metrics(QApplication::font());
	metrics.horizontalAdvance("0123456789 ENTER CLEAR");
	mark("font metrics");

	// The symbols, which may need fallback fonts
	metrics.horizontalAdvance(symbols);
	mark("symbol fonts");

	// Rich text, as the buttons and displays use, at another size
	QTextDocument doc;
	QFont big = QApplication::font();
	big.setPointSize(16);
	doc.setDefaultFont(big);
	doc.setHtml("1.234&nbsp;567<sup><small>-3</small></sup> &times; &larr;");
	doc.size();
	mark("rich text");

	// Style sheets like the window's
	{
		QWidget window;
		window.setStyleSheet("background-color: rgb(57, 56, 58);");
		QLabel *label = new QLabel("x", &window);
		label->setStyleSheet("QLabel { color: rgb(255, 255, 255); "
				"border: 1px solid #ffffff; border-radius: 5px; font-size: 10pt; }");
		QPushButton *button = new QPushButton(&window);
		label->ensurePolished();
		button->ensurePolished();
		label->sizeHint();
		button->sizeHint();
	}
	mark("style sheets");

	// The tab button icons
	QIcon(":/res/numpad.png").pixmap(90, 90);
	mark("icon");
}

void StartupTiming::logAfterFirstPaint(QWidget *window)
{
	if (enabled) {
		window->installEventFilter(this);
	}
}

bool StartupTiming::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::Paint) {
		object->removeEventFilter(this);
		mark("first paint");
		qint64 cpu = cpuTimeMs();
		if (cpu >= 0) {
			stages << QString("(CPU time %1 ms)").arg(cpu);
		}
		write();
	}
	return false;
}

void StartupTiming::write()
{
	QString dir = QCoreApplication::applicationDirPath();
	if ( ! QFileInfo(dir).isWritable()) {
		dir = QDir::tempPath();
	}
	QFile f(QDir(dir).filePath("startup-timing.log"));
	if ( ! f.open(QIODevice::Append | QIODevice::Text)) {
		return;
	}
	QString line = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	// Which build this is (set by the build scripts, e.g. dockerwinstatic)
	line += " " ARPCALC_BUILD_NAME;
#ifdef __VERSION__
	line += " (GCC " __VERSION__ ")";
#endif
	line += QString(", Qt %1, %2 style").arg(qVersion(), styleName);
	// Any other options, so that runs with different options can be told apart
	if ( ! options.isEmpty()) {
		line += " [" + options.join(" ") + "]";
	}
	line += ": ";
	line += stages.join(", ");
	line += "\n";
	f.write(line.toUtf8());
}
