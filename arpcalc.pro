TEMPLATE = app
TARGET = ARPCalc
INCLUDEPATH += .
INCLUDEPATH += ./inc
INCLUDEPATH += ./qtinc

# For debug output:
#CONFIG += console

win32 {
	RC_FILE = ./res/icon.rc
	PYTHON = python
}

CONFIG(dockerwin) {
	LIBS += -lgmp -lmpfr

	PYTHON = python3
	CONFIG += c++latest

	# TODO: tidy up the output files
	OBJECTS_DIR = generated_files/dockerwin
	MOC_DIR = generated_files/dockerwin
	RCC_DIR = generated_files/dockerwin
	UI_DIR = generated_files/dockerwin
	DESTDIR = output/dockerwin

	CONFIG += link_pkgconfig
	PKGCONFIG += libzip
}
else {
	win32 {
		INCLUDEPATH += ./libs/x64/include
		LIBS += ./libs/x64/lib/libmpfr.a ./libs/x64/lib/libgmp.a
		CONFIG += c++latest

		console {
			OBJECTS_DIR = generated_files/console
			MOC_DIR = generated_files/console
			RCC_DIR = generated_files/console
			UI_DIR = generated_files/console
			DESTDIR = output/winconsole
		}
		else {
			OBJECTS_DIR = generated_files/release
			MOC_DIR = generated_files/release
			UI_DIR = generated_files/release
			RCC_DIR = generated_files/release
			DESTDIR = output/winrelease
		}
	}
	linux {
		LIBS += -lgmp -lmpfr
		CONFIG += c++20
		QMAKE_CXXFLAGS += -std=c++20

		PYTHON = python3
		OBJECTS_DIR = generated_files/linux
		MOC_DIR = generated_files/linux
		UI_DIR = generated_files/linux
		RCC_DIR = generated_files/linux
		DESTDIR = output/linux

		target.path = output/AppDir/usr/bin
		INSTALLS += target
	}
}

LIBS += -lzip

QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]
QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]/QtNetwork

getchangeset.commands = $$PYTHON get_changeset.py

QMAKE_EXTRA_TARGETS += getchangeset
PRE_TARGETDEPS += getchangeset

CONFIG -= debug_and_release debug_and_release_target
CONFIG += release


QT += widgets
QT += network

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += \
	inc/arpfloat.h \
	inc/commands.h \
	inc/stack.h \
	inc/strutils.h \
	qtinc/calcwindow.h \
	qtinc/choicewindow.h \
	qtinc/clickablelabel.h
SOURCES += \
	src/arpfloat.cpp \
	src/changeset.cpp \
	src/commands.cpp \
	src/conversion.cpp \
	src/grids.cpp \
	src/keys.cpp \
	src/ops.cpp \
	src/si.cpp \
	src/stack.cpp \
	src/strutils.cpp \
	qtsrc/main.cpp \
	qtsrc/calcwindow.cpp \
	qtsrc/choicewindow.cpp \
	qtsrc/clickablelabel.cpp
FORMS += ui/qcalcwindow.ui
RESOURCES = resources.qrc
