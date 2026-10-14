#!/bin/bash
# Builds a single, statically-linked ARPCalc.exe against MXE's static Qt.
# This avoids loading ~30 DLLs at start-up (Qt, ICU, freetype, harfbuzz,
# glib, ...) and searching the Qt plugin folders.
PATH=$PATH:/opt/mxe/usr/bin

MXETARGET=x86_64-w64-mingw32.static

/opt/mxe/usr/${MXETARGET}/qt6/bin/qmake -o Makefile.dockerwinstatic "CONFIG+=dockerwin" && \
	make -f Makefile.dockerwinstatic clean && \
	make -f Makefile.dockerwinstatic && \
	bash zip_winrelease.sh dockerstatic
