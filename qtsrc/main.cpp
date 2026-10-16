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
#include <QFont>
#include <QTimer>
#include "crashlog.h"
#include "calcwindow.h"
#include "startuptiming.h"

extern const char * version;

#ifdef Q_OS_WIN
#include <thread>
#include <QtCore/qt_windows.h>
#include <d3d9.h>

// Qt's Windows platform plugin initialises Direct3D 9 the first time a window
// is created (it uses it to identify the GPU).  On a laptop with two GPUs this
// loads both vendors' drivers, which takes ~0.4 s when warm and several
// seconds on the first start after a reboot.  Doing the same thing on a
// worker thread, before the main thread gets there, overlaps that cost with
// the font set-up that the main thread has to do anyway.
static void prewarmDirect3D()
{
	HMODULE lib = LoadLibraryA("d3d9.dll");
	if (!lib) {
		return;
	}
	using Create9 = IDirect3D9 *(WINAPI *)(UINT);
	auto create = reinterpret_cast<Create9>(
			reinterpret_cast<void *>(GetProcAddress(lib, "Direct3DCreate9")));
	if (!create) {
		return;
	}
	if (IDirect3D9 *d3d = create(D3D_SDK_VERSION)) {
		D3DADAPTER_IDENTIFIER9 id;
		d3d->GetAdapterIdentifier(0, 0, &id);  // what qwindows does
		d3d->Release();
	}
}
#endif

int main(int argc, char *argv[])
{
	StartupTiming timing(argc, argv);

#ifdef Q_OS_WIN
	std::thread(prewarmDirect3D).detach();
#endif

	installCrashLogger(version);

	// Creates an instance of QApplication
	QApplication a(argc, argv);
	timing.mark("QApplication");

	// ARPCalc Sans (Noto Sans plus the arrows and maths symbols from Noto Sans
	// Math: see tools/make_font.py) is built in, so the app looks the same
	// everywhere.  It has every character the app shows, so Qt never needs
	// to look for a fallback font, which on Windows means reading every
	// installed font (~0.5 s with ~1100 fonts).  For the same reason, give a
	// single family rather than a list: for a list, Qt builds its list of
	// fallback fonts as soon as the font is loaded.
	QFont f;
	if (QFontDatabase::addApplicationFont(":/fonts/ARPCalcSans-Regular.ttf") >= 0) {
		f.setFamily("ARPCalc Sans");
		a.setFont(f);
	}
#ifdef Q_OS_WIN
	else {
		// Has everything except the swap button's double arrow
		f.setFamily("Segoe UI");
		a.setFont(f);
	}
#endif
	timing.mark("font");

	// Tooltips pick up the main window's dark background (its style sheet's
	// background-color has no selector, so applies to them too) but not a
	// text colour, so with recent Qt versions they were black on dark grey.
	// Set the colours explicitly: the style sheet for styles that honour it
	// and the palette for any that don't.  Done before any widgets exist so
	// that nothing needs to be re-polished.
	QPalette tipPalette = QToolTip::palette();
	tipPalette.setColor(QPalette::ToolTipText, QColor(255, 255, 255));
	tipPalette.setColor(QPalette::ToolTipBase, QColor(57, 56, 58));
	QToolTip::setPalette(tipPalette);
	a.setStyleSheet(""
		"QToolTip {"
			"color: rgb(255, 255, 255);"
			"background-color: rgb(57, 56, 58);"
			"border: 1px solid rgb(255, 255, 255);"
		"}"
		);

	timing.probe();

	// This is our MainWidget class containing our GUI and functionality
	CalcWindow w;
	timing.mark("window created");
	timing.logAfterFirstPaint(&w);
	w.show(); // Show main window
	timing.mark("shown");

	QTimer::singleShot(0, &w, &CalcWindow::startNetworkOperations);

	// run the application and return execs() return value/code
	return a.exec();
}
