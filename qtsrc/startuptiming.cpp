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

StartupTiming::StartupTiming(int argc, char *argv[])
{
	for (int i=1;i<argc;i++) {
		if (strcmp(argv[i], "--startup-timing") == 0) {
			enabled = true;
		}
	}
	if ( ! enabled) {
		return;
	}
	beforeMain = msSinceProcessCreation();
	clock.start();
	if (beforeMain >= 0) {
		stages << QString("main() %1 ms").arg(beforeMain);
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
	line += QString(" Qt %1, %2 style: ").arg(qVersion(), styleName);
	line += stages.join(", ");
	line += "\n";
	f.write(line.toUtf8());
}
