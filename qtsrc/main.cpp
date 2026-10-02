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
#include "calcwindow.h"

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
#ifdef Q_OS_WIN
	std::thread(prewarmDirect3D).detach();
#endif

	// Creates an instance of QApplication
	QApplication a(argc, argv);

#ifdef Q_OS_WIN
	// Explicit font list (Windows only: these families don't exist elsewhere,
	// and on other platforms the desktop's own font should be left alone).
	// Used to speed up start-up
	QFont f;
	f.setFamilies({"Segoe UI", "Segoe UI Symbol", "Cambria Math"});
	a.setFont(f);
#endif

	// This is our MainWidget class containing our GUI and functionality
	CalcWindow w;
	w.show(); // Show main window

	QTimer::singleShot(0, &w, &CalcWindow::startNetworkOperations);

	// run the application and return execs() return value/code
	return a.exec();
}
