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
#ifndef STARTUPTIMING_H
#define STARTUPTIMING_H

#include <QElapsedTimer>
#include <QObject>
#include <QStringList>

class QWidget;

// When ARPCalc is run with --startup-timing, records how long each stage of
// start-up took and, once the window has first been painted, appends a line
// to startup-timing.log (next to the executable if that folder is writable,
// otherwise in the temporary folder).  On Windows the times are measured
// from when the process was created, so include the time taken to load the
// DLLs (and for any antivirus software to scan them).
class StartupTiming : public QObject
{
	public:
		StartupTiming(int argc, char *argv[]);
		~StartupTiming();
		void mark(QString stage);
		void logAfterFirstPaint(QWidget *window);

		// Records a stage from elsewhere in the app (if enabled)
		static void markStage(QString stage);

	protected:
		bool eventFilter(QObject *object, QEvent *event) override;

	private:
		void write();

		bool enabled = false;
		QElapsedTimer clock;
		qint64 beforeMain = -1; // ms from process creation to this object
		QStringList stages;
		QString styleName;
		QStringList options;

		static StartupTiming *instance;
};

#endif // STARTUPTIMING_H
